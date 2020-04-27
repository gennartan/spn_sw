#ifndef __SPN_H__
#define __SPN_H__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <math.h>
#include "posit.h"
#include "utils.h"

#define MAXLINE 100

// =============================================================================
// Node  =======================================================================
// =============================================================================

class Node{
public:
	int id;
	int literal;
	double weight;
	int type;

	// Information on node children
	int n_children;
	int* children_ids;
	Node** children;

	int n_parents;
	int* parents_ids;
	Node** parents;

	// Litteral that are decendant of this node
	int n_litt;
	int *litt;

	// depth of the node == the maximum distance to a literal node
	int depth;

	// 	An SPN is deterministic if aull disjunct are logically disjoints
	// 	That is : Each child of a sum node cannot be true at the same time
	int isDeterministic;
	// 	An SPN is decomposable if conjuncts does not share variables
	// 	That is : Each child of a product node does not contrain common litteral(s)
	int isDecomposable;
	// 	An SPN is smooth if disjuncts mention the same set of variables
	// 	That is : Children of a sum node has the same set of variables (+-litterals)
	int isSmooth;

	Node();
	~Node();

	void print();
	void set_leaf(int id, int literal);
	void set_sum(int id, int *ids, int n_children);
	void set_product(int id, int *ids, int n_children);
	void set_weight(int id, int id_1, double weight);

	double compute(int **literals, int n_lit);
	err_t compute_err(Number_representation *repr, num_size_t size, int** literals, int n_lit);
	double compute_area(Number_representation *repr, num_size_t size);
};


class SPN{
public:
	Node **spn;
	int spn_size;
	int n_lit;

	SPN(const char* filename);
	~SPN();

	void print();
	Node* search_node(int id);
	double compute(int** literals);
	err_t compute_err(Number_representation *repr, num_size_t size);
	double compute_area(Number_representation *repr, num_size_t size);
	int **create_literals();
	void delete_literals(int** literals);
	int next_literals(int **literals);
	void print_literals(FILE* f, int** literals);

	int isDeterministic();
	int isDecomposable();
	int isSmooth();
};

#endif