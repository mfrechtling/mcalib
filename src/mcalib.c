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


#include "mcalib_types.h"

#define NEAREST_FLOAT(x)	((float) (x))
#define	NEAREST_DOUBLE(x)	((double) (x))

int 	MCALIB_OP_TYPE 		= MCALIB_OP_IEEE;
int 	MCALIB_RNG_TYPE		= MCALIB_RNG_UNFM;
int 	MCALIB_T		= 24;

mcalib_antithetic *_mca_atrng;
struct mcalib_list *MCALIB_LUT = NULL;

/******************** MCA RANDOM FUNCTIONS ********************
* The following functions are used to calculate the random
* perturbations used for MCA and apply these to MPFR format
* operands
***************************************************************/	

double _mca_runf(void) {
	while(1) {
		double d_rand = ((double)rand() / (double)RAND_MAX);
		if ((d_rand > 0.) & (d_rand < 1.0)) {
			return d_rand;
		}
	}
}

double _mca_atrand(void) {
	if (_mca_atrng->index == 0) {
		_mca_atrng->rand[0] = _mca_runf();
		_mca_atrng->rand[1] = (0. - (_mca_atrng->rand[0] - 0.5)) + 0.5;
		_mca_atrng->index = 1;
	} else {
		_mca_atrng->index = 0;
	}
	return _mca_atrng->rand[1 - _mca_atrng->index];
}

double _mca_qrand(mcalib_index *index) {
	double ret;
	if (MCALIB_LUT == NULL) {
		MCALIB_LUT = mcalib_new_list();
	}
	mcalib_get_data(MCALIB_LUT, index, &ret);
	return ret;
}

double _mca_rand(mcalib_index *index) {
	switch(MCALIB_RNG_TYPE) {
		case MCALIB_RNG_UNFM :
			mcalib_clear_index(index);
			return _mca_runf();
		case MCALIB_RNG_ANT :
			mcalib_clear_index(index);
			return _mca_atrand();
		case MCALIB_RNG_SBL :
			return _mca_qrand(index);
	}
}

int _mca_inexact(mpfr_ptr a, mpfr_rnd_t rnd_mode, mcalib_index *index) {
	if (MCALIB_OP_TYPE == MCALIB_OP_IEEE) {
		mcalib_clear_index(index);
		return 0;
	}
	mpfr_exp_t e_a = mpfr_get_exp(a);
	mpfr_prec_t p_a = mpfr_get_prec(a);
	mpfr_t mpfr_rand, mpfr_offset, mpfr_zero;
	e_a = e_a - (MCALIB_T - 1);
	mpfr_inits2(p_a, mpfr_rand, mpfr_offset, mpfr_zero, (mpfr_ptr) 0);
	mpfr_set_d(mpfr_zero, 0., rnd_mode);
	int cmp = mpfr_cmp(a, mpfr_zero);
	if (cmp == 0) {
		mcalib_clear_index(index);
		mpfr_clear(mpfr_rand);
		mpfr_clear(mpfr_offset);
		mpfr_clear(mpfr_zero);
		return 0;
	}
	double d_rand = (_mca_rand(index) - 0.5);
	double d_offset = pow(2, e_a);
	mpfr_set_d(mpfr_rand, d_rand, rnd_mode);
	mpfr_set_d(mpfr_offset, d_offset, rnd_mode);
	mpfr_mul(mpfr_rand, mpfr_rand, mpfr_offset, rnd_mode);
	mpfr_add(a, a, mpfr_rand, rnd_mode);
	mpfr_clear(mpfr_rand);
	mpfr_clear(mpfr_offset);
	mpfr_clear(mpfr_zero);
}

void _mca_init(void) {
	struct timeval t1;
	gettimeofday(&t1, NULL);
	srand(t1.tv_usec * t1.tv_sec);
	_mca_atrng = (mcalib_antithetic*)malloc(sizeof(mcalib_antithetic));
	_mca_atrng->rand = (double*)malloc(2 * sizeof(double));
	_mca_atrng->index = 0;
}

void _mca_clear(void) {
	mcalib_clear_list(MCALIB_LUT);
	free(_mca_atrng->rand);
	free(_mca_atrng);
}

void make_index(mcalib_index *index, unsigned id, char *file_name, unsigned line_number) {

}

/******************** MCA ARITHMETIC FUNCTIONS ********************
* The following set of functions perform the MCA operation. Operands
* are first converted to MPFR format, inbound and outbound perturbations
* are applied using the _mca_inexact function, and the result converted
* to the original format for return
*******************************************************************/

