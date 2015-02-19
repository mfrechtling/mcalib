// The Monte Carlo Arihmetic Library - A tool for automated rounding error
// analysis of floating point software. Copyright (C) 2014 The Computer
// Engineering Laboratory, The University of Sydney. Maintained by Michael
// Frechtling:
// 
// 	michael.frechtling@sydney.edu.au
// 
// This file is part of the Monte Carlo Arithmetic Library, (MCALIB). MCALIB is
// free software: you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
// 
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.

#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "mcalib.h"

#define	floatval_t	double
static int N;

void knuth_test(void) {
	floatval_t a = 11111113., b = -11111111, c = 7.5111111;
	floatval_t u, v;
	u = (a +  b) + c;
	v = a + (b + c);
	printf("%d, %d, %d, %.16e, %.16e\n", N, MCALIB_RNG_TYPE, MCALIB_T, u, v);
}

int main(int argc, char *argv[]) {
	if (argc == 3) {
		MCALIB_T = atoi(argv[1]);
		N = atoi(argv[2]);
	} else {
		MCALIB_T = 24;
		N = 100;
	}
	_mca_init(4, N);
	MCALIB_OP_TYPE = MCALIB_OP_MCA;
	int i, j;
	for (i = 0; i < 3; i++) {
		MCALIB_RNG_TYPE = i;
		for (j = 0; j < N; j++) {
			knuth_test();
		}
	}
	MCALIB_OP_TYPE = MCALIB_OP_IEEE;
	return 0;
}
