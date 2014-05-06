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

#define floatval_t	long double

int main(void) {
	floatval_t check = 1.0 * 1.0;
	floatval_t test = _longmul(1.0, 1.0);
	if (test != check) {
		exit(1);
	}
	return 0;
}