float _mca_sbin(float a, float b, unsigned id, char *file_name, unsigned line_number, mpfr_bin mpfr_op) {
	mpfr_t mpfr_a, mpfr_b, mpfr_r;
	mpfr_prec_t prec = 24 + MCALIB_T;
	mpfr_rnd_t rnd = MPFR_RNDN;
	mpfr_inits2(prec, mpfr_a, mpfr_b, mpfr_r, (mpfr_ptr) 0);
	mpfr_set_flt(mpfr_a, a, rnd);
	mpfr_set_flt(mpfr_b, b, rnd);
	if (MCALIB_OP_TYPE != MCALIB_OP_RR) {
		mcalib_index *index_a, *index_b;
		index_a = mcalib_new_index();
		make_index(index_a, ((id << 4) + 0xa), file_name, line_number);
		index_b = mcalib_new_index();
		make_index(index_b, ((id << 4) + 0xb), file_name, line_number);
		_mca_inexact(mpfr_a, rnd, index_a);
		_mca_inexact(mpfr_b, rnd, index_b);
	}
	mpfr_op(mpfr_r, mpfr_a, mpfr_b, rnd);
	if (MCALIB_OP_TYPE != MCALIB_OP_PB) {
		mcalib_index *index_r;
		index_r = mcalib_new_index();
		make_index(index_r, ((id << 4) + 0xc), file_name, line_number);
		_mca_inexact(mpfr_r, rnd, index_r);
	}
	float ret = mpfr_get_flt(mpfr_r, rnd);
	mpfr_clear(mpfr_a);
	mpfr_clear(mpfr_b);
	mpfr_clear(mpfr_r);
	return NEAREST_FLOAT(ret);
}

