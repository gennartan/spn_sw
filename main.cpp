#include <stdio.h>
#include <dirent.h>
#include <iostream>
#include <string>
#include <list>
#include "spn.h"

using namespace std;

int complete_analysis(int argc, char* argv[]);
int simple_analysis(int argc, char* argv[]);
std::string pathAppend(const std::string& p1, const std::string& p2);
int analyse_spn(SPN* spn, FILE *f, fileinfo_t fileinfo);


int main(int argc, char*argv[]){
	return simple_analysis(argc, argv);
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
	printf("%s\n", in_filename);

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
	const char* out_filename = "stats.csv";
	FILE* f = fopen(out_filename, "w");
	if(f == NULL){
		fprintf(stderr, "Cannot open file : %s\n", out_filename);
		exit(-1);
	}

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

int analyse_spn(SPN* spn, FILE *f, fileinfo_t fileinfo){
// =============================================================================
// Error analysis
// =============================================================================

	// fprintf(f, "===========================================================\n");
	// fprintf(f, "Typical POSIT size error bound:\n");
	// fprintf(f, "===========================================================\n");
	Posit_err Posit_repr;
	num_size_t posit_size;
	err_t posit_error;

	// int NBITS_POSIT[] = {8, 16, 31};
	// int ES_POSIT[] = {0, 1 ,2};

	// for(int i=0;i<3;i++){
	// 	posit_size.nBits = NBITS_POSIT[i];
	// 	posit_size.es = ES_POSIT[i];
	// 	posit_error = spn->compute_err(&Posit_repr, posit_size);
	// 	print_err(f, posit_error);
	// }

	// fprintf(f, "===========================================================\n");
	// fprintf(f, "Typical FLOAT size error bound:\n");
	// fprintf(f, "===========================================================\n");
	Float_err Float_repr;
	num_size_t float_size;
	err_t float_error;

	// int NBITS_FLOAT[] = {8, 16, 32, 64};
	// int ES_FLOAT[] = {4, 5, 8, 11};

	// for(int i=0;i<4;i++){
	// 	float_size.nBits = NBITS_FLOAT[i];
	// 	float_size.es = ES_FLOAT[i];
	// 	float_error = spn->compute_err(&Float_repr, float_size);
	// 	print_err(f, float_error);
	// }

	// fprintf(f, "===========================================================\n");
	// fprintf(f, "Custom POSIT size error bound:\n");
	// fprintf(f, "===========================================================\n");
	int CUSTOM_NBITS_POSIT[] = {8, 12, 16, 20, 24, 28, 31};
	int CUSTOM_ES_POSIT[] = {0, 2, 4, 6, 8, 10, 11};


	list<int> nbits_posit_list(CUSTOM_NBITS_POSIT, CUSTOM_NBITS_POSIT+sizeof(CUSTOM_NBITS_POSIT)/sizeof(int));
	list<int> es_posit_list(CUSTOM_ES_POSIT, CUSTOM_ES_POSIT+sizeof(CUSTOM_ES_POSIT)/sizeof(int));

	for(list<int>::iterator nb=nbits_posit_list.begin(); nb!=nbits_posit_list.end(); nb++){
		err_t best_error;
		best_error.max_rel_error = INFINITY;
		best_error.min_rel_error = INFINITY;
		best_error.out_of_range = 1;

		for(list<int>::iterator es=es_posit_list.begin(); es!=es_posit_list.end(); es++){
			if(*es > *nb){
				break;
			}

			posit_size.nBits = *nb;
			posit_size.es = *es;

			err_t curr_err = spn->compute_err(&Posit_repr, posit_size);
			if((curr_err.max_rel_error < best_error.max_rel_error && curr_err.out_of_range==0) || best_error.out_of_range==1){
				best_error = curr_err;
			}
		}


		print_err(f, best_error, fileinfo, 1);
	}


	// fprintf(f, "===========================================================\n");
	// fprintf(f, "Custom FLOAT size error bound:\n");
	// fprintf(f, "===========================================================\n");
	int CUSTOM_NBITS_FLOAT[] = {8, 12, 16, 20, 24, 28, 31};
	int CUSTOM_ES_FLOAT[] = {0, 2, 4, 6, 8, 10, 11};

	list<int> nbits_float_list(CUSTOM_NBITS_FLOAT, CUSTOM_NBITS_FLOAT+sizeof(CUSTOM_NBITS_FLOAT)/sizeof(int));
	list<int> es_float_list(CUSTOM_ES_FLOAT, CUSTOM_ES_FLOAT+sizeof(CUSTOM_ES_FLOAT)/sizeof(int));

	for(list<int>::iterator nb=nbits_float_list.begin(); nb!=nbits_float_list.end(); nb++){
		err_t best_error;
		best_error.max_rel_error = INFINITY;
		best_error.min_rel_error = INFINITY;
		best_error.out_of_range = 1; // best error not set yet

		for(list<int>::iterator es=es_float_list.begin(); es!=es_float_list.end(); es++){
			if(*es > *nb){
				break;
			}
			float_size.nBits = *nb;
			float_size.es = *es;


			err_t curr_err = spn->compute_err(&Float_repr, float_size);
			if((curr_err.max_rel_error < best_error.max_rel_error && curr_err.out_of_range==0) || best_error.out_of_range==1){
				best_error = curr_err;
			}
		}

		print_err(f, best_error, fileinfo, 0);
	}


// =============================================================================
// Utilization analysis
// =============================================================================

	return 0;
}