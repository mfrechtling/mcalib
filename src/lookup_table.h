#ifndef MCALIB_RNG_LIST
#define MCALIB_RNG_LIST
#include "stdlib.h"
#include "stdio.h"
#include "gsl/gsl_qrng.h"

typedef struct mcalib_list 		mcalib_list;
typedef struct mcalib_list_element	mcalib_list_element;
typedef struct mcalib_data		mcalib_data;
typedef struct mcalib_index		mcalib_index;

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
	gsl_qrng		*qrng;
};

struct mcalib_index {
	unsigned		*id;
	char			*location;
	unsigned long int	*addr_lvl_zero;
	unsigned long int	*addr_lvl_one;
};

mcalib_list* mcalib_new_list(void);
mcalib_list_element* mcalib_new_element(void);
mcalib_data* mcalib_new_data(void);
mcalib_index* mcalib_new_index(void);
void mcalib_clear_list(mcalib_list *list);
void mcalib_clear_element(mcalib_list_element *element);
void mcalib_clear_data(mcalib_data *data);
void mcalib_clear_index(mcalib_addr *index);
int mcalib_get_data(mcalib_list *list, mcalib_index *index, double *rng);
int mcalib_add_data(mcalib_list *list, mcalib_data *new_data);
#endif