float _mca_sunr(float a, unsigned id, char *file_name, unsigned line_number, mpfr_unr mpfr_op) {
	mpfr_t mpfr_a, mpfr_r;
	mpfr_prec_t prec = 24 + MCALIB_T;
	mpfr_rnd_t rnd = MPFR_RNDN;
	mpfr_inits2(prec, mpfr_a, mpfr_r, (mpfr_ptr) 0);
	mpfr_set_flt(mpfr_a, a, rnd);
	if (MCALIB_OP_TYPE != MCALIB_OP_RR) {
		mcalib_index *index_a;
		index_a = mcalib_new_index();
		make_index(index_a, ((id << 4) + 0xa), file_name, line_number);
		_mca_inexact(mpfr_a, rnd, index_a);
	}
	mpfr_op(mpfr_r, mpfr_a, rnd);
	if (MCALIB_OP_TYPE != MCALIB_OP_PB) {
		mcalib_index *index_r;
		index_r = mcalib_new_index();
		make_index(index_r, ((id << 4) + 0xc, file_name, line_number);
		_mca_inexact(mpfr_r, rnd, index_r);
	}
	float ret = mpfr_get_flt(mpfr_r, rnd);
	mpfr_clear(mpfr_a);
	mpfr_clear(mpfr_r);
	return NEAREST_FLOAT(ret);
}

double _mca_dbin(double a, double b, unsigned id, char *file_name, unsigned line_number, mpfr_bin mpfr_op) {
	mpfr_t mpfr_a, mpfr_b, mpfr_r;
	mpfr_prec_t prec = 53 + MCALIB_T;
	mpfr_rnd_t rnd = MPFR_RNDN;
	mpfr_inits2(prec, mpfr_a, mpfr_b, mpfr_r, (mpfr_ptr) 0);
	mpfr_set_d(mpfr_a, a, rnd);
	mpfr_set_d(mpfr_b, b, rnd);
	if (MCALIB_OP_TYPE != MCALIB_OP_RR) {
		mcalib_index *index_a, *index_b;
		index_a = mcalib_new_index();
		make_index(index_a, ((id << 4) + 0xa), file_name, line_number);
		index_b = mcalib_new_index();
		make_index(index_b, ((id << 4) + 0xb), file_name, line_number);
		_mca_inexact(mpfr_a, rnd, index_a);
		_mca_inexact(mpfr_b, rnd, index_b);
	}
	mpfr_op(mpfr_r, mpfr_a, mpfr_b, rnd);
	if (MCALIB_OP_TYPE != MCALIB_OP_PB) {
		mcalib_index *index_r;
		index_r = mcalib_new_index();
		make_index(index_r, ((id << 4) + 0xc), file_name, line_number);
		_mca_inexact(mpfr_r, rnd, index_r);
	}
	double ret = mpfr_get_d(mpfr_r, rnd);
	mpfr_clear(mpfr_a);
	mpfr_clear(mpfr_b);
	mpfr_clear(mpfr_r);
	return NEAREST_DOUBLE(ret);
}

double _mca_dunr(double a, mpfr_unr mpfr_op) {
	mpfr_t mpfr_a, mpfr_r;
	mpfr_prec_t prec = 53 + MCALIB_T;
	mpfr_rnd_t rnd = MPFR_RNDN;
	mpfr_inits2(prec, mpfr_a, mpfr_r, (mpfr_ptr) 0);
	mpfr_set_d(mpfr_a, a, rnd);
	if (MCALIB_OP_TYPE != MCALIB_OP_RR) {
		mcalib_index *index_a;
		index_a = mcalib_new_index();
		make_index(index_a, ((id << 4) + 0xa), file_name, line_number);
		_mca_inexact(mpfr_a, rnd, index_a);
	}
	mpfr_op(mpfr_r, mpfr_a, rnd);
	if (MCALIB_OP_TYPE != MCALIB_OP_PB) {
		mcalib_index *index_r;
		index_r = mcalib_new_index();
		make_index(index_r, ((id << 4) + 0xc), file_name, line_number);
		_mca_inexact(mpfr_r, rnd, index_r);
	}
	double ret = mpfr_get_d(mpfr_r, rnd);
	mpfr_clear(mpfr_a);
	mpfr_clear(mpfr_r);
	return NEAREST_DOUBLE(ret);
}

/******************** MCA COMPARE FUNCTIONS ********************
* Compare operations do not require MCA and as such these functions
* only convert the operands to MPFR format and execute the mpfr_cmp
* function, the integer result is then passed back to the original
* fphook function.
****************************************************************/

int _mca_scmp(float a, float b) {
	mpfr_t mpfr_a, mpfr_b;
	mpfr_prec_t prec = 24;
	mpfr_rnd_t rnd = MPFR_RNDN;
	mpfr_inits2(prec, mpfr_a, mpfr_b, (mpfr_ptr) 0);
	mpfr_set_flt(mpfr_a, a, rnd);
	mpfr_set_flt(mpfr_b, b, rnd);
	int cmp_r = mpfr_cmp(mpfr_a, mpfr_b);
	mpfr_clear(mpfr_a);
	mpfr_clear(mpfr_b);
	return cmp_r;
}

int _mca_dcmp(double a, double b) {
	mpfr_t mpfr_a, mpfr_b;
	mpfr_prec_t prec = 53;
	mpfr_rnd_t rnd = MPFR_RNDN;
	mpfr_inits2(prec, mpfr_a, mpfr_b, (mpfr_ptr) 0);
	mpfr_set_d(mpfr_a, a, rnd);
	mpfr_set_d(mpfr_b, b, rnd);
	int cmp_r = mpfr_cmp(mpfr_a, mpfr_b);
	mpfr_clear(mpfr_a);
	mpfr_clear(mpfr_b);
	return cmp_r;
}

/************************* FPHOOKS FUNCTIONS *************************
* These functions correspond to those inserted into the source code 
* during source to source compilation and are replacement to floating 
* point operators
**********************************************************************/

int _floateq(float a, float b, unsigned id, char *file_name, unsigned line_number) {
	//return a == b
	int res = _mca_scmp(a, b);
	return (res == 0 ? 1 : 0);
}

int _floatne(float a, float b, unsigned id, char *file_name, unsigned line_number) {
	//return a != b
	int res = _mca_scmp(a, b);
	return (res == 0 ? 0 : 1);
}

int _floatlt(float a, float b, unsigned id, char *file_name, unsigned line_number) {
	//return a < b
	int res = _mca_scmp(a, b);
	return (res < 0 ? 1 : 0);
}

int _floatgt(float a, float b, unsigned id, char *file_name, unsigned line_number) {
	//return a > b
	int res = _mca_scmp(a, b);
	return (res > 0 ? 1 : 0);
}

int _floatle(float a, float b, unsigned id, char *file_name, unsigned line_number) {
	//return a <= b
	int res = _mca_scmp(a, b);
	return (res <= 0 ? 1 : 0);
}

int _floatge(float a, float b, unsigned id, char *file_name, unsigned line_number) {
	//return a >= b
	int res = _mca_scmp(a, b);
	return (res >= 0 ? 1 : 0);
}

int _doubleeq(double a, double b, unsigned id, char *file_name, unsigned line_number) {
	//return a == b
	int res = _mca_dcmp(a, b);
	return (res == 0 ? 1 : 0);
}

int _doublene(double a, double b, unsigned id, char *file_name, unsigned line_number) {
	//return a != b
	int res = _mca_dcmp(a, b);
	return (res == 0 ? 0 : 1);
}

int _doublelt(double a, double b, unsigned id, char *file_name, unsigned line_number) {
	//return a < b
	int res = _mca_dcmp(a, b);
	return (res < 0 ? 1 : 0);
}

int _doublegt(double a, double b, unsigned id, char *file_name, unsigned line_number) {
	//return a > b
	int res = _mca_dcmp(a, b);
	return (res > 0 ? 1 : 0);
}

int _doublele(double a, double b, unsigned id, char *file_name, unsigned line_number) {
	//return a <= b
	int res = _mca_dcmp(a, b);
	return (res <= 0 ? 1 : 0);
}

int _doublege(double a, double b, unsigned id, char *file_name, unsigned line_number) {
	//return a >= b
	int res = _mca_dcmp(a, b);
	return (res >= 0 ? 1 : 0);
}

int _longeq(long double a, long double b, unsigned id, char *file_name, unsigned line_number) {
	return (a == b);
}

int _longne(long double a, long double b, unsigned id, char *file_name, unsigned line_number) {
	return (a != b);
}

int _longlt(long double a, long double b, unsigned id, char *file_name, unsigned line_number) {
	return (a < b);
}

int _longgt(long double a, long double b, unsigned id, char *file_name, unsigned line_number) {
	return (a > b);
}

int _longle(long double a, long double b, unsigned id, char *file_name, unsigned line_number) {
	return (a <= b);
}

int _longge(long double a, long double b, unsigned id, char *file_name, unsigned line_number) {
	return (a >= b);
}


float _floatadd(float a, float b, unsigned id, char *file_name, unsigned line_number) {
	//return a + b
	return _mca_sbin(a, b, id, file_name, line_number,(mpfr_bin)MP_ADD);
}

float _floatsub(float a, float b, unsigned id, char *file_name, unsigned line_number) {
	//return a - b
	return _mca_sbin(a, b, id, file_name, line_number, (mpfr_bin)MP_SUB);
}

float _floatmul(float a, float b, unsigned id, char *file_name, unsigned line_number) {
	//return a * b
	return _mca_sbin(a, b, id, file_name, line_number, (mpfr_bin)MP_MUL);
}

float _floatdiv(float a, float b, unsigned id, char *file_name, unsigned line_number) {
	//return a / b
	return _mca_sbin(a, b, id, file_name, line_number, (mpfr_bin)MP_DIV);
}

float _floatneg(float a, unsigned id, char *file_name, unsigned line_number) {
	//return -a
	return _mca_sunr(a, id, file_name, line_number, (mpfr_unr)MP_NEG);
}

double _doubleadd(double a, double b, unsigned id, char *file_name, unsigned line_number) {
	//return a + b
	return _mca_dbin(a, b, id, file_name, line_number, (mpfr_bin)MP_ADD);
}

double _doublesub(double a, double b, unsigned id, char *file_name, unsigned line_number) {
	//return a - b
	return _mca_dbin(a, b, id, file_name, line_number, (mpfr_bin)MP_SUB);
}

double _doublemul(double a, double b, unsigned id, char *file_name, unsigned line_number) {
	//return a * b
	return _mca_dbin(a, b, id, file_name, line_number, (mpfr_bin)MP_MUL);
}

double _doublediv(double a, double b, unsigned id, char *file_name, unsigned line_number) {
	//return a / b
	return _mca_dbin(a, b, id, file_name, line_number, (mpfr_bin)MP_DIV);
}

double _doubleneg(double a, unsigned id, char *file_name, unsigned line_number) {
	//return -a
	return _mca_dunr(a, id, file_name, line_number, (mpfr_unr)MP_NEG);
}

long double _longadd(long double a, long double b, unsigned id, char *file_name, unsigned line_number) {
	long double ret = a + b;
	return ret;
}

long double _longsub(long double a, long double b, unsigned id, char *file_name, unsigned line_number) {
	long double ret = a - b;
	return ret;
}

long double _longmul(long double a, long double b, unsigned id, char *file_name, unsigned line_number) {
	long double ret = a * b;
	return ret;
}

long double _longdiv(long double a, long double b, unsigned id, char *file_name, unsigned line_number) {
	long double ret = a / b;
	return ret;
}

long double _longneg(long double a, unsigned id, char *file_name, unsigned line_number) {
	long double ret = -a;
	return ret;
}
