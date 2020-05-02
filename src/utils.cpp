#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "posit.h"

#include "utils.h"

void print_err(err_t err){
	if(err.out_of_range){
		printf("Results are out of range for size (%d, %d)\n", err.nBits, err.es);
	}
	printf("(%d, %d): Min-Max values are (%lf - %lf) and errors are (%lf - %lf)\n", err.nBits, err.es, err.min_value, err.max_value, err.min_rel_error, err.max_rel_error);
	// printf("Was out of range : %d\n", err.out_of_range);
	// printf("Max value : %lf \t Relative Error : %lf\n", err.max_value, err.max_rel_error);
	// printf("Min value : %lf \t Relative Error : %lf\n", err.min_value, err.min_rel_error);
}

err_t err_init(num_size_t size){
	err_t res;
	res.nBits = size.nBits;
	res.es = size.es;
	return res;
}

void err_set(err_t* error, double max_val, double min_val, double max_err, double min_err){
	error->max_value = max_val;
	error->min_value = min_val;
	error->max_rel_error = max_err;
	error->min_rel_error = min_err;
}

// =============================================================================
// POSIT =======================================================================
// =============================================================================


err_t Posit_err::encoding_error(num_size_t size, double val){
	err_t res = err_init(size);
	Posit pos = Posit(res.nBits+1, res.es);
	pos.set(val);
	double rel_error;
	if(val == 0){
		rel_error = 0;
	}else{
		rel_error= pow(2, -(pos.fs()+1));
	}


	double useed = pow(2, pow(2, size.es));
	double max_posit = pow(useed, size.nBits-1);
	double min_posit = 1.0 / max_posit;

	if(val != 0){
		if(val > max_posit || val < min_posit){
			rel_error = 1;
			res.out_of_range = 1;
		}
	}

	err_set(&res, val, val, rel_error, rel_error);
	return res;
}

err_t Posit_err::multiplication_error(num_size_t size, err_t pos1, err_t pos2){
	err_t res = err_init(size);

	double max_val = pos1.max_value * pos2.max_value;
	double accumulated_error = (1+pos1.max_rel_error)*(1+pos2.max_rel_error);
	err_t new_error_p = this->encoding_error(size, max_val);
	double new_error = new_error_p.max_rel_error;
	double rel_error_bound = accumulated_error*(1+new_error) - 1;

	double min_val = pos1.min_value * pos2.min_value;
	new_error_p = this->encoding_error(size, min_val);
	new_error = new_error_p.min_rel_error;
	accumulated_error = (1+pos1.min_rel_error)*(1+pos2.min_rel_error);
	double rel_error_bound_min;
	if(min_val==0){
		rel_error_bound_min = 0;
	}else{
		rel_error_bound_min = accumulated_error*(1+new_error) - 1;
	}

	res.out_of_range = pos1.out_of_range||pos2.out_of_range||new_error_p.out_of_range;

	err_set(&res, max_val, min_val, rel_error_bound, rel_error_bound_min);
	return res;
}

err_t Posit_err::addition_error(num_size_t size, err_t pos1, err_t pos2){
	err_t res = err_init(size);

	double max_val = pos1.max_value + pos2.max_value;
	double accumulated_error = fmax(pos1.max_rel_error, pos2.max_rel_error);
	err_t new_error_p = this->encoding_error(size, max_val);
	double new_error = new_error_p.max_rel_error;
	double rel_error_bound = (1+accumulated_error)*(1+new_error) - 1;

	double min_val;
	if(pos1.min_value < pos2.min_value && pos1.min_value != 0){
		min_val = pos1.min_value;
		accumulated_error = pos1.min_rel_error;
		res.out_of_range = pos1.out_of_range;
	}else{
		min_val = pos2.min_value;
		accumulated_error = pos2.min_rel_error;
		res.out_of_range = pos2.out_of_range;
	}

	new_error_p = this->encoding_error(size, min_val);
	res.out_of_range = res.out_of_range || new_error_p.out_of_range;
	new_error = new_error_p.min_rel_error;
	double rel_error_bound_min = (1+accumulated_error)*(1+new_error) - 1;
	err_set(&res, max_val, min_val, rel_error_bound, rel_error_bound_min);
	return res;
}


