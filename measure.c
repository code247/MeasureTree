#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct intervals {
	int left;
	int right;
	struct intervals  *next;
} intervals;

typedef struct m_tree_t {
	struct m_tree_t	*left;
	struct m_tree_t	*right;
	struct intervals *intv;
	int key;
	int min;
	int max;
	int leftmin;
	int rightmax;
	int measure;
	int	height;
} m_tree_t;

int min(int a, int b) {
	if (a < b) return a;
	else return b;
}

int max(int a, int b) {
	if (a > b) return a;
	else return b;
}

m_tree_t *get_node() {
	m_tree_t *tmp_node;
	tmp_node = (m_tree_t *) malloc(sizeof(m_tree_t));
	tmp_node->right = NULL;
	tmp_node->left = NULL;
	tmp_node->height = 0;
	tmp_node->key = 0;
	tmp_node->max = INFINITY;
	tmp_node->min = -INFINITY;
	tmp_node->leftmin = INFINITY;
	tmp_node->rightmax = -INFINITY;
	tmp_node->measure = 0;
	tmp_node->intv = NULL;
	return( tmp_node );
}

m_tree_t *create_m_tree() {
	m_tree_t *tmp_node;
	tmp_node = get_node();
	return( tmp_node );
}

void addInterval(intervals *intv, int l, int r){
	if(intv != NULL){
		intervals *last = NULL;
		intervals *tmpI = intv;
		while(tmpI != NULL) {
			last = tmpI;
			tmpI = tmpI->next;
		}
		intervals *tmp = (intervals *) malloc(sizeof(intervals));
		tmp->left = l;
		tmp->right = r;
		tmp->next = NULL;
		last->next = tmp;
	}
}

int deleteInterval(m_tree_t *tmp_node, int l, int r){
	if (l > r){
		int t = l;
		l = r;
		r = t;
	}
	intervals *intv = (intervals *) tmp_node->left;
	if(intv != NULL){
		intervals *tmp = intv;
		intervals *prev = NULL;
		while(tmp != NULL){
			if(tmp->left == l && tmp->right == r){
				if(prev == NULL){
					intv = intv->next;
					//free(tmp);
					break;
				} else {
					prev->next = tmp->next;
					//free(tmp);
					break;
				}
			} else {
				prev = tmp;
				tmp = tmp->next;
			}
		}
	}
	if (intv == NULL) {
		return -1;
	} else {
		tmp_node->left = (intervals *) intv;
		return 0;
	}
}

void updatedIntervals(m_tree_t *tmp) {
	intervals *intv = (intervals *) tmp->left;
	int min = intv->left;
	int max = intv->right;
	intv = intv->next;
	while(intv != NULL){
		if (min < intv->left) min = intv->left;
		if (max > intv->right) max = intv->right;
		intv = intv->next;
	}
	tmp->leftmin = min;
	tmp->rightmax = max;
}

void leafMeasure(m_tree_t *leaf) {
	int l, r;

	if(leaf->leftmin > leaf->min) l = leaf->leftmin;
	else l = leaf->min;

	if(leaf->rightmax < leaf->max) r = leaf->rightmax;
	else r = leaf->max;

	leaf->measure = r - l;
}

void internalMeasure(m_tree_t *node) {
	if(node->right->leftmin < node->min && node->left->rightmax >= node->max)
		node->measure = node->max- node->min;
	if(node->right->leftmin >= node->min&& node->left->rightmax >= node->max)
		node->measure = node->max- node->key + node->left->measure;
	if(node->right->leftmin < node->min&& node->left->rightmax < node->max)
		node->measure = node->right->measure + node->key - node->min;
	if(node->right->leftmin >= node->min&& node->left->rightmax < node->max)
		node->measure = node->right->measure + node->left->measure;
}

void left_rotation(m_tree_t *n) {
	m_tree_t *tmp_node;
	int key = n->key;
	tmp_node = n->left;
	n->left = n->right;
	n->key = n->right->key;
	n->right  = n->left->right;
	n->left->right = n->left->left;
	n->left->left  = tmp_node;
	n->left->key = key;
	n->left->min = n->min;
	n->left->max = n->key;
	n->left->leftmin = min(n->left->left->leftmin,n->left->right->leftmin);
	n->left->rightmax = max(n->left->left->rightmax,n->left->right->rightmax);
	internalMeasure(n->left);
}

