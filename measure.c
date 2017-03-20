#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct m_tree_t {
	struct m_tree_t	*left;
	struct m_tree_t	*right;
	int head;
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

m_tree_t * head = NULL;

m_tree_t * get_node(){
	m_tree_t *node;
	node = (m_tree_t *) malloc(sizeof(m_tree_t));
	node->head = 0;
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

int getBalance(m_tree_t *node) {
    if (node == NULL) return 0;
    return node->left->height - node->right->height;
}

m_tree_t * rightRotate(m_tree_t *node) {
	m_tree_t *newNode = node->left;
	m_tree_t *tmp = newNode->right;

    newNode->right = node;
    node->left = tmp;

    node->height = max(height(node->left), height(node->right))+1;
    newNode->height = max(height(newNode->left), height(newNode->right))+1;

    return newNode;
}

m_tree_t * leftRotate(m_tree_t *node) {
	m_tree_t *newNode = node->right;
	m_tree_t *tmp = newNode->left;

    newNode->left = node;
    node->right = tmp;

    node->height = max(height(node->left), height(node->right))+1;
    newNode->height = max(height(newNode->left), height(newNode->right))+1;

    return newNode;
}

void updateHeight(m_tree_t *node){
	int l = 0, r = 0;
	if (node->left != NULL) l = node->left->height + 1;
	if (node->right != NULL) r = node->right->height + 1;
	if (l > r) node->height = l;
	else node->height = r;
}

void updateMeasure(m_tree_t *node) {
	if (node != NULL){
		if (node->side == 0){
			if (node->left != NULL && node->right != NULL){
				if (((node->right->leftmin == node->right->min) || (node->right->leftmin == node->key)) && (node->left->rightmax == node->left->max)){
					if (node->other > node->right->min){
						node->measure = node->left->measure + node->right->measure + node->right->min - node->key;
					}
					else{
						node->measure = node->left->measure + node->right->max - node->key;
					}
				}
				else if (node->left->rightmax != node->left->max){
					if (node->other > node->right->min){
						node->measure = node->left->measure + node->right->measure + node->right->min - node->left->max;
					}
					else{
						node->measure = node->left->measure + node->right->max - node->left->max;
					}
				}
				else if (node->right->leftmin != node->right->min && node->right->leftmin){
					if (node->other > node->right->min){
						node->measure = node->left->measure + node->right->measure + node->right->min - node->key;
					}
					else{
						node->measure = node->right->measure + node->right->min - node->left->min;
					}
				}
			} else {
				if (node->left == NULL && node->right == NULL) node->measure = 0;
				else {
					if (node->left != NULL)
						node->measure = node->left->measure;

					if (node->right != NULL)
						node->measure = node->right->measure;
				}
			}

		} else {
			if (node->left != NULL && node->right != NULL){
				if (((node->left->rightmax == node->left->max) || (node->left->rightmax == node->key)) && (node->right->leftmin == node->right->min)){
					if (node->other < node->left->max){
						node->measure = node->left->measure + node->right->measure + node->right->min - node->key;
					}
					else{
						node->measure = node->left->measure + node->right->max - node->key;
					}
				}
				else if (node->left->rightmax != node->left->max){
					if (node->other > node->right->min){
						node->measure = node->left->measure + node->right->measure + node->right->min - node->left->max;
					}
					else{
						node->measure = node->left->measure + node->right->max - node->left->max;
					}
				}
				else if (node->right->leftmin != node->right->min && node->right->leftmin){
					if (node->other > node->right->min){
						node->measure = node->left->measure + node->right->measure + node->right->min - node->key;
					}
					else{
						node->measure = node->right->measure + node->right->min - node->left->min;
					}
				}
			} else {
				if (node->left == NULL && node->right == NULL) node->measure = 0;
				else {
					if (node->left != NULL)
						node->measure = node->left->measure;

					if (node->right != NULL)
						node->measure = node->right->measure;
				}
			}
		}
	}
}

void restructure(m_tree_t *node) {
	int val;
	//min
	if (node->left != NULL) node->min = node->left->min;
	else node->min = node->key;
	//max
	if (node->right != NULL) node->max = node->right->max;
	else node->max = node->key;
	//leftmin
	if (node->side == 0) val = node->key;
	else val = node->other;
	if (node->left != NULL && node->right != NULL){
		node->leftmin = min(val, min(node->left->leftmin, node->right->leftmin));
	} else {
		if (node->left != NULL) node->leftmin = min(val, node->left->leftmin);
		else if (node->right != NULL) node->leftmin = min(val, node->right->leftmin);
		else node->leftmin = val;
	}
	//rightmax
	if (node->side == 1) val = node->key;
	else val = node->other;

	if (node->left != NULL && node->right != NULL){
		node->rightmax = max(val, max(node->left->rightmax, node->right->rightmax));
	} else {
		if (node->left != NULL) node->rightmax = max(val, node->left->rightmax);
		else if (node->right != NULL) node->rightmax = max(val, node->right->rightmax);
		else node->rightmax = val;
	}
	// measure
	updateMeasure(node);
}

m_tree_t * rebalance(m_tree_t *node) {
	int balance = getBalance(node);

	// Left Left Case
	if (balance > 1 && node->key < node->left->key)
		return rightRotate(node);

	// Right Right Case
	if (balance < -1 && node->key > node->right->key)
		return leftRotate(node);

	// Left Right Case
	if (balance > 1 && node->key > node->left->key) {
		node->left =  leftRotate(node->left);
		return rightRotate(node);
	}

	// Right Left Case
	if (balance < -1 && node->key < node->right->key) {
		node->right = rightRotate(node->right);
		return leftRotate(node);
	}

	/* return the (unchanged) node pointer */
	return node;
}

m_tree_t * insert(m_tree_t *node, int a, int b) {
	if (node == NULL) {
		node = get_node();
		node->key = a;
		node->other = b;
		if (a < b) node->side = 0;
		else node->side = 1;
	}
	else if (node->head == 1){
		if (node->left == NULL) {
			node->left = get_node();
			node->left->key = a;
			node->left->other = b;
			if (a < b) node->left->side = 0;
			else node->left->side = 1;
		} else {
			m_tree_t *updatedNode = insert(node->left, a, b);
			node->left = updatedNode;
		}
	}
	else if (node->key > a){
		m_tree_t *updatedNode = insert(node->left, a, b);
		node->left = updatedNode;
	}
	else if (node->key < a){
		m_tree_t *updatedNode = insert(node->right, a, b);
		node->right = updatedNode;
	}
	updateHeight(node);
	restructure(node);

	if (node == head) return node;
	else return rebalance(node);
}

m_tree_t * delete(m_tree_t *tree, int a, int b){

}

m_tree_t * create_m_tree(){
	m_tree_t *tree;
	tree = (m_tree_t *) malloc(sizeof(m_tree_t));
	tree->head = 1;
	return tree;
}

void insert_interval(m_tree_t * tree, int a, int b) {
	if (tree != NULL) {
		insert(tree, a, b);
		insert(tree, b, a);
	}
}


void delete_interval(m_tree_t * tree, int a, int b) {

}


int query_length(m_tree_t * tree) {
	if(tree != NULL){
		if(tree->head == 1 && tree->left != NULL) return tree->left->measure;
		else return tree->measure;
	} else return 0;
}


void destroy_m_tree(m_tree_t *tree) {

}

