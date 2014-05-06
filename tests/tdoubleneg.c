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

#include <stdlib.h>
#include "mcalib.h"
#include "math.h"

#define floatval_t	double

floatval_t get_rand(void) {
	floatval_t ret;
	while(1) {
		ret = rand() / (floatval_t)RAND_MAX;
		if ((ret > 0.) && (ret < 1.0)) {
			return (ret - 0.5);
		}
	}
}

int main(void) {
	MCALIB_OP_TYPE = MCALIB_MCA;
	MCALIB_T = 0;
	floatval_t a = 1.0, eps_a, eps_r, check, test;
	srand(0L);
	eps_a = pow(2, floor(log2(a)) + 2) * get_rand();
	check = (-(a + eps_a));
	eps_r = pow(2, floor(log2(abs(check))) + 2) * get_rand();
	check += eps_r;
	srand(0L);
	test = _doubleneg(1.0);
	if (test != check) {
		exit(1);
	}
	return 0;
}
