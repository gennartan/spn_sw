#include "spn.h"

using namespace std;


// =============================================================================
// Node ========================================================================
// =============================================================================

Node::Node(){
	this->id = -1;
	this->literal = 0;
	this->weight = 0.0;
	this->type = None;

	this->n_children = 0;
	this->children_ids = NULL;
	this->children = NULL;

	this->n_parents = 0;
	this->parents_ids = NULL;
	this->parents=  NULL;

	this->n_litt = 0;
	this->litt = NULL;

	this->isDecomposable = -1;
	this->isDeterministic = -1;
	this->isSmooth = -1;

	this->depth = -1;
}

Node::~Node(){
	if(this->n_children>0){
		free(this->children);
		free(this->children_ids);
	}
	if(this->n_parents>0){
		free(this->parents);
		free(this->parents_ids);
	}
	if(this->n_litt>0){
		free(this->litt);
	}
}

void Node::print(){

	char type_name[] = "NLSPW";
		printf("Node %d type : %c \t And depth = %d\n", this->id, type_name[this->type], this->depth);
		if(this->type==L){
			printf("  --> Literal : %d\n", this->literal);
		}else if(this->type==S || this->type==P){
			printf("  --> Children IDs : ");
			for(int i=0;i<this->n_children;i++){
				printf("%d\t", this->children_ids[i]);
			}
			printf("\n");
		}else if(this->type==W){
			printf("  --> Children Id and weight : %d\t%lf\n", this->children_ids[0], this->weight);
		}

}

void Node::set_leaf(int id, int literal){
	this->type = L;
	this->id = id;
	this->literal = literal;
}

void Node::set_sum(int id, int *ids, int n_children){
	this->type = S;
	this->id = id;
	this->n_children = n_children;
	this->children_ids = (int*) malloc(n_children*sizeof(int));
	for(int i=0;i<n_children;i++){
		this->children_ids[i] = ids[i];
	}
	this->children = (Node**) malloc(n_children*sizeof(Node*));
}

void Node::set_product(int id, int *ids, int n_children){
	this->type = P;
	this->id = id;
	this->n_children = n_children;
	this->children_ids = (int*) malloc(n_children*sizeof(int));
	for(int i=0;i<n_children;i++){
		this->children_ids[i] = ids[i];
	}
	this->children = (Node**) malloc(n_children*sizeof(Node*));
}

void Node::set_weight(int id, int id_1, double weight){
	this->type = W;
	this->id = id;
	this->n_children = 1;
	this->weight = weight;
	this->children_ids = (int*) malloc(sizeof(int));
	this->children_ids[0] = id_1;
	this->children = (Node**) malloc(sizeof(Node*));
}

double Node::compute_exact(int **literals, int n_lit){
	switch(this->type){
		case(S):{
			double result = 0.0;
			for(int i=0;i<this->n_children;i++){
				result += this->children[i]->compute_exact(literals, n_lit);
			}
			// cout << "Intermediate sum result " << result << endl;
			return result;
		}
		break;
		case(P):{
			double result = 1.0;
			for(int i=0;i<this->n_children;i++){
				result *= this->children[i]->compute_exact(literals, n_lit);
			}
			// cout << "Intermediate product result " << result << endl;
			return result;
		}
		break;
		case(W):{
			double result = this->weight * this->children[0]->compute_exact(literals, n_lit);
			// cout << "Intermediate weight result " << result << endl;
			return result;
		}
		break;
		case(L):{
			int sign = this->literal > 0;
			int nbr = sign ? this->literal : -this->literal;
			// cout << sign << nbr << endl;
			// cout << "Integer value: " << literals[nbr-1][sign] << endl;
			double result = static_cast<double>(literals[nbr-1][sign]);
			// cout << "Intermediate literal result " << result << endl;
			return result;
		}
		break;
	}
	return -1;
}

Posit Node::compute_real_posit(int **literals, int n_lit, num_size_t size){
	switch(this->type){
		case(S):{
			Posit result = Posit(size.nBits+1, size.es);
			result.set(0.0);
			for(int i=0;i<this->n_children;i++){
				result = result + this->children[i]->compute_real_posit(literals, n_lit, size);
			}
			// cout << "Intermediate sum result " << result << endl;
			return result;
		}
		break;
		case(P):{
			Posit result = Posit(size.nBits+1, size.es);
			result.set(1.0);
			for(int i=0;i<this->n_children;i++){
				result = result * this->children[i]->compute_real_posit(literals, n_lit, size);
			}
			return result;
		}
		break;
		case(W):{
			Posit result = Posit(size.nBits+1, size.es);
			Posit weight = Posit(size.nBits+1, size.es);
			weight.set(this->weight);
			result = weight * this->children[0]->compute_real_posit(literals, n_lit, size);
			return result;
		}
		break;
		case(L):{
			Posit result = Posit(size.nBits+1, size.es);
			int sign = this->literal > 0;
			int nbr = sign ? this->literal : -this->literal;
			// cout << sign << nbr << endl;
			// cout << "Integer value: " << literals[nbr-1][sign] << endl;
			result.set(static_cast<double>(literals[nbr-1][sign]));
			// cout << "Intermediate literal result " << result << endl;
			return result;
		}
		break;
	}
	Posit dummy = Posit(size.nBits+1, size.es);
	dummy.set(-1.0);
	printf("Dummy error\n");
	return dummy;
}

