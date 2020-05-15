#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "posit.h"

#include "utils.h"

void print_err(err_t err){
	if(err.out_of_range){
		printf("Results are out of range for size (%d, %d)\n", err.nBits, err.es);
	}
	printf("(%d, %d): Min-Max values are (%lf - %lf) and errors are (2^-%d - 2^-%d)\n", err.nBits, err.es, err.min_value, err.max_value, err.err_exp_min, err.err_exp_max);
}

void print_err(FILE* f, err_t err){
	fprintf(f, "(%d, %d): Min-Max values are (%lf - %lf) and errors are (2^-%d - 2^-%d)", err.nBits, err.es, err.min_value, err.max_value, err.err_exp_min, err.err_exp_max);
	if(err.out_of_range){
		fprintf(f, "  Out of range\n");
	}else{
		fprintf(f, "\n");
	}
}

void print_err(FILE*f, err_t err, fileinfo_t fileinfo, int is_posit){
	fprintf(f, "%s %s %d %d %d ",
				fileinfo.dir.c_str(),
				fileinfo.filename.c_str(),
				fileinfo.nNodes,
				fileinfo.nMult,
				fileinfo.nAdd);

	fprintf(f, "%d %d %d 2^-%d 2^-%d\n",
				is_posit,
				err.nBits,
				err.es,
				err.err_exp_min,
				err.err_exp_max);
}

err_t err_init(num_size_t size){
	err_t res;
	res.nBits = size.nBits;
	res.es = size.es;
	return res;
}

void err_set(err_t* error, double min_val, double max_val, int min_err_exp, int max_err_exp){
	error->max_value = max_val;
	error->min_value = min_val;
	error->err_exp_max = max_err_exp;
	error->err_exp_min = min_err_exp;
}

// =============================================================================
// POSIT =======================================================================
// =============================================================================


err_t Posit_err::encoding_error(num_size_t size, double val){
	err_t res = err_init(size);
	Posit pos = Posit(res.nBits+1, res.es);
	pos.set(val);
	int err_exp;
	if(val == 0){
		err_exp = 0;
	}else{
		err_exp = pos.fs() + 1;
	}

	double useed = pow(2, pow(2, size.es));
	double max_posit = pow(useed, size.nBits-1);
	double min_posit = 1.0 / max_posit;

	if(val != 0){
		if(val > max_posit || val < min_posit){
			err_exp = 0;
			res.out_of_range = 1;
		}
	}

	// printf("New encoding error : %d\n", err_exp);
	err_set(&res, val, val, err_exp, err_exp);
	return res;
}

err_t Posit_err::multiplication_error(num_size_t size, err_t pos1, err_t pos2){
	err_t res = err_init(size);

	double max_val = pos1.max_value * pos2.max_value;
	err_t new_error_p = this->encoding_error(size, max_val);
	int max_new_error = pos1.err_exp_max+pos2.err_exp_max+new_error_p.err_exp_max;

	double min_val = pos1.min_value * pos2.min_value;
	new_error_p = this->encoding_error(size, min_val);
	int min_new_error = pos1.err_exp_min+pos2.err_exp_min+new_error_p.err_exp_min;

	res.out_of_range = pos1.out_of_range||pos2.out_of_range||new_error_p.out_of_range;

	// printf("New multiplication error : %d\n", max_new_error);
	err_set(&res, min_val, max_val, min_new_error, max_new_error);
	return res;
}

