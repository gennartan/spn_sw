#include <stdio.h>
#include <dirent.h>
#include <iostream>
#include <string>
#include <list>
#include <math.h>
#include "spn.h"

using namespace std;

int complete_analysis(int argc, char* argv[]);
int simple_analysis(int argc, char* argv[]);
std::string pathAppend(const std::string& p1, const std::string& p2);
int analyse_spn(SPN* spn, FILE *f, fileinfo_t fileinfo);
int get_results_from_file(FILE *outfile, SPN* spn, FILE *in_file);
int get_diffs(FILE* outfile, SPN* spn, FILE* double_float, FILE* posit);

int main(int argc, char*argv[]){


	// FILE *double_float, *posit, *outfile;
	// posit = fopen("../result/result_msnbc_posit_0-0.txt", "r");
	// if(posit==NULL){
	// 	fprintf(stderr, "Posit file does not exist\n");
	// 	exit(-1);
	// }
	// double_float = fopen("../result/result_msnbc_exact_0-0.txt", "r");
	// if(double_float==NULL){
	// 	fprintf(stderr, "Double float file does not exist\n");
	// 	exit(-1);
	// }

	// SPN spn = SPN("example/msnbc_0-0.spn");
	// get_diffs(stdout, &spn, double_float, posit);

	// return 0;


	// FILE *infile, *outfile;
	// infile = fopen("../dataset/dataset_msnbc_0-0.txt", "r");
	// if(infile==NULL){
	// 	fprintf(stderr, "Cannot open input file \n");
	// 	exit(-1);
	// }
	// outfile = fopen("../result/result_msnbc_exact_0-0.txt", "w");
	// if(outfile==NULL){
	// 	fprintf(stderr, "Cannot open output file\n");
	// 	exit(-1);
	// }

	// SPN spn = SPN("example/msnbc_0-0.spn");

	// get_results_from_file(outfile, &spn, infile);

	// fclose(infile);
	// fclose(outfile);


	return simple_analysis(argc, argv);
	return complete_analysis(argc, argv);
}

int get_diffs(FILE* outfile, SPN* spn, FILE* double_float, FILE* posit){
	double max_rel_error = 0;
	double mean_rel_error = 0;

	int double_exp, posit_exp;
	double double_mant, posit_mant;

	int exp;
	double mant;
	for(int i=0;i<50000;i++){
		fscanf(double_float, "%lf *2^ %d\n",  &double_mant, &double_exp);
		fscanf(posit, "%lf *2^ %d\n",  &posit_mant, &posit_exp);

		double myDouble = ldexp(double_mant, double_exp);
		double myPosit = ldexp(posit_mant, posit_exp);

		double rel_error = fabs((myPosit/myDouble)-1.0);
		mean_rel_error += rel_error;

		max_rel_error = fmax(max_rel_error, rel_error);
		if(max_rel_error==rel_error){
			mant = frexp(rel_error, &exp);
		}
	}
	mean_rel_error /= 50000;
	double mmant; int mexp;
	mmant = frexp(mean_rel_error, &mexp);

	fprintf(outfile, "%lf *2^ %d\n", mant, exp);
	fprintf(outfile, "%lf *2^ %d\n", mmant, mexp);
	return 0;
}

int get_results_from_file(FILE *outfile, SPN* spn, FILE *infile){
	/* The input files comes from
	<https://github.com/UCLA-StarAI/Density-Estimation-Datasets/>
	*/
	int **literals = spn->create_literals();
	double result = 0.0, mantissa;
	int exp;


	int curr_lit = 0;
	while(!feof(infile)){
		for(int i=0;i<spn->n_lit;i++){
			fscanf(infile, "%d,", &curr_lit);
			literals[i][0] = curr_lit==0 ? 0 : 1;
			literals[i][1] = curr_lit==0 ? 1 : 0;
		}

		num_size_t size;
		size.nBits = 16;
		size.es = 4;
		// Posit value = spn->compute_real_posit(literals, size);
		double value = spn->compute_exact(literals);
		// result = value.getDouble();
		result = value;
		mantissa = frexp(result, &exp);
		fprintf(outfile, "%lf *2^ %d\n", mantissa, exp);
	}


	return 0;
}


