# The Monte Carlo Arihmetic Library - A tool for automated rounding error
# analysis of floating point software. Copyright (C) 2014 The Computer
# Engineering Laboratory, The University of Sydney. Maintained by Michael
# Frechtling:
# 
# 	michael.frechtling@sydney.edu.au
# 
# This file is part of the Monte Carlo Arithmetic Library, (MCALIB). MCALIB is
# free software: you can redistribute it and/or modify it under the terms of
# the GNU General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
# 
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.

CC = gcc
CL = cilly
CFLAGS = -O2
CLFLAGS = --save-temps --dofphooks --dosimplify --dooneRet --dofpsimplify
CLIBS = -lm
CLLIBS = -lm -lmca

all:		knuth_mca

knuth:		knuth.c
		$(CC) $(CFLAGS) knuth.c -o knuth $(CLIBS)

knuth_mca:	knuth_mca.c
		$(CL) $(CLFLAGS) knuth_mca.c -o knuth $(CLLIBS)

knuth_qmca:	
		make knuth_mca; ./mcalib_fix.py knuth_mca.cil.c
		mv knuth_mca.cil.c.new knuth_mca.cil.c
		gcc -D_GNUCC -E ./knuth_mca.cil.c -o ./knuth_mca.cil.i
		gcc -D_GNUCC -c -o ./knuth_mca.o ./knuth_mca.cil.i
		gcc knuth_mca.o -o knuth_mca -lm -lmca


.PHONY:		clean

clean:	
		rm -rf knuth_mca *.cil.c *.i *.o
