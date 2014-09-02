(*
 * This option changes floating point unary and binary operations of all 
 * fkind types to call a function e.g. float _floatadd(float, float).
 * It can be used to intercept floating point calls and change their
 * behaviour. It should be used in conjunction with dosimplify
 * e.g. cilly --dosimplify --dofpsimplifys -c file.c
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

(** A visitor that will turn all expressions into tmp vars *)
class fpsimplifyVisitor (fi: fundec) = object (self)
  inherit nopCilVisitor

  method private makeTemp (e1: exp) : exp = 
    let t = makeTempVar fi (typeOf e1) in
    (* Add this instruction before the current statement *)
      self#queueInstr [Set(var t, e1, !currentLoc)];
      Lval(var t)

  (* change binary ops to temporary variables 
   * ugly - can reduce redundant copies with a bit more code *)
  method vexpr (e: exp) = 
    match e with 
      BinOp(bo, e1, e2, tres) -> 
        ChangeTo (self#makeTemp(e))
    | _ -> SkipChildren

end

(* Insert floating point hooks into C code *)
let doit (f: file) =
  let doGlobal = function 
    GFun(fi, _) ->  
      let v = new fpsimplifyVisitor fi in
      fi.sbody <- visitCilBlock v fi.sbody;
    | _ -> () in
    iterGlobals f doGlobal

let feature : featureDescr = 
  { fd_name = "fpsimplify";
    fd_enabled = ref false;
    fd_description = "simplify fp conditionals";
    fd_extraopt = [];
    fd_doit = doit;
    fd_post_check = true;
  } 

