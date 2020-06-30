#include <stdio.h>
#include <stdlib.h>

#define N 5

struct leaf
{
	int frequency;
	struct node *parent;
};

struct topleaf
{
	int frequency;
	char word[32];
};

struct node 
{
	int num_leafs;
	char letter;
	struct node *array[26];
	struct node *parent;
	struct leaf *fyllo;
	struct topleaf topleafs[N];
};


struct node* new_node(struct node *);

void update_top_leafs(struct node *, char *);

struct node* new_node(struct node *);

struct node* insert(struct node *, char *, int );

void search(struct node *, char *, char *, char *, int *, int *, int *);

void destroy_trie(struct node *);
