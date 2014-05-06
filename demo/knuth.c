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

#define	floatval_t	double

void knuth_test(void) {
	floatval_t a = 11111113., b = -11111111, c = 7.5111111;
	floatval_t u, v;
	u = (a +  b) + c;
	v = a + (b + c);
	printf("U = %g\tV = %g\n", u, v);
}

int main(void) {
	knuth_test();
	return 0;
}