myFloat Node::compute_real_float(int **literals, int n_lit, num_size_t size){
	switch(this->type){
		case(S):{
			myFloat result = myFloat(size.nBits+1, size.es);
			result.set(0.0);
			for(int i=0;i<this->n_children;i++){
				result = result + this->children[i]->compute_real_float(literals, n_lit, size);
			}
			// cout << "Intermediate sum result " << result << endl;
			return result;
		}
		break;
		case(P):{
			myFloat result = myFloat(size.nBits+1, size.es);
			result.set(1.0);
			for(int i=0;i<this->n_children;i++){
				result = result * this->children[i]->compute_real_float(literals, n_lit, size);
			}
			return result;
		}
		break;
		case(W):{
			myFloat result = myFloat(size.nBits+1, size.es);
			myFloat weight = myFloat(size.nBits+1, size.es);
			weight.set(this->weight);
			result = weight * this->children[0]->compute_real_float(literals, n_lit, size);
			return result;
		}
		break;
		case(L):{
			myFloat result = myFloat(size.nBits+1, size.es);
			int sign = this->literal > 0;
			int nbr = sign ? this->literal : -this->literal;
			// cout << sign << nbr << endl;
			// cout << "Integer value: " << literals[nbr-1][sign] << endl;
			result.set(static_cast<double>(literals[nbr-1][sign]));
			// cout << "Intermediate literal result " << result << endl;
			return result;
		}
		break;
	}
	myFloat dummy = myFloat(size.nBits+1, size.es);
	dummy.set(-1.0);
	printf("Dummy error\n");
	return dummy;
}


double smallest_weight = 1.0;

err_t Node::compute_err(Number_representation *repr, num_size_t size, int** literals, int n_lit){
	switch(this->type){
		case(S):{
			err_t res = this->children[0]->compute_err(repr, size, literals, n_lit);
			for(int i=1;i<this->n_children;i++){
				res = repr->addition_error(size, res, this->children[i]->compute_err(repr, size, literals, n_lit));
			}
			return res;
		}
		break;
		case(P):{
			err_t res = this->children[0]->compute_err(repr, size, literals, n_lit);
			for(int i=1;i<this->n_children;i++){
				res = repr->multiplication_error(size, res, this->children[i]->compute_err(repr, size, literals, n_lit));
				// print_err(res);
			}
			return res;
		}
		break;
		case(W):{
			err_t res;
			err_t weight = repr->encoding_error(size, this->weight);
			res = repr->multiplication_error(size, weight, this->children[0]->compute_err(repr, size, literals, n_lit));
			return res;
		}
		break;
		case(L):{
			int sign = this->literal > 0;
			int nbr = sign ? this->literal : -this->literal;

			double result = static_cast<double>(literals[nbr-1][sign]);

			err_t res = repr->encoding_error(size, result);
			return res;
		}
	}
	// error
	return repr->encoding_error(size, -1);
}

double Node::compute_area(Number_representation *repr, num_size_t size){
	switch(this->type){
		case(S):{
			return repr->adder_area(size);
		}
		break;
		case(P):
		case(W):{
			return repr->multiplier_area(size);
		}
		break;
		default:
			return 0.0;
	}
}

// =============================================================================
// =============================================================================
// =============================================================================
// =============================================================================


