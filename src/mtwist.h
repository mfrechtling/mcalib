#ifndef MCALIB_MRSTWIST
#define MCALIB_MRSTWIST
#include <stdio.h>
#include <stdlib.h>
#include "time.h"

//
// uint32 must be an unsigned integer type capable of holding at least 32
// bits; exactly 32 should be fastest, but 64 is better on an Alpha with
// GCC at -O3 optimization so try your options and see what's best for you
//

typedef unsigned long uint32;

#define N              (624)                 // length of state vector
#define M              (397)                 // a period parameter
#define K              (0x9908B0DFU)         // a magic constant
#define hiBit(u)       ((u) & 0x80000000U)   // mask all but highest   bit of u
#define loBit(u)       ((u) & 0x00000001U)   // mask all but lowest    bit of u
#define loBits(u)      ((u) & 0x7FFFFFFFU)   // mask     the highest   bit of u
#define mixBits(u, v)  (hiBit(u)|loBits(v))  // move hi bit of u to hi bit of v

typedef struct mtwist	mtwist;

struct mtwist {
	uint32   *state;	// state vector + 1 extra to not violate ANSI C
	uint32   *next;		// next random value is computed from here
	int      left;		// can *next++ this many times before reloading
};

mtwist* mcalib_new_mtwist(void);
void seedMT(uint32 seed, mtwist *mca_twist);
uint32 reloadMT(mtwist *mca_twist);
uint32 randomMT(mtwist *mca_twist);
#endif