int simple_analysis(int argc, char* argv[]){

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
	fprintf(f, "Input filename: %s\n", in_filename);
	fprintf(f, "Output format : \n");
	fprintf(f, "Directory Filename nNodes nMult nAdd "
				"[isPosit nBits es min_err max_err] "
				"[isPosit nBits es min_err max_err]\n");

	int **literals = spn.create_literals();
	for(int i=0;i<spn.n_lit;i++){
		literals[i][0] = 1;
		literals[i][1] = 1;
	}
	num_size_t posit_size;
	posit_size.nBits = 16;
	posit_size.es = 4;
	Posit number = spn.compute_real_posit(literals, posit_size);
	// printf("Value : %lf\n", number.getDouble());
	// number.print();

	fileinfo_t fileinfo;
	fileinfo.dir = "None";
	fileinfo.filename = in_filename;
	fileinfo.nNodes = spn.n_node;
	fileinfo.nMult = spn.n_multiplier;
	fileinfo.nAdd = spn.n_adder;

	analyse_spn(&spn, f, fileinfo);


	if(f!=stdout){
		fclose(f);
	}
	return 0;
}


int complete_analysis(int argc, char* argv[]){
	// =========================================================================
	// INPUT DATA
	// =========================================================================
	std::string base_path = "/home/gennart/Documents/MAI/Thesis/nshah/spn";
	std::list<string> subFolders;
	subFolders.push_back("adult");
	subFolders.push_back("audio");
	subFolders.push_back("bbc");
	subFolders.push_back("cpu");
	subFolders.push_back("dna");
	subFolders.push_back("jester");
	subFolders.push_back("msnbc");

	// =========================================================================
	// OUTPUT DATA
	// =========================================================================
	const char* out_filename = "stats_3.csv";
	FILE* f = fopen(out_filename, "w");
	if(f == NULL){
		fprintf(stderr, "Cannot open file : %s\n", out_filename);
		exit(-1);
	}

	fprintf(f, "Output format : \n");
	fprintf(f, "Directory Filename nNodes nMult nAdd "
				"[isPosit nBits es min_err max_err] "
				"[isPosit nBits es min_err max_err]\n");

	// =========================================================================
	// Long loop
	// =========================================================================
	std::list<string>::iterator it;
	for(it=subFolders.begin();it!=subFolders.end();it++){
		fprintf(stdout, "Entering next folder\n");
		string tmp_path = pathAppend(base_path, *it);
		struct dirent *entry = NULL;
		DIR *dp = opendir(tmp_path.c_str());
		if(dp != NULL){
			while((entry = readdir(dp))){
				string filename(entry->d_name);
				if(filename.compare(".")!=0 && filename.compare("..")!=0){

					string spn_path = pathAppend(tmp_path, filename);

					SPN spn = SPN(spn_path.c_str());

					fileinfo_t fileinfo;
					fileinfo.dir = tmp_path;
					fileinfo.filename = filename;
					fileinfo.nNodes = spn.n_node;
					fileinfo.nMult = spn.n_multiplier;
					fileinfo.nAdd = spn.n_adder;

					analyse_spn(&spn, f, fileinfo);
				}

			}
		}
		closedir(dp);
	}

	fclose(f);

	return 0;
}


std::string pathAppend(const std::string& p1, const std::string& p2){
   char sep = '/';
   string tmp = p1;

  if (p1[p1.length()] != sep) { // Need to add a
     tmp += sep;                // path separator
     return(tmp + p2);
  }
  else
     return(p1 + p2);
}


int analyse_spn(SPN* spn, FILE *f, fileinfo_t fileinfo){
// =============================================================================
// Error analysis
// =============================================================================

	Posit_err Posit_repr;
	num_size_t posit_size;
	err_t posit_error;


	Float_err Float_repr;
	num_size_t float_size;
	err_t float_error;

	int CUSTOM_NBITS[] = {8, 12, 16, 20, 24, 28};
	int CUSTOM_ES[] = {0, 2, 3, 4, 6, 8, 10, 11};


	list<int> nbits_list(CUSTOM_NBITS, CUSTOM_NBITS+sizeof(CUSTOM_NBITS)/sizeof(int));
	list<int> es_list(CUSTOM_ES, CUSTOM_ES+sizeof(CUSTOM_ES)/sizeof(int));

	for(list<int>::iterator nb=nbits_list.begin(); nb!=nbits_list.end(); nb++){
		err_t best_error_posit;
		best_error_posit.max_rel_error = INFINITY;
		best_error_posit.min_rel_error = INFINITY;
		best_error_posit.out_of_range = 1;
		err_t best_error_float;
		best_error_float.max_rel_error = INFINITY;
		best_error_float.min_rel_error = INFINITY;
		best_error_float.out_of_range = 1;


		// Find best exponent for posit
		for(list<int>::iterator es=es_list.begin(); es!=es_list.end(); es++){
			if(*es > *nb){
				break;
			}

			posit_size.nBits = *nb;
			posit_size.es = *es;

			err_t curr_err = spn->compute_err(&Posit_repr, posit_size);
			if((curr_err.min_rel_error < best_error_posit.min_rel_error && curr_err.out_of_range==0) || best_error_posit.out_of_range==1){
				best_error_posit = curr_err;
			}
		}

		// Find best exponent for float
		for(list<int>::iterator es=es_list.begin(); es!=es_list.end(); es++){
			if(*es > *nb){
				break;
			}

			float_size.nBits = *nb;
			float_size.es = *es;

			err_t curr_err = spn->compute_err(&Float_repr, float_size);
			if((curr_err.max_rel_error < best_error_float.max_rel_error && curr_err.out_of_range==0) || best_error_float.out_of_range==1){
				best_error_float = curr_err;
			}
		}



		// fprintf(f, "Min val posit : %lf \t %d\n", best_error_posit.min_value, best_error_posit.min_value==0);
		// fprintf(f, "Min val float : %lf \t %dp\n", best_error_float.min_value, best_error_float.min_value==0);
		print_err(f, best_error_posit, fileinfo, 1);
		print_err(f, best_error_float, fileinfo, 0);
		fprintf(f, "\n");
	}

	return 0;
}