void right_rotation(m_tree_t *n){
	m_tree_t *tmp_node;
	int key = n->key;
	tmp_node = n->right;
	n->right = n->left;
	n->key = n->left->key;
	n->left  = n->right->left;
	n->right->left = n->right->right;
	n->right->right  = tmp_node;
	n->right->key = key;
	n->right->min = n->key;
	n->right->max = n->max;
	n->right->leftmin = min(n->right->left->leftmin,n->right->right->leftmin);
	n->right->rightmax = max(n->right->left->rightmax,n->right->right->rightmax);
	internalMeasure(n->right);
}



void insert_node(m_tree_t *tree, int endpoint, int other){
	m_tree_t *tmp_node;
	int finished;
	if( tree->left == NULL ) {
		tree->right = NULL;
		tree->left = NULL;
		tree->key = endpoint;
		tree->measure = 0;
		tree->leftmin = endpoint;
		tree->rightmax = other;
		tree->intv = (intervals *) malloc(sizeof(intervals));
		tree->intv->left = endpoint;
		tree->intv->right = other;
		tree->intv->next = NULL;
		tree->left = (m_tree_t *) tree->intv;
		leafMeasure(tree);
	} else {
		m_tree_t * path_stack[100]; int stack_p = 0;
		tmp_node = tree;
		while(tmp_node->right != NULL) {
			path_stack[stack_p++] = tmp_node;
			if(endpoint < tmp_node->key ){
				tmp_node = tmp_node->left;
			} else{
				tmp_node = tmp_node->right;
			}
		}
		if(tmp_node->key == endpoint) {
			if(endpoint < other) {
				addInterval(tmp_node->intv, endpoint, other);
				tmp_node->leftmin = min(tmp_node->leftmin, endpoint);
				tmp_node->rightmax = max(tmp_node->rightmax, other);
			} else {
				addInterval(tmp_node->intv, other, endpoint);
				tmp_node->leftmin = min(tmp_node->leftmin, other);
				tmp_node->rightmax = max(tmp_node->rightmax, endpoint);
			}
			leafMeasure(tmp_node);
		} else {
			m_tree_t *old_leaf, *new_leaf;
			old_leaf = get_node();
			old_leaf->left = tmp_node->left;
			old_leaf->key = tmp_node->key;
			old_leaf->right  = NULL;
			old_leaf->height = 0;
			old_leaf->leftmin = tmp_node->leftmin;
			old_leaf->rightmax = tmp_node->rightmax;
			old_leaf->intv = tmp_node->intv;

			new_leaf = get_node();

			new_leaf->key = endpoint;
			new_leaf->height = 0;
			new_leaf->intv = (intervals *) malloc(sizeof(intervals));
			if(endpoint < other) {
				new_leaf->intv->left = endpoint;
				new_leaf->intv->right = other;
				new_leaf->intv->next = NULL;
				new_leaf->leftmin = endpoint;
				new_leaf->rightmax = other;
			} else {
				new_leaf->intv->left = other;
				new_leaf->intv->right = endpoint;
				new_leaf->intv->next = NULL;
				new_leaf->leftmin = other;
				new_leaf->rightmax = endpoint;
			}
			new_leaf->left = (m_tree_t *) new_leaf->intv;
			if(tmp_node->key < endpoint) {
				tmp_node->left = old_leaf;
				tmp_node->right = new_leaf;
				tmp_node->key = endpoint;
				old_leaf->min = tmp_node->min;
				old_leaf->max = endpoint;
				new_leaf->min = endpoint;
				new_leaf->max = tmp_node->max;
			} else {
				tmp_node->left = new_leaf;
				tmp_node->right = old_leaf;
				new_leaf->min = tmp_node->min;
				new_leaf->max = tmp_node->key;
				old_leaf->min = tmp_node->key;
				old_leaf->max = tmp_node->max;
			}
			leafMeasure(old_leaf);
			leafMeasure(new_leaf);
			tmp_node->leftmin = min(tmp_node->left->leftmin,tmp_node->right->leftmin);
			tmp_node->rightmax = max(tmp_node->left->rightmax,tmp_node->right->rightmax);
			path_stack[stack_p++] = tmp_node;
		}
		finished = 0;
		int stack_tmp = stack_p;
		while(stack_tmp > 0) {
			tmp_node = path_stack[--stack_tmp];
			internalMeasure(tmp_node);
			tmp_node->leftmin = min(tmp_node->left->leftmin,tmp_node->right->leftmin);
			tmp_node->rightmax = max(tmp_node->left->rightmax,tmp_node->right->rightmax);
		}
		while( stack_p > 0 && !finished ){
			int tmp_height, old_height;
			tmp_node = path_stack[--stack_p];
			old_height= tmp_node->height;
			if( tmp_node->left->height - tmp_node->right->height == 2 ) {
				if( tmp_node->left->left->height - tmp_node->right->height == 1 ) {
					right_rotation( tmp_node );
					tmp_node->right->height = tmp_node->right->left->height + 1;
					tmp_node->height = tmp_node->right->height + 1;
				} else {
					left_rotation( tmp_node->left );
					right_rotation( tmp_node );
					tmp_height = tmp_node->left->left->height;
					tmp_node->left->height  = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			} else if ( tmp_node->left->height - tmp_node->right->height == -2 ) {
				if( tmp_node->right->right->height - tmp_node->left->height == 1 ) {
					left_rotation( tmp_node );
					tmp_node->left->height = tmp_node->left->right->height + 1;
					tmp_node->height = tmp_node->left->height + 1;
				} else {
					right_rotation( tmp_node->right );
					left_rotation( tmp_node );
					tmp_height = tmp_node->right->right->height;
					tmp_node->left->height  = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			} else {/* update height even if there was no rotation */
				if( tmp_node->left->height > tmp_node->right->height )
					tmp_node->height = tmp_node->left->height + 1;
				else
					tmp_node->height = tmp_node->right->height + 1;
			}
			if( tmp_node->height == old_height )
				finished = 1;
		}
	}
}

void delete_node(m_tree_t *tree, int endpoint, int other){
	m_tree_t *tmp_node, *upper_node, *other_node;
	int finished;
	if(tree->right == NULL){
		if(tree->key == endpoint) {
			free(tree->left);
			tree->left = NULL;
			tree->right  = NULL;
			tree->key  = 0;
			tree->height = 0;
			tree->leftmin = 0;
			tree->rightmax = 0;
			tree->min = -INFINITY;
			tree->max = INFINITY;
			tree->measure = 0;
		}
		return;
	}
	m_tree_t * path_stack[100]; int stack_p = 0;
	tmp_node = tree;
	while( tmp_node->right != NULL ) {
		path_stack[stack_p++] = tmp_node;
		upper_node = tmp_node;
		if(endpoint < tmp_node->key){
			tmp_node   = upper_node->left;
			other_node = upper_node->right;
		} else {
			tmp_node   = upper_node->right;
			other_node = upper_node->left;
		}
	}
	if(tmp_node->key != endpoint) return;
	int flag = deleteInterval(tmp_node, endpoint, other);
	if(flag == -1) {
		upper_node->key   = other_node->key;
		upper_node->left  = other_node->left;
		upper_node->right = other_node->right;
		upper_node->height = other_node->height;
		upper_node->measure = other_node->measure;
		upper_node->leftmin = other_node->leftmin;
		upper_node->rightmax = other_node->rightmax;
		if(upper_node->right != NULL){
			upper_node->right->max = upper_node->max;
			upper_node->left->min = upper_node->min;
		}
		free(tmp_node);
		other_node->left = NULL;
		other_node->right = NULL;
		free(other_node);
	} else {
		updatedIntervals(tmp_node);
		leafMeasure(tmp_node);
	}
	int stack_tmp = stack_p;
	while(stack_tmp > 0) {
		tmp_node = path_stack[--stack_tmp];
		if (tmp_node->right != NULL) {
			internalMeasure(tmp_node);
			tmp_node->leftmin = min(tmp_node->left->leftmin,tmp_node->right->leftmin);
			tmp_node->rightmax = max(tmp_node->left->rightmax,tmp_node->right->rightmax);
		}
	}
	/*start rebalance*/
	finished = 0; stack_p -= 1;
	while( stack_p > 0 && !finished ){
		int tmp_height, old_height;
		tmp_node = path_stack[--stack_p];
		old_height= tmp_node->height;
		if( tmp_node->left->height - tmp_node->right->height == 2 ) {
			if( tmp_node->left->left->height - tmp_node->right->height == 1 ) {
				right_rotation( tmp_node );
				tmp_node->right->height = tmp_node->right->left->height + 1;
				tmp_node->height = tmp_node->right->height + 1;
			} else {
				left_rotation( tmp_node->left );
				right_rotation( tmp_node );
				tmp_height = tmp_node->left->left->height;
				tmp_node->left->height  = tmp_height + 1;
				tmp_node->right->height = tmp_height + 1;
				tmp_node->height = tmp_height + 2;
			}
		} else if ( tmp_node->left->height - tmp_node->right->height == -2 ) {
			if( tmp_node->right->right->height - tmp_node->left->height == 1 ) {
				left_rotation( tmp_node );
				tmp_node->left->height = tmp_node->left->right->height + 1;
				tmp_node->height = tmp_node->left->height + 1;
			} else {
				right_rotation( tmp_node->right );
				left_rotation( tmp_node );
				tmp_height = tmp_node->right->right->height;
				tmp_node->left->height  = tmp_height + 1;
				tmp_node->right->height = tmp_height + 1;
				tmp_node->height = tmp_height + 2;
			}
		} else {/* update height even if there was no rotation */
			if( tmp_node->left->height > tmp_node->right->height )
				tmp_node->height = tmp_node->left->height + 1;
			else
				tmp_node->height = tmp_node->right->height + 1;
		}
		if( tmp_node->height == old_height )
			finished = 1;
	}
}

void insert_interval(m_tree_t *tree, int a, int b){
	if(a < b){
		insert_node(tree, a, b);
		insert_node(tree, b, a);
	}
}

void delete_interval(m_tree_t *tree, int a, int b){
	if(tree->left != NULL && a < b) {
		delete_node(tree, a, b);
		delete_node(tree, b, a);
	}
}

int query_length(m_tree_t *tree){
	if(tree != NULL) return tree->measure;
	else return -1;
}

int main() {
	m_tree_t *tree_ = create_m_tree();
	//	insert_interval(tree_, 1, 0);
	//	//ASSERT_EQ(query_length(tree_), 0);
	//
	//	insert_interval(tree_, 1, 2);
	//	//ASSERT_EQ(query_length(tree_), 1);
	//
	//	insert_interval(tree_, 2, 4);
	//	//ASSERT_EQ(query_length(tree_), 3);
	//
	//	insert_interval(tree_, 6, 10);
	//	//ASSERT_EQ(query_length(tree_), 7);
	//
	//	insert_interval(tree_, 7, 8);
	//	//ASSERT_EQ(query_length(tree_), 7);
	//
	//	insert_interval(tree_, 7, 11);
	//	//ASSERT_EQ(query_length(tree_), 8);
	//
	//	insert_interval(tree_, -1, 1);
	//	//ASSERT_EQ(query_length(tree_), 10);
	//
	//	insert_interval(tree_, -5, -3);
	//	//ASSERT_EQ(query_length(tree_), 12);
	//
	//	insert_interval(tree_, -6, -4);
	//	//ASSERT_EQ(query_length(tree_), 13);
	//
	//	insert_interval(tree_, -7, 11);
	//	//ASSERT_EQ(query_length(tree_), 18);

	delete_interval(tree_, 1, 2);
	//ASSERT_EQ(query_length(tree_), 0);

	insert_interval(tree_, 1, 2);
	delete_interval(tree_, 1, 3);
	//ASSERT_EQ(query_length(tree_), 1);
	delete_interval(tree_, 1, 2);
	//ASSERT_EQ(query_length(tree_), 0);

	insert_interval(tree_, 1, 2);
	insert_interval(tree_, 2, 4);
	delete_interval(tree_, 1, 2);
	//ASSERT_EQ(query_length(tree_), 2);

	insert_interval(tree_, 5, 9);
	delete_interval(tree_, 2, 4);
	//ASSERT_EQ(query_length(tree_), 4);

	insert_interval(tree_, 6, 8);
	delete_interval(tree_, 5, 9);
	//ASSERT_EQ(query_length(tree_), 2);

	insert_interval(tree_, 7, 10);
	delete_interval(tree_, 6, 8);
	//ASSERT_EQ(query_length(tree_), 3);

	insert_interval(tree_, 6, 8);
	delete_interval(tree_, 7, 10);
	//ASSERT_EQ(query_length(tree_), 2);

	insert_interval(tree_, 2, 10);
	delete_interval(tree_, 6, 8);
	//ASSERT_EQ(query_length(tree_), 8);

	insert_interval(tree_, -1, 2);
	delete_interval(tree_, 2, 10);
	//ASSERT_EQ(query_length(tree_), 3);

	delete_interval(tree_, -1, 2);
	//ASSERT_EQ(query_length(tree_), 0);
	int x = query_length(tree_);
	int y = 0;
}
