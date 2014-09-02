(*
 * This option changes floating point unary and binary operations of all 
 * fkind types to call a function e.g. float _floatadd(float, float).
 * It can be used to intercept floating point calls and change their
 * behaviour. It should be used in conjunction with dosimplify
 * e.g. cilly --dosimplify --dofphooks -c file.c
 *
 * Philip H.W. Leong <firstname.lastname@sydney.edu.au> Oct 2012 (phwl)
 *
 * Copyright (c) 2001-2002, 
 *  George C. Necula    <necula@cs.berkeley.edu>
 *  Scott McPeak        <smcpeak@cs.berkeley.edu>
 *  Wes Weimer          <weimer@cs.berkeley.edu>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. The names of the contributors may not be used to endorse or promote
 * products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *)

open Pretty
open Cil
module E = Errormsg
module H = Hashtbl
  
(* need to fix prototypes are they are incorrect *)
let fixargs args =
  let p = [ ("oi", intType, []); 
            ("lf", charPtrType, []);
            ("ll", intType, []) ] in
    args @ p

(* create a generic hook function *)
let mkFPFun fk op args =
  let mkhook ftype fname fargs =
    let fdec = emptyFunction fname in
      fdec.svar.vtype <- TFun(TFloat(ftype, []), fargs, true, []); fdec and
      fname = (match fk with
                FDouble -> "_double" 
              | FFloat -> "_float" 
              | FLongDouble -> "_longdouble") ^ op in
        mkhook fk fname (args)

(* create a hook function for binary fp *)
let binfpFun bop fkind =   
  let op = match bop with
             PlusA -> "add"
           | MinusA -> "sub"
           | Mult -> "mul"
           | Div -> "div" 
           | _ -> "error" and
      arg = Some [  ("a", doubleType, []); ("b", doubleType, []) ] in
        mkFPFun fkind op arg

(* create a hook function for binary fp comparison *)
let binfpcompFun bop fkind =   
  let op = match bop with
             Eq -> "eq"
           | Ne -> "ne"
           | Lt -> "lt"
           | Gt -> "gt" 
           | Le -> "le"
           | Ge -> "ge" 
           | _ -> "error" in
  let arg = Some [ ("a", doubleType, []); ("b", doubleType, []) ] in
  mkFPFun fkind op arg

(* create a unique label for a function call *)
let opid = ref 0;;
    
(* do a Call but append a unique op id, file name, and line number to args *)
let lCall(lv,v,e,l) =
  opid := !opid + 1;
  let al = [ Const(CInt64(Int64.of_int(!opid), IInt, None));
             mkString(l.file);
             Const(CInt64(Int64.of_int(l.line), IInt, None)) ] in
    Call(lv, v, e @ al, l)

(* create a hook function for unary fp *)
let unfpFun uop fkind =   
  let op = match uop with
              Neg -> "neg" 
           | _ -> "error" in
  let arg = Some [ ("a", doubleType, []) ] in
  mkFPFun fkind op arg

(** A visitor that will turn all expressions into tmp vars *)
class fphookVisitor (fi: fundec) = object (self)
  inherit nopCilVisitor

  method vinst (i: instr) : instr list visitAction = 
    match i with 
      (* simple floating point binary op *)
    | Set(lv, BinOp(b, e1, e2, TFloat(fkind, _)), l) -> 
        let f = binfpFun b fkind in
          ChangeTo 
          [ lCall(Some lv, (Lval(Var(f.svar),NoOffset)), [ e1 ; e2 ], l); ]

      (* simple floating point binary comparison op *)
    | Set(lv, BinOp(b, e1, e2, t), l) -> 
        let isFPType t = 
          match typeOf t with
            (TFloat _) -> true
          | _ -> false in
        if (isFPType(e1)) then
          let fk = match(typeOf(e1)) with
            TFloat(fkind, _) -> fkind 
          | _ -> FDouble in			(* phwl - make double if there is no type *)
          let f = binfpcompFun b fk in
            ChangeTo 
            [ lCall(Some lv, (Lval(Var(f.svar),NoOffset)), [ e1 ; e2 ], l); ]
        else SkipChildren

      (* simple unary op *)
    | Set(lv, UnOp(u, e1, TFloat(fkind, _)), l) -> begin
        let f = unfpFun u fkind in
          ChangeTo 
          [ lCall(Some lv, (Lval(Var(f.svar),NoOffset)), [ e1 ], l); ]
      end 

    | _ -> SkipChildren

end

(* Insert floating point hooks into C code *)
let doit (f: file) =
  f.globals <- GText ("#include \"mcalib.h\"") :: f.globals;
  f.globals <- GText ("/* fphooks 1.0 (phwl) */") :: f.globals;
  let doGlobal = function 
    GFun(fi, _) ->  
      let v = new fphookVisitor fi in
      fi.sbody <- visitCilBlock v fi.sbody;
    | _ -> () in
    iterGlobals f doGlobal

let feature : featureDescr = 
  { fd_name = "fphooks";
    fd_enabled = ref false;
    fd_description = "generation of floating point hooks";
    fd_extraopt = [];
    fd_doit = doit;
    fd_post_check = true;
  } 

