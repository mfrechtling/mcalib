#ifndef MCALIB_RNG_LIST
#define MCALIB_RNG_LIST
#include "stdlib.h"
#include "stdio.h"
#include "gsl/gsl_qrng.h"
#include "mtwist.h"

typedef struct mcalib_list 		mcalib_list;
typedef struct mcalib_list_element	mcalib_list_element;
typedef struct mcalib_data		mcalib_data;
typedef struct mcalib_index		mcalib_index;
typedef struct mcalib_antithetic	mcalib_antithetic;
typedef struct mcalib_sobol		mcalib_sobol;
extern int master_index;

struct mcalib_list {
	mcalib_list_element	*hp;
	mcalib_list_element	*tp;
};

struct mcalib_list_element {
	mcalib_list_element	*next;
	mcalib_list_element	*prev;
	mcalib_data		*data;
};

struct mcalib_data {
	mcalib_index		*index;
	mcalib_antithetic	*ant;
	int			*n;
	int			*d;
};

struct mcalib_index {
	unsigned		*id;
	char			*location;
	unsigned		*addr;
	unsigned		*master_index_val;
};

struct mcalib_antithetic {
	double 			*rand;
	int 			*index;
};

mcalib_list* mcalib_new_list(void);
mcalib_list_element* mcalib_new_element(void);
mcalib_data* mcalib_new_data(void);
mcalib_index* mcalib_new_index(void);
mcalib_antithetic* mcalib_new_antithetic(void);
void mcalib_clear_list(mcalib_list *list);
void mcalib_clear_element(mcalib_list_element *element);
void mcalib_clear_data(mcalib_data *data);
void mcalib_clear_index(mcalib_index *index);
void mcalib_clear_antithetic(mcalib_antithetic *ant);
int mcalib_get_data(int rng_type, mcalib_list *list, mcalib_index *index, double *rng, mtwist *MCALIB_MTWIST, double **SOBOL);
void mcalib_atrand_get(mcalib_antithetic *ant, double *ret, mtwist *MCALIB_MTWIST);
int mcalib_add_data(mcalib_list *list, mcalib_data *new_data);
double mcalib_runf(mtwist *MCALIB_MTWIST);
void print_list(mcalib_list *list);
void mcalib_sobol_get(mcalib_sobol *sobol, double *rng, unsigned check_val, int master_index_val);
#endif