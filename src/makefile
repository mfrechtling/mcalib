CC      = gcc
LL      = ld
CFLAGS  = -I.
LFLAGS  = -r
LIB     = -lm -lmpfr -lgmp -lrt -lgsl -lgslcblas

CL = cilly
CLFLAGS= -g -pg --save-temps --dofphooks --dosimplify --dooneRet --dofpsimplify
CLLIBS = -lm -lmpfr -lgmp -lrt -lgsl -lgslcblas


all:            libmcalib

lu_table:    	lookup_table.c
		$(CC) $(CFLAGS) -c lookup_table.c -o lookup_table.o $(LIB)

mcalib:         lu_table mcalib.c
		$(CC) $(CFLAGS) -c mcalib.c -o mcalib.o $(LIB)

libmcalib:      mcalib
		$(LL) $(LFLAGS) mcalib.o lookup_table.o -o libmcalib.a

test:		libmcalib test.c
		$(CL) $(CLFLAGS) libmcalib.a test.c -o test $(CLLIBS)
		

clean:
	-rm -f *.a *~ *.o *.cil* *.i

