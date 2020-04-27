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
	int nBits[] = {8, 12, 16, 20, 24, 28, 31, 64};
	list<int> NBits (nBits, nBits+sizeof(nBits)/sizeof(int));

	int es[] = {0, 2, 4, 6, 8, 10, 11};
	list<int> ES (es, es+sizeof(es)/sizeof(int));

	Posit_err Posit_repr;
	list<err_t> posit_error;
	Float_err Float_repr;
	list<err_t> float_error;

	num_size_t size;
	for(list<int>::iterator nb=NBits.begin(); nb!=NBits.end(); nb++){
		err_t P_err = err_init(size);
		P_err.max_rel_error = INFINITY; P_err.min_rel_error = INFINITY;
		err_t F_err = err_init(size);
		F_err.max_rel_error = INFINITY; F_err.min_rel_error = INFINITY;

		for(list<int>::iterator es=ES.begin(); es!=ES.end() && *es<*nb; es++){
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
			if(err.max_rel_error < F_err.max_rel_error && err.out_of_range==0){
				F_err = err;
			}
		}
		if(size.nBits <= 31){
			posit_error.push_back(P_err);
		}
		float_error.push_back(F_err);
	}

	// cout << "NBits : ";
	// for (list<int>::iterator nb=NBits.begin(); nb!=NBits.end(); nb++){
	// 	cout << *nb << " ";
	// }
	// cout << endl;

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



	// // for(int nBits = 8; nBits<=16; nBits+=2){
	// int Nbits = 8;
	// double rel_error_posit[] = {INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY};
	// double rel_error_float[] = {INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY};

	// int Nexp = 6;


	// for(int i=0;i<Nbits;i++){
	// 	size.nBits = nBits[i];
	// 	for(int j=0;j<Nexp;j++){
	// 		size.es = es[j];
	// 		if(size.nBits <= 31){
	// 			err_t pos_err = spn->compute_err(&Posit_repr, size);
	// 			rel_error_posit[i] = fmin(rel_error_posit[i], pos_err.max_rel_error);
	// 		}
	// 		err_t float_err = spn->compute_err(&Float_repr, size);
	// 		rel_error_float[i] = fmin(rel_error_float[i], float_err.max_rel_error);
	// 	}
	// }

	// printf("NBITS: ");
	// for(int i=0;i<Nbits;i++){
	// 	printf("%d\t", nBits[i]);
	// }
	// printf("\nPOSIT: ");
	// for(int i=0;i<Nbits;i++){
	// 	if(rel_error_posit[i] < 10){
	// 		printf("%lf\t", rel_error_posit[i]);
	// 	}else{
	// 		printf("INF\t\t");
	// 	}
	// }
	// printf("\nFLOAT: ");

	// for(int i=0;i<Nbits;i++){
	// 	if(rel_error_float[i] < 10 || nBits[i]==64){
	// 		printf("%lf\t", rel_error_float[i]);
	// 	}else{
	// 		printf("INF(%d)\t\t", nBits[i]);
	// 	}
	// }
	// printf("\n");


	return 0;
}

#endif