// int tight_posit_spn(SPN* spn, num_size_t size){

// 	int **literals = spn->create_literals();
// 	for(int i=0;i<spn->n_lit;i++){
// 		int j = rand() % 3;
// 		literals[i][0] = j >= 1 ? 1 : 0;
// 		literals[i][1] = j != 1 ? 0 : 1;
// 	}

// 	Posit pos = spn->compute_real_posit(literals, size);
// 	myFloat flo = spn->compute_real_float(literals, size);


// }

// int main(int argc, char* argv[]){

// 	char *in_filename, *out_filename;
// 	if(argc == 3){
// 		in_filename = argv[1];
// 		out_filename = argv[2];
// 	}else if(argc == 2){
// 		in_filename = argv[1];
// 		out_filename = NULL;
// 	}else{
// 		cout << "Usage : ./exe filename.spn outfile.txt" << endl;
// 		const char *default_name = "example/example_03.spn";
// 		in_filename = (char*) default_name;
// 		out_filename = NULL;
// 	}

// 	FILE *f;
// 	if(out_filename == NULL){
// 		f = stdout;
// 	}else{
// 		f = fopen((const char*)out_filename, "w");
// 	}
// 	if(f==NULL){
// 		cerr << "Cannot open file! : " << out_filename <<endl;
// 		exit(0);
// 	}


// 	SPN spn = SPN(in_filename);
// 	printf("%s\n", in_filename);

// 	analyse_spn(&spn, f);
// // #ifdef SIMPLE_TEST
// // 	simple_test(&spn, f);
// // #else
// // 	complete_test(&spn, f);
// // #endif


// 	if(f!=stdout){
// 		fclose(f);
// 	}
// }

// #ifdef SIMPLE_TEST
// int simple_test(SPN *spn, FILE* f){

// 	// printf("Network is decomposable : %d\n", spn.isDecomposable());
// 	// printf("Network is deterministic : %d\n", spn.isDeterministic());
// 	// printf("Network is smooth : %d\n", spn.isSmooth());

// 	printf("Compute output of SPN\n");
// 	int **literals = spn->create_literals();


// 	while(spn->next_literals(literals) == 0){
// 		spn->print_literals(f, literals);
// 		fprintf(f, "%lf\n", spn->compute_exact(literals));
// 	}

// 	spn->delete_literals(literals);

// 	printf("Compute maximum error of SPN\n");
// 	Posit_err Posit_repr;
// 	num_size_t posit_size;
// 	posit_size.nBits = 18;
// 	posit_size.es = 3;

// 	Float_err Float_repr;
// 	num_size_t float_size;
// 	float_size.nBits = 16;
// 	float_size.es = 3;

// 	err_t posit_err = spn->compute_err(&Posit_repr, posit_size);
// 	err_t float_err = spn->compute_err(&Float_repr, float_size);

// 	printf("For posit\n");
// 	print_err(posit_err);
// 	printf("For float\n");
// 	print_err(float_err);

// 	return 0;
// }

// #else

// int complete_test(SPN *spn, FILE* f){

// 	int nBits[] = {8, 12, 16, 20, 24, 28, 31};
// 	list<int> NBits (nBits, nBits+sizeof(nBits)/sizeof(int));

// 	int es[] = {0, 2, 4, 6, 8, 10, 11};
// 	list<int> ES (es, es+sizeof(es)/sizeof(int));

// 	Posit_err Posit_repr;
// 	list<err_t> posit_error;
// 	Float_err Float_repr;
// 	list<err_t> float_error;

