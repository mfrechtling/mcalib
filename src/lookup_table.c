#include "lookup_table.h"
#define DBL_PREC	53
#define MAX_LENGTH	128

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
	new_data->qrng = gsl_qrng_alloc(gsl_qrng_sobol, 3);
	return new_data;
}

mcalib_index* mcalib_new_index(void) {
	mcalib_index *new_index = (mcalib_index*)malloc(sizeof(mcalib_index));
	new_index->addr_lvl_zero = (unsigned long int*)malloc(sizeof(unsigned long int));
	new_index->addr_lvl_one = (unsigned long int*)malloc(sizeof(unsigned long int));
	new_index->id = (unsigned*)malloc(sizeof(unsigned));
	new_index->location = (char*)malloc(MAX_LENGTH*sizeof(char));
	return new_index;
}

mcalib_antithetic* mcalib_new_antithetic(void) {
	mcalib_antithetic *new_ant = (mcalib_antithetic*)malloc(sizeof(mcalib_antithetic));
	new_ant->rand = (double*)malloc(2 * sizeof(double));
	new_ant->index = 0;
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
	gsl_qrng_free(data->qrng);
	mcalib_clear_antithetic(data->ant);
	free(data);
}

void mcalib_clear_index(mcalib_index *index) {
	free(index->id);
	free(index->location);
	free(index->addr_lvl_zero);
	free(index->addr_lvl_one);
	free(index);
}

void mcalib_clear_antithetic(mcalib_antithetic *ant) {
	free(ant->rand);
	free(ant);
}

int mcalib_get_data(int rng_type, mcalib_list *list, mcalib_index *index, double *rng) {
	mcalib_list_element *curr_element = list->hp;
	while(curr_element != NULL) {
		if (*curr_element->data->index->addr_lvl_zero != *index->addr_lvl_zero) {
			curr_element = curr_element->next;
			continue;
		}
		if (*curr_element->data->index->addr_lvl_one != *index->addr_lvl_one) {
			curr_element = curr_element->next;
			continue;
		}
		if (*curr_element->data->index->location != *index->location) {
			curr_element = curr_element->next;
			continue;
		}
		if (*curr_element->data->index->id != *index->id) {
			curr_element = curr_element->next;
			continue;
		}
		if (rng_type == 1) {
			mcalib_atrand_get(curr_element->data->ant, rng);		
		} else {
			double temp_res[3];
			gsl_qrng_get(curr_element->data->qrng, temp_res);
			int check_val = (*curr_element->data->index->id & 0xf);
			switch (check_val) {
				case 0xa:
					*rng = temp_res[0];
					break;
				case 0xb:
					*rng = temp_res[1];
					break;
				case 0xc:
					*rng = temp_res[2];
					break;
			}
		}
		mcalib_clear_index(index);
		return 1;
	}
	mcalib_data *new_data = mcalib_new_data();
	new_data->index = index;
	mcalib_add_data(list, new_data);
	if (rng_type == 1) {
		mcalib_atrand_get(&new_data->ant, rng);
	} else {
		double temp_res[3];
		gsl_qrng_get(new_data->qrng, temp_res);
		int check_val = (*new_data->index->id & 0xf);
		switch (check_val) {
			case 0xa:
				*rng = temp_res[0];
				break;
			case 0xb:
				*rng = temp_res[1];
				break;
			case 0xc:
				*rng = temp_res[2];
				break;
		}
	}
	return 1;
}

void mcalib_atrand_get(mcalib_antithetic *ant, double *ret) {
	if (ant->index == 0) {
		ant->rand[0] = mcalib_runf();
		ant->rand[1] = (0. - (ant->rand[0] - 0.5)) + 0.5;
		ant->index = 1;
	} else {
		ant->index = 0;
	}
	*ret = ant->rand[1 - ant->index];
}

double mcalib_runf(void) {
	while(1) {
		double d_rand = ((double)rand() / (double)RAND_MAX);
		if ((d_rand > 0.) & (d_rand < 1.0)) {
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
		printf("ELEMENT %d:\nLOCATION = %s\nID = %d\nADDR0 = %lu\nADDR1 = %lu\n", counter, curr_element->data->index->location, *curr_element->data->index->id, *curr_element->data->index->addr_lvl_zero, *curr_element->data->index->addr_lvl_one);
		curr_element = curr_element->next;
		counter++;
	}
}
