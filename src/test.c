#include "stdlib.h"
#include "stdio.h"
#include "mcalib.h"

#define floatval_t		double
#define NEAREST_FLOAT(x)	(floatval_t)(x)

void test(int n) {
	floatval_t a = 0., b = 1, c = 1;
	MCALIB_OP_TYPE = MCALIB_OP_MCA;
	MCALIB_RNG_TYPE = MCALIB_RNG_UNFM;

	int i;
	for (i = 0; i < n; i++) {
		printf("RESULT, %d, %d, %.16e\n", n, MCALIB_T, b + c);
	}
}

void knuth_test(int n) {
	floatval_t x, y, z, u, v;
	x = NEAREST_FLOAT(11111113.);
	y = NEAREST_FLOAT(-11111111.);
	z = NEAREST_FLOAT(7.5111111);
	MCALIB_RNG_TYPE = MCALIB_RNG_UNFM;
	int i;
	for (i = 0; i < n; i++) {
		MCALIB_OP_TYPE = MCALIB_OP_MCA;
		u = x + (y + z);
		v = (x + y) + z;
		MCALIB_OP_TYPE = MCALIB_OP_IEEE;
		printf("RESULT, %d, %d, %.16e\n", n, MCALIB_T, (double)(u - v));
	}
}

int main(int argc, char *argv[]) {
	if (argc != 3) return 0;
	MCALIB_T = atoi(argv[1]);
	_mca_init();
	knuth_test(atoi(argv[2]));
	_mca_clear();
	return 0;
}