double Posit_err::adder_area(num_size_t size){
	printf("adder_area not implemented yet\n");
	return 0.0;
}

double Posit_err::multiplier_area(num_size_t size){
	printf("multiplier_area not implemented yet\n");
	return 0.0;
}

double Posit_err::compute_period(num_size_t size){
	printf("computer_period not implemented yet\n");
	return 0;
}


// =============================================================================
// Floating point ==============================================================
// =============================================================================



err_t Float_err::encoding_error(num_size_t size, double val){
	err_t res = err_init(size);
	double rel_error;
	if(val==0){
		rel_error = 0;
	}else{
		rel_error = pow(2, -(res.nBits-res.es+1));
	}

	double max_exp = pow(2, pow(2, size.es-1)-1);
	double max_mant = 2 - pow(2, size.es - size.nBits);
	double max_float = max_mant*max_exp;
	double min_float = 1.0 / max_exp;

	// printf("(2 - 2^(%d))* 2^%d\n", size.es-size.nBits, pow(2, size.es)-1);
	// exit(0);
	if(val != 0){
		if(val > max_float || val < min_float){
			res.out_of_range = 1;
			rel_error = 1;
		}
	}

	err_set(&res, val, val, rel_error, rel_error);
	return res;
}

err_t Float_err::multiplication_error(num_size_t size, err_t f1, err_t f2){
	err_t res = err_init(size);
	double max_val = f1.max_value * f2.max_value;
	double accumulated_error = (1+f1.max_rel_error)*(1+f2.max_rel_error);
	err_t new_error_f = this->encoding_error(size, max_val);
	double new_error = new_error_f.max_rel_error;
	double rel_error_bound;
	if(max_val==0){
		rel_error_bound = 0;
	}else{
		rel_error_bound = (accumulated_error)*(1+new_error) - 1;
	}

	double min_val = f1.min_value * f2.min_value;
	accumulated_error = (1+f1.min_rel_error)*(1+f2.min_rel_error);
	new_error_f = this->encoding_error(size, min_val);
	new_error = new_error_f.min_rel_error;
	double rel_error_bound_min;
	if(min_val==0){
		rel_error_bound_min = 0;
	}else{
		rel_error_bound_min = (accumulated_error)*(1+new_error) - 1;
	}

	//if(min_val == 0 && max_val != 0){
	//	printf("EROROROROROROR\n");
	//}
	res.out_of_range =f1.out_of_range||f2.out_of_range||new_error_f.out_of_range;

	// printf("Multiplication val / error == %lf / %lf\n", max_val, rel_error_bound);
	err_set(&res, max_val, min_val, rel_error_bound, rel_error_bound_min);
	return res;
}
err_t Float_err::addition_error(num_size_t size, err_t f1, err_t f2){
	err_t res = err_init(size);
	double max_val = f1.max_value + f2.max_value;
	double accumulated_error = fmax(f1.max_rel_error, f2.max_rel_error);
	double new_error = this->encoding_error(size, max_val).max_rel_error;
	double rel_error_bound = (1+accumulated_error)*(1+new_error) - 1;

	double min_val;
	if(f1.min_value < f2.min_value && f1.min_value != 0){
		min_val = f1.min_value;
		accumulated_error = f1.min_rel_error;
		// out of range only for minval (worst case)
		res.out_of_range = f1.out_of_range;
	}else{
		min_val = f2.min_value;
		accumulated_error = f2.min_rel_error;
		// out of range only for minval (worst case)
		res.out_of_range = f2.out_of_range;
	}
	new_error = this->encoding_error(size, min_val).min_rel_error;
	double rel_error_bound_min = (1+accumulated_error)*(1+new_error) - 1;

	// printf("Addition val / error == %lf / %lf\n", max_val, rel_error_bound);
	err_set(&res, max_val, min_val, rel_error_bound, rel_error_bound_min);
	return res;
}

double Float_err::adder_area(num_size_t size){
	printf("adder_area not implemented yet\n");
	return 0.0;
}

double Float_err::multiplier_area(num_size_t size){
	printf("multiplier_area not implemented yet\n");
	return 0.0;
}

double Float_err::compute_period(num_size_t size){
	printf("computer_period not implemented yet\n");
	return 0;
}