err_t Posit_err::addition_error(num_size_t size, err_t pos1, err_t pos2){
	err_t res = err_init(size);

	double max_val = pos1.max_value + pos2.max_value;
	err_t new_error_p = this->encoding_error(size, max_val);
	int max_new_error = fmax(pos1.err_exp_max, pos2.err_exp_max) + new_error_p.err_exp_max;

	double min_val;
	int min_new_error = 0;
	if(pos1.min_value < pos2.min_value && pos1.min_value != 0){
		min_val = pos1.min_value;
		min_new_error = pos1.err_exp_min;
		res.out_of_range = pos1.out_of_range;
	}else{
		min_val = pos2.min_value;
		min_new_error = pos2.err_exp_min;
		res.out_of_range = pos2.out_of_range;
	}
	new_error_p = this->encoding_error(size, min_val);
	min_new_error += new_error_p.err_exp_min;

	res.out_of_range = res.out_of_range || new_error_p.out_of_range;
	// printf("New addition error : %d\n", max_new_error);
	err_set(&res, min_val, max_val, min_new_error, max_new_error);
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


utilization_t Posit_err::adder_utilization(num_size_t size){
	utilization_t ut;

	if(size.nBits==8){
		ut.nLUT = 162;
	}else if(size.nBits==16){
		ut.nLUT = 466;
	}else if(size.nBits==32){
		ut.nLUT = 1160;
		ut.nMUXFX = 4;
	}else{
		printf("Unknown number of bits");
		exit(-1);
	}

	return ut;
}

utilization_t Posit_err::multiplier_utilization(num_size_t size){
	utilization_t ut;

	if(size.nBits==8){
		ut.nLUT = 187;
	}else if(size.nBits==16){
		ut.nLUT = 407;
		ut.nMULT = 1;
	}else if(size.nBits==32){
		ut.nLUT = 1027;
		ut.nMULT = 4;
	}else{
		printf("Unknown number of bits");
		exit(-1);
	}

	return ut;
}



// =============================================================================
// Floating point ==============================================================
// =============================================================================



err_t Float_err::encoding_error(num_size_t size, double val){
	err_t res = err_init(size);
	int err_exp;
	if(val==0){
		err_exp = 0;
	}else{
		err_exp = res.nBits-res.es+1;
	}

	double max_exp = pow(2, pow(2, size.es-1)-1);
	double max_mant = 2 - pow(2, size.es - size.nBits);
	double max_float = max_mant*max_exp;
	double min_float = 1.0 / max_exp;

	if(val != 0){
		if(val > max_float || val < min_float){
			res.out_of_range = 1;
			err_exp = 0;
		}
	}

	err_set(&res, val, val, err_exp, err_exp);
	return res;
}

err_t Float_err::multiplication_error(num_size_t size, err_t f1, err_t f2){
	err_t res = err_init(size);
	double max_val = f1.max_value * f2.max_value;
	err_t new_error_f = this->encoding_error(size, max_val);
	int max_new_error = f1.err_exp_max+f2.err_exp_max+new_error_f.err_exp_max;

	double min_val = f1.min_value * f2.min_value;
	new_error_f = this->encoding_error(size, min_val);
	int min_new_error = f1.err_exp_min+f2.err_exp_min+new_error_f.err_exp_min;
	// if(min_val==0){
	// 	rel_error_bound_min = 0;
	// }else{
	// 	rel_error_bound_min = (accumulated_error)*(1+new_error) - 1;
	// }

	res.out_of_range =f1.out_of_range||f2.out_of_range||new_error_f.out_of_range;

	err_set(&res, min_val, max_val, min_new_error, max_new_error);
	return res;
}
err_t Float_err::addition_error(num_size_t size, err_t f1, err_t f2){
	err_t res = err_init(size);
	double max_val = f1.max_value + f2.max_value;
	err_t new_error_f = this->encoding_error(size, max_val);
	int max_new_error = fmax(f1.err_exp_max, f2.err_exp_max) + new_error_f.err_exp_max;

	double min_val;
	int min_new_error = 0;
	if(f1.min_value < f2.min_value && f1.min_value != 0){
		min_val = f1.min_value;
		min_new_error = f1.err_exp_min;
		res.out_of_range = f1.out_of_range;
	}else{
		min_val = f2.min_value;
		min_new_error = f2.err_exp_min;
		// out of range only for minval (worst case)
		res.out_of_range = f2.out_of_range;
	}
	new_error_f = this->encoding_error(size, min_val);
	min_new_error += new_error_f.err_exp_min;

	err_set(&res, min_val, max_val, min_new_error, max_new_error);
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

utilization_t Float_err::adder_utilization(num_size_t size){
	utilization_t ut;
	printf("adder_utilization not implemented yet\n");
	return ut;
}

utilization_t Float_err::multiplier_utilization(num_size_t size){
	utilization_t ut;
	printf("multiplier_utilization not implemented yet\n");
	return ut;
}
