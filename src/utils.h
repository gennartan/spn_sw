#ifndef __UTILS_H__
#define __UTILS_H__

enum TYPE {None, L, S, P, W};

typedef struct num_size_t_struct{
	int nBits = 0;
	int es = 0;
} num_size_t;

typedef struct err_t_struct{
	int nBits = 0;
	int es = 0;
	double max_value=0, min_value=0;
	double max_rel_error=0, min_rel_error=0;
	int out_of_range = 0;
} err_t;

typedef struct cost_t_struct{
	num_size_t size;
	double max_node_numbers; // A node is a sum or a product
	double multiplier_speed; // min period to run on FPGA
	double min_rel_error, max_rel_error; // relative error
} cost_t;


void print_err(err_t err);
err_t err_init(num_size_t size);
void err_set(err_t* error, double max_val, double min_val, double max_err, double min_err);

// =============================================================================
// General interface for number representations ================================
// =============================================================================

class Number_representation{
public:
	// Precision - Error model
	virtual err_t encoding_error(num_size_t size, double val) = 0;
	virtual err_t multiplication_error(num_size_t size, err_t pos1, err_t pos2) = 0;
	virtual err_t addition_error(num_size_t size, err_t pos1, err_t pos2) = 0;

	// Area - Hardware size
	virtual double adder_area(num_size_t size) = 0;
	virtual double multiplier_area(num_size_t size) = 0;

	// Speed - Hardware/software timing
	virtual double compute_period(num_size_t size) = 0;
};

// =============================================================================
// Posit  ======================================================================
// =============================================================================

class Posit_err : public Number_representation{
	public:
	// Precision - Error model
	virtual err_t encoding_error(num_size_t size, double val);
	virtual err_t multiplication_error(num_size_t size, err_t pos1, err_t pos2);
	virtual err_t addition_error(num_size_t size, err_t pos1, err_t pos2);

	// Area - Hardware size
	virtual double adder_area(num_size_t size);
	virtual double multiplier_area(num_size_t size);

	// Speed - Hardware/software timing
	virtual double compute_period(num_size_t size);
};

// =============================================================================
// Floating point ==============================================================
// =============================================================================

class Float_err : public Number_representation{
	public:
	// Precision - Error model
	virtual err_t encoding_error(num_size_t size, double val);
	virtual err_t multiplication_error(num_size_t size, err_t pos1, err_t pos2);
	virtual err_t addition_error(num_size_t size, err_t pos1, err_t pos2);

	// Area - Hardware size
	virtual double adder_area(num_size_t size);
	virtual double multiplier_area(num_size_t size);

	// Speed - Hardware/software timing
	virtual double compute_period(num_size_t size);
};


#endif