#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct m_tree_t {
	struct m_tree_t	*left;
	struct m_tree_t	*right;
	int	height;
	int key;
	int side;
	int other;
	int min;
	int max;
	int leftmin;
	int rightmax;
	int measure;
} m_tree_t;

m_tree_t * get_node(){
	m_tree_t *node;
	node = (m_tree_t *) malloc(sizeof(m_tree_t));
	node->left = NULL;
	node->right = NULL;
	node->key = 0;
	node->height = 0;
	node->side = 0;
	node->other = 0;
	node->min = 0;
	node->max = 0;
	node->leftmin = 0;
	node->rightmax = 0;
	node->measure = 0;
	return (node);
}

m_tree_t * create_m_tree(){
	m_tree_t *tree;
	tree = (m_tree_t *) malloc(sizeof(m_tree_t));
	return tree;
}

m_tree_t * insert(m_tree_t *tree, int a, int b) {

}

m_tree_t * delete(m_tree_t *tree, int a, int b){

}

void insert_interval(m_tree_t * tree, int a, int b)
{
}


void delete_interval(m_tree_t * tree, int a, int b)
{
}


int query_length(m_tree_t * tree) {

}


void destroy_m_tree(m_tree_t *tree) {

}