SPN::SPN(const char* filename){
	this->spn = NULL;
	this->spn_size = 0;
	this->n_lit = 0;
	this->n_multiplier = 0;
	this->n_adder = 0;
	this->n_node = 0;
	FILE* f = fopen(filename, "r");
	if(f==NULL){
		cerr << "Error opening file: " << filename << endl;
		return;
	}

	char line[MAXLINE];


	do{
		fgets(line, MAXLINE, f);
	}while(line[0] != 's');
	sscanf(line, "spn %d\n", &this->spn_size);

	this->spn = (Node**) malloc(this->spn_size*sizeof(Node*));
	int i = 0;
	while(fgets(line, MAXLINE, f) != NULL){
		int id, lit, n_child, id_1, id_2;
		double weight;
		Node* node = new Node();
		if (line[0] == 'L'){
			sscanf(line, "L %d %d\n", &id, &lit);
			node->set_leaf(id, lit);
		}else if (line[0] == 'S'){
			sscanf(line, "S %d %d %d %d\n", &id, &n_child, &id_1, &id_2);
			if(n_child!=2){
				cerr  << "Error: sum has more than 2 children :" << n_child << endl;
				exit(-1);
			}
			int ids[2] = {id_1, id_2};
			node->set_sum(id, ids, n_child);
		}else if (line[0] == 'P'){
			sscanf(line, "P %d %d %d %d\n", &id, &n_child, &id_1, &id_2);
			if(n_child!=2){
				cerr << "Error: product has more than 2 children : " << n_child << endl;
				exit(-1);
			}
			int ids[2] = {id_1, id_2};
			node->set_product(id, ids, 2);
		}else if (line[0] == 'W'){
			sscanf(line, "W %d %d %lf\n", &id, &id_1, &weight);
			node->set_weight(id, id_1, weight);
		}else if(line[0] == 's' || line[0] == 'c'){
			continue;
		}else{
			cerr << "Error: Unknown node type" << endl << line;
			exit(-1);
		}


		if(node->id != -1){
			this->spn[i] = node;
			i++;
		}else{
			delete node;
		}

	}
	fclose(f);

	queue<Node*> q;
	q.push(this->spn[this->spn_size-1]);

	// Set node Descendance and acsendance
	while(!q.empty()){
		Node* curr_node = q.front();
		q.pop();

		for(int i=0;i<curr_node->n_children;i++){
			Node* child = this->search_node(curr_node->children_ids[i]);
			curr_node->children[i] = child;

			// check if parent is this parent already set
			int parent_already_present = 0;
			for(int j=0;j<child->n_parents;j++){
				if(curr_node->id == child->parents_ids[j]){
					parent_already_present = 1;
					break;
				}
			}
			if(!parent_already_present){
				child->n_parents++;
				child->parents_ids = (int*) realloc(child->parents_ids, child->n_parents*sizeof(int));
				child->parents_ids[child->n_parents-1] = curr_node->id;
				child->parents = (Node**) realloc(child->parents, child->n_parents*sizeof(Node*));
				child->parents[child->n_parents-1] = curr_node;
			}

			q.push(child);
		}
	}


	for(int i=0;i<this->spn_size;i++){
		if(this->spn[i]->type == L){
			this->spn[i]->depth = 0;
			q.push(this->spn[i]);
		}
	}
	while(!q.empty()){
		Node* curr_node = q.front();
		q.pop();

		for(int i=0;i<curr_node->n_parents;i++){
			Node* parent = curr_node->parents[i];

			if(curr_node->depth+1 > parent->depth){
				parent->depth = curr_node->depth+1;
				q.push(parent);
			}
		}
	}

	// Count adder and multiplier
	this->n_adder = 0;
	this->n_multiplier = 0;
	this->n_node = 0; // every node except literal
	for(int i=0;i<this->spn_size;i++){
		if(this->spn[i]->type == S){
			this->n_adder++;
		}else if(this->spn[i]->type==P || this->spn[i]->type==W){
			this->n_multiplier++;
		}

		if(this->spn[i]->type != L){
			this->n_node++;
		}
	}
}

SPN::~SPN(){
	for(int i=0;i<this->spn_size;i++){
		delete spn[i];
	}
	free(this->spn);
}

void SPN::print(){
	for(int i=0;i<this->spn_size;i++){
		spn[i]->print();
	}
}


Node* SPN::search_node(int id){
	for(int i=0;i<this->spn_size;i++){
		if(this->spn[i]->id == id){
			return this->spn[i];
		}
	}
	return NULL;
}

double SPN::compute_exact(int** literals){
	return this->spn[this->spn_size-1]->compute_exact(literals, this->n_lit);
}

Posit SPN::compute_real_posit(int** literals, num_size_t size){
	return this->spn[this->spn_size-1]->compute_real_posit(literals, this->n_lit, size);
}

myFloat SPN::compute_real_float(int **literals, num_size_t size){
	return this->spn[this->spn_size-1]->compute_real_float(literals, this->n_lit, size);
}


err_t SPN::compute_err(Number_representation *repr, num_size_t size){
	int **literals = this->create_literals();
	for(int i=0;i<this->n_lit;i++){
		literals[i][0] = 1;
		literals[i][1] = 1;
	}
	err_t res = this->spn[this->spn_size-1]->compute_err(repr, size, literals, this->n_lit);
	this->delete_literals(literals);
	return res;
}