// 	num_size_t size;

// 	// =====================================================================
// 	// Report of utilization
// 	// =====================================================================
// 	int report_size[] = {8, 16, 32};
// 	for(int i=0;i<3;i++){
// 		size.nBits = report_size[i];
// 		spn->report_utilization(&Posit_repr, size);
// 	}


// 	// Compute error bound
// 	for(list<int>::iterator nb=NBits.begin(); nb!=NBits.end(); nb++){
// 		err_t P_err = err_init(size);
// 		P_err.max_rel_error = INFINITY; P_err.min_rel_error = INFINITY;
// 		err_t F_err = err_init(size);
// 		F_err.max_rel_error = INFINITY; F_err.min_rel_error = INFINITY;

// 		// =====================================================================
// 		// Compute error for best exponent size
// 		// =====================================================================
// 		for(list<int>::iterator es=ES.begin(); es!=ES.end() && *es<*nb;es++){
// 			size.nBits = *nb;
// 			size.es = *es;

// 			err_t err;
// 			if(size.nBits <= 31){
// 				err = spn->compute_err(&Posit_repr, size);
// 				if(err.max_rel_error < P_err.max_rel_error && err.out_of_range==0){
// 					P_err = err;
// 				}
// 			}

// 			err = spn->compute_err(&Float_repr, size);
// 			if((err.max_rel_error < F_err.max_rel_error && err.out_of_range==0)  || (F_err.max_rel_error==INFINITY && *es==11)){
// 				F_err = err;
// 			}
// 		}
// 		int** literals = spn->create_literals();
// 		for(int i=0;i<spn->n_lit;i++){
// 			literals[i][0] = 1;
// 			literals[i][1] = 1;
// 		}
// 		// spn->random_literals(literals);

// 		// =====================================================================
// 		// Compute real values for posit and floatin point
// 		// =====================================================================
// 		double exact_val = spn->compute_exact(literals);
// 		// Posit
// 		size.nBits = P_err.nBits;
// 		size.es = P_err.es;
// 		Posit result_posit = spn->compute_real_posit(literals, size);
// 		double res_pos = result_posit.getDouble();
// 		double exact_pos = exact_val;
// 		double error_bound_pos = P_err.max_rel_error;
// 		printf("(%d, %d) Posit %lf ~==~ %lf\t diff ==%lf < %lf\n", size.nBits, size.es, exact_pos, res_pos, exact_pos-res_pos, error_bound_pos);

// 		// Float
// 		size.nBits = F_err.nBits;
// 		size.es = F_err.es;
// 		myFloat result_float = spn->compute_real_float(literals, size);
// 		double res_flt = result_float.getDouble();
// 		double exact_flt = exact_val;
// 		double error_bound_flt = F_err.max_rel_error;
// 		printf("(%d, %d) Float %lf ~==~ %lf\t diff ==%lf < %lf\n", size.nBits, size.es, exact_flt, res_flt, exact_flt-res_flt, error_bound_flt);

// 		posit_error.push_back(P_err);
// 		float_error.push_back(F_err);
// 	}

// 	// Compute Real value with best nBits and best es
// 	for(list<err_t>::iterator e=posit_error.begin(); e!=posit_error.end(); e++){
// 		size.nBits = e->nBits;
// 		size.es = e->es;

// 		int** literals = spn->create_literals();
// 		for(int i=0;i<spn->n_lit;i++){
// 			literals[i][0] = 1;
// 			literals[i][1] = 1;
// 		}
// 		Posit result_posit = spn->compute_real_posit(literals, size);
// 		printf("(%d, %d) Posit value : %f\n", size.nBits, size.es, result_posit.getDouble());


// 	}

// 	for(list<err_t>::iterator e=float_error.begin(); e!=float_error.end(); e++){
// 		size.nBits = e->nBits;
// 		size.es = e->es;

// 		int **literals = spn->create_literals();
// 		for(int i=0;i<spn->n_lit;i++){
// 			literals[i][0] = 1;
// 			literals[i][1] = 1;
// 		}
// 		myFloat result_float = spn->compute_real_float(literals, size);
// 		printf("(%d, %d) Float value : %f\n", size.nBits, size.es, result_float.getDouble());
// 	}

// 	cout << "Posit " << endl;
// 	for(list<err_t>::iterator e=posit_error.begin(); e!=posit_error.end(); e++){
// 		print_err(*e);
// 	}
// 	cout << endl;

// 	cout << "FLoat " << endl;
// 	for(list<err_t>::iterator e=float_error.begin(); e!=float_error.end(); e++){
// 		print_err(*e);
// 	}
// 	cout << endl;

// 	return 0;
// }
