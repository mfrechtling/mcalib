#include "lookup_table.h"
#define DBL_PREC	53
#define MAX_LENGTH	128

int master_index = 0;

mcalib_list* mcalib_new_list(void) {
	mcalib_list *new_list = (mcalib_list*)malloc(sizeof(mcalib_list));
	new_list->hp = NULL;
	new_list->tp = NULL;
	return new_list;
}

mcalib_list_element* mcalib_new_element(void) {
	mcalib_list_element *new_element = (mcalib_list_element*)malloc(sizeof(mcalib_list_element));
	new_element->next = NULL;
	new_element->data = NULL;
	return new_element;
}

mcalib_data* mcalib_new_data(void) {
	mcalib_data *new_data = (mcalib_data*)malloc(sizeof(mcalib_data));
	new_data->index = NULL;
	new_data->ant = mcalib_new_antithetic();
	new_data->n = (int*)malloc(sizeof(int));
	new_data->d = (int*)malloc(sizeof(int));
	return new_data;
}

mcalib_index* mcalib_new_index(void) {
	mcalib_index *new_index = (mcalib_index*)malloc(sizeof(mcalib_index));
	new_index->addr = (unsigned*)malloc(5 * sizeof(unsigned));
	new_index->id = (unsigned*)malloc(sizeof(unsigned));
	new_index->location = (char*)malloc(MAX_LENGTH*sizeof(char));
	new_index->master_index_val = (unsigned*)malloc(sizeof(unsigned));
	return new_index;
}

mcalib_antithetic* mcalib_new_antithetic(void) {
	mcalib_antithetic *new_ant = (mcalib_antithetic*)malloc(sizeof(mcalib_antithetic));
	new_ant->rand = (double*)malloc(2 * sizeof(double));
	new_ant->index = (int*)malloc(sizeof(int));
	*new_ant->index = 0;
	return new_ant;
}

void mcalib_clear_list(mcalib_list *list) {
	if (list != NULL) {
		if (list->hp != NULL) {
			mcalib_clear_element(list->hp);
		}
		free(list);
	}
}

void mcalib_clear_element(mcalib_list_element *element) {
	if (element->next != NULL) {
		mcalib_clear_element(element->next);
	}
	mcalib_clear_data(element->data);
	free(element);
}

void mcalib_clear_data(mcalib_data *data) {
	mcalib_clear_index(data->index);
	mcalib_clear_antithetic(data->ant);
	free(data->n);
	free(data->d);
	free(data);
}

void mcalib_clear_index(mcalib_index *index) {
	free(index->id);
	free(index->location);
	free(index->addr);
	free(index->master_index_val);
	free(index);
}

void mcalib_clear_antithetic(mcalib_antithetic *ant) {
	free(ant->rand);
	free(ant);
}

int mcalib_check_index(mcalib_index *index, mcalib_index *check_index) {
	int i;
	for (i = 0; i < 5; i++) {
		if (index->addr[i] != check_index->addr[i]) {
			return 0;
		}
	}
	if (*index->location != *check_index->location) {
		return 0;
	}
	if (*index->id != *check_index->id) {
		return 0;
	}
	return 1;
}

int mcalib_get_data(int rng_type, mcalib_list *list, mcalib_index *index, double *rng, mtwist *MCALIB_MTWIST, double **SOBOL) {
	mcalib_list_element *curr_element = list->hp;
	while(curr_element != NULL) {
		if (mcalib_check_index(curr_element->data->index, index) == 0) {
			curr_element = curr_element->next;
			continue;
		} else {
			if (rng_type == 1) {
				mcalib_atrand_get(curr_element->data->ant, rng, MCALIB_MTWIST);
			} else{
				//mcalib_sobol_get(curr_element->data->sobol, rng, ((*curr_element->data->index->id) & 0xf), *curr_element->data->index->master_index_val);
				//printf("N = %d\tD = %d\n", *curr_element->data->n, *curr_element->data->d);
				*rng = SOBOL[*curr_element->data->n][*curr_element->data->d];
				*curr_element->data->n = *curr_element->data->n + 1;
			}
			mcalib_clear_index(index);
			return 1;
		}
	}
	mcalib_data *new_data = mcalib_new_data();
	new_data->index = index;
	*new_data->index->master_index_val = master_index;
	*new_data->n = 0;
	*new_data->d = master_index;
	master_index++;
	//printf("MASTER_INDEX++: %d vs %d\n", master_index, *new_data->index->master_index_val);
	mcalib_add_data(list, new_data);
	if (rng_type == 1) {
		mcalib_atrand_get(new_data->ant, rng, MCALIB_MTWIST);
	} else {
		//mcalib_sobol_get(new_data->sobol, rng, ((*new_data->index->id) & 0xf), *new_data->index->master_index_val);
		//printf("N = %d\tD = %d\n", *new_data->n, *new_data->d);
		*rng = SOBOL[*new_data->n][*new_data->d];
		*new_data->n = *new_data->n + 1;
	}
	return 1;
}

/*void mcalib_sobol_get(mcalib_sobol *sobol, double *rng, unsigned check_val, int master_index_val) {
	gsl_qrng_get(sobol->qrng, sobol->rand);
	*rng = sobol->rand[master_index_val];
}*/

void mcalib_atrand_get(mcalib_antithetic *ant, double *ret, mtwist *MCALIB_MTWIST) {
	if (*ant->index == 0) {
		ant->rand[0] = mcalib_runf(MCALIB_MTWIST);
		ant->rand[1] = (-(ant->rand[0] - 0.5)) + 0.5;
		*ant->index = 1;
	} else {
		*ant->index = 0;
	}
	*ret = ant->rand[1 - *ant->index];
}

double mcalib_runf(mtwist *MCALIB_MTWIST) {
	while(1) {
		double d_rand = ((double)randomMT(MCALIB_MTWIST) / (double)(4294967295));
		if ((d_rand > 0.) & (d_rand < 1.0)) {
			//printf("RAND, %d, %x, %.16e\n", 0, (id & 0xf), d_rand - 0.5);
			return d_rand;
		}
	}
}

int mcalib_add_data(mcalib_list *list, mcalib_data *new_data) {
	mcalib_list_element *new_element = mcalib_new_element();
	new_element->data = new_data;
	if (list->hp == NULL) {
		//New list - no data
		list->hp = new_element;
		list->tp = new_element;
		return 1;
	} else {
		//Existing list, add data to tail
		list->tp->next = new_element;
		list->tp = new_element;
		return 1;
	}
	return 0;	
}

void print_list(mcalib_list *list) {
	mcalib_list_element *curr_element = list->hp;
	int counter = 0;
	while(curr_element != NULL) {
		printf("ELEMENT %d:\nLOCATION = %s\nID = %x\nMASTER INDEX = %d\nADDR_A = %u\nADDR_B = %u\nADDR_C = %u\n", counter, curr_element->data->index->location, *curr_element->data->index->id, *curr_element->data->index->master_index_val, curr_element->data->index->addr[0], curr_element->data->index->addr[1], curr_element->data->index->addr[2]);
		curr_element = curr_element->next;
		counter++;
	}
}