double SPN::compute_area(Number_representation *repr, num_size_t size){
	double area = 0.0;
	for(int i=0;i<this->spn_size;i++){
		area += this->spn[i]->compute_area(repr, size);
	}
	return area;
}

utilization_t SPN::compute_utilization(Number_representation *repr, num_size_t size){
	utilization_t ut;

	int n_adder=0;
	int n_multiplier=0;
	for(int i=0;i<this->spn_size;i++){
		utilization_t node_ut;
		if(this->spn[i]->type == S){
			node_ut = repr->adder_utilization(size);
			n_adder++;
		}else if(this->spn[i]->type == P | this->spn[i]->type == W){
			node_ut = repr->multiplier_utilization(size);
			n_multiplier++;
		}
		ut.nLUT += node_ut.nLUT;
		ut.nMULT += node_ut.nMULT;
		ut.nMUXFX += node_ut.nMUXFX;
	}

	// printf("N_ADDER      : %d\n", n_adder);
	// printf("N_MULTIPLIER : %d\n", n_multiplier);
	return ut;
}

void SPN::report_utilization(Number_representation *repr, num_size_t size){
	utilization_t ut = this->compute_utilization(repr, size);

	printf("==========================================\n");
	printf("Utilization report (%d bits) : \n", size.nBits);
	printf("==========================================\n");
	printf("LUTS : %d / 66500\n", ut.nLUT);
	printf("MULT : %d\n", ut.nMULT);
	printf("MUXF : %d\n", ut.nMUXFX);
	printf("MULT + MUXF : %d / 220\n", ut.nMULT + ut.nMUXFX);
}

int** SPN::create_literals(){
	int max_lit = 0;
	for(int i=0;i<this->spn_size;i++){
		if(this->spn[i]->type == L){
			int lit = this->spn[i]->literal < 0 ? -this->spn[i]->literal : this->spn[i]->literal;
			max_lit = lit > max_lit ? lit : max_lit;
		}
	}
	if(max_lit==0){
		cerr << "No literals found in spn" << endl;
		exit(-1);
	}

	this->n_lit = max_lit;
	// cout << "max lit: " << max_lit << endl;
	int** literals = (int**) malloc(max_lit*sizeof(int*));
	for(int i=0;i<max_lit;i++){
		literals[i] = (int*) malloc(2*sizeof(int));
		literals[i][0] = 0;
		literals[i][1] = 0;
	}

	return literals;
}

void SPN::delete_literals(int** literals){
	for(int i=0;i<this->n_lit;i++){
		free(literals[i]);
	}
	free(literals);
}

int SPN::next_literals(int** literals){

	// compute next literal
	int carry = 1;
	int valid = 1;
	int finish = 0;

	do{
		carry = 1;
		for(int i=0;i<this->n_lit;i++){
			if(literals[i][0] == 0 && carry == 1){
				literals[i][0] = 1;
				carry = 0;
			}else if(literals[i][0] == 1 && carry == 1){
				literals[i][0] = 0;
				carry = 1;
				if(literals[i][1] == 0){
					literals[i][1] = 1;
					carry = 0;
				}else{ // literals[i][1] == 1
					literals[i][1] = 0;
					carry = 1;
				}
			}
		}

		// check if everything is ok :-)
		valid = 1;
		for(int i=0;i<this->n_lit;i++){
			if(literals[i][0]==0 && literals[i][1]==0){
				valid = 0;
				break;
			}
		}

		finish = 1;
		for(int i=0;i<this->n_lit;i++){
			if(literals[i][0]==0 || literals[i][1]==0){
				finish = 0;
				break;
			}
		}
		if(finish == 1){
			return finish;
		}
	}while(!valid && !finish);
	// if(!valid && carry==0){
	// 	carry = this->next_literals(literals);
	// }

	return carry;
}

int SPN::random_literals(int** literals){

	for(int i=0;i<this->n_lit;i++){
		int rnd = rand() % 3;
		if(rnd==0){
			literals[i][0] = 0;
			literals[i][1] = 1;
		}else if(rnd==1){
			literals[i][0] = 1;
			literals[i][1] = 0;
		}else{
			literals[i][0] = 1;
			literals[i][1] = 1;
		}
	}
	return 0;
}

void SPN::print_literals(FILE* f, int** literals){
	for(int i=0;i<this->n_lit;i++){
		fprintf(f, "%d %d ", literals[i][0], literals[i][1]);
	}
	// cout << endl;
}

int SPN::isDeterministic(){
	return this->spn[this->spn_size-1]->isDeterministic;
}

int SPN::isDecomposable(){
	return this->spn[this->spn_size-1]->isDecomposable;
}

int SPN::isSmooth(){
	return this->spn[this->spn_size-1]->isSmooth;
}
