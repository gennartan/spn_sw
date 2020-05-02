#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include "spn.h"

// #define SIMPLE_TEST

using namespace std;
int simple_test(SPN *spn, FILE* f);
int complete_test(SPN *spn, FILE* f);

int main(int argc, char* argv[]){

	char *in_filename, *out_filename;
	if(argc == 3){
		in_filename = argv[1];
		out_filename = argv[2];
	}else if(argc == 2){
		in_filename = argv[1];
		out_filename = NULL;
	}else{
		cout << "Usage : ./exe filename.spn outfile.txt" << endl;
		const char *default_name = "example/example_03.spn";
		in_filename = (char*) malloc(10*sizeof(char));
		in_filename = (char*) default_name;
		out_filename = NULL;
	}

	FILE *f;
	if(out_filename == NULL){
		f = stdout;
	}else{
		f = fopen((const char*)out_filename, "w");
	}
	if(f==NULL){
		cerr << "Cannot open file! : " << out_filename <<endl;
		exit(0);
	}


	SPN spn = SPN(in_filename);
	printf("%s\n", in_filename);

#ifdef SIMPLE_TEST
	simple_test(&spn, f);
#else
	complete_test(&spn, f);
#endif


	if(f!=stdout){
		fclose(f);
	}

	if(argc >= 3){
		free(out_filename);
	}
}

#ifdef SIMPLE_TEST
int simple_test(SPN *spn, FILE* f){

	// printf("Network is decomposable : %d\n", spn.isDecomposable());
	// printf("Network is deterministic : %d\n", spn.isDeterministic());
	// printf("Network is smooth : %d\n", spn.isSmooth());

	printf("Compute output of SPN\n");
	int **literals = spn->create_literals();


	while(spn->next_literals(literals) == 0){
		spn->print_literals(f, literals);
		fprintf(f, "%lf\n", spn->compute(literals));
	}

	spn->delete_literals(literals);

	printf("Compute maximum error of SPN\n");
	Posit_err Posit_repr;
	num_size_t posit_size;
	posit_size.nBits = 18;
	posit_size.es = 3;

	Float_err Float_repr;
	num_size_t float_size;
	float_size.nBits = 16;
	float_size.es = 3;

	err_t posit_err = spn->compute_err(&Posit_repr, posit_size);
	err_t float_err = spn->compute_err(&Float_repr, float_size);

	printf("For posit\n");
	print_err(posit_err);
	printf("For float\n");
	print_err(float_err);

	return 0;
}

#else

int complete_test(SPN *spn, FILE* f){
	int nBits[] = {8, 12, 16, 20, 24, 28, 31};
	list<int> NBits (nBits, nBits+sizeof(nBits)/sizeof(int));

	int es[] = {0, 2, 4, 6, 8, 10, 11};
	list<int> ES (es, es+sizeof(es)/sizeof(int));

	Posit_err Posit_repr;
	list<err_t> posit_error;
	Float_err Float_repr;
	list<err_t> float_error;

	// Compute error bound
	num_size_t size;
	for(list<int>::iterator nb=NBits.begin(); nb!=NBits.end(); nb++){
		err_t P_err = err_init(size);
		P_err.max_rel_error = INFINITY; P_err.min_rel_error = INFINITY;
		err_t F_err = err_init(size);
		F_err.max_rel_error = INFINITY; F_err.min_rel_error = INFINITY;

		for(list<int>::iterator es=ES.begin(); es!=ES.end() && *es<*nb;es++){
			size.nBits = *nb;
			size.es = *es;

			err_t err;
			if(size.nBits <= 31){
				err = spn->compute_err(&Posit_repr, size);
				if(err.max_rel_error < P_err.max_rel_error && err.out_of_range==0){
					P_err = err;
				}
			}

			err = spn->compute_err(&Float_repr, size);
			if((err.max_rel_error < F_err.max_rel_error && err.out_of_range==0)  || (F_err.max_rel_error==INFINITY && *es==11)){
				F_err = err;
			}
		}
		int** literals = spn->create_literals();
		for(int i=0;i<spn->n_lit;i++){
			literals[i][0] = 1;
			literals[i][1] = 1;
		}

		spn->random_literals(literals);

		double exact_val = spn->compute_exact(literals);
		// Posit
		size.nBits = P_err.nBits;
		size.es = P_err.es;
		Posit result_posit = spn->compute_real_posit(literals, size);
		double res_pos = result_posit.getDouble();
		double exact_pos = exact_val;
		double error_bound_pos = P_err.max_rel_error;
		printf("(%d, %d) Posit %lf ~==~ %lf\t diff ==%lf < %lf\n", size.nBits, size.es, exact_pos, res_pos, exact_pos-res_pos, error_bound_pos);

		// Float
		size.nBits = F_err.nBits;
		size.es = F_err.es;
		myFloat result_float = spn->compute_real_float(literals, size);
		double res_flt = result_float.getDouble();
		double exact_flt = exact_val;
		double error_bound_flt = F_err.max_rel_error;
		printf("(%d, %d) Float %lf ~==~ %lf\t diff ==%lf < %lf\n", size.nBits, size.es, exact_flt, res_flt, exact_flt-res_flt, error_bound_flt);

		posit_error.push_back(P_err);
		float_error.push_back(F_err);
	}

	// Compute Real value with best nBits and best es
	for(list<err_t>::iterator e=posit_error.begin(); e!=posit_error.end(); e++){
		size.nBits = e->nBits;
		size.es = e->es;

		int** literals = spn->create_literals();
		for(int i=0;i<spn->n_lit;i++){
			literals[i][0] = 1;
			literals[i][1] = 1;
		}
		Posit result_posit = spn->compute_real_posit(literals, size);
		printf("(%d, %d) Posit value : %f\n", size.nBits, size.es, result_posit.getDouble());


	}

	for(list<err_t>::iterator e=float_error.begin(); e!=float_error.end(); e++){
		size.nBits = e->nBits;
		size.es = e->es;

		int **literals = spn->create_literals();
		for(int i=0;i<spn->n_lit;i++){
			literals[i][0] = 1;
			literals[i][1] = 1;
		}
		myFloat result_float = spn->compute_real_float(literals, size);
		printf("(%d, %d) Float value : %f\n", size.nBits, size.es, result_float.getDouble());
	}

	cout << "Posit " << endl;
	for(list<err_t>::iterator e=posit_error.begin(); e!=posit_error.end(); e++){
		print_err(*e);
	}
	cout << endl;

	cout << "FLoat " << endl;
	for(list<err_t>::iterator e=float_error.begin(); e!=float_error.end(); e++){
		print_err(*e);
	}
	cout << endl;

	return 0;
}

#endif