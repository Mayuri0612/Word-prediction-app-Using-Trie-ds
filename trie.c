#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include "trie.h"
#include "sort.h"

struct node* new_node(struct node *h)
{
	int i;
	h = malloc(sizeof (struct node));
	for (i = 0; i < 24; ++i)
		h->array[i] = 0;
	return h;
}

void update_top_leafs(struct node *h, char *s)
{
	int i, j, z, min, num_leafs, empty, cont=0;
	struct topleaf sorted_topleafs[N];
	struct leaf *l = h->fyllo;
	struct node *p = h;
	
	l->frequency++;
	
	for(i=0; i<strlen(s); i++)
	{	
		z=0;
		cont =0;
		num_leafs = p->num_leafs;
		
		for(j=0; j<N; j++)//if the array is full but one of the words is the one we test just increase frequency to be updated
		{
			if(strcmp(p->topleafs[j].word, s) == 0)
			{
				p->topleafs[j].frequency++;
				z=1;
			}
		}
		
		
		if(z != 1)
		{
			for(j=0; j<N; j++)//if array of top frequent words has space insert the word
			{
				if(p->topleafs[j].frequency == -1)
				{
					empty = j;
					cont = 1;
					break;
				}
			}
		}
		
		if(cont==1)
		{	
			p->topleafs[empty].frequency = l->frequency;
			strcpy(p->topleafs[empty].word, s);
		}
		
		if(cont == 0 && z!=1)
		{
			if(z!=1)//if one of those words in the array has lower frequency than the one we test change them
			{	
				min = 0;
				for(j=1; j<N; j++)//find the word with the lowest frequency of the array
				{	
					//printf("%d %s min -> %d %s\n", p->topleafs[j].frequency,p->topleafs[j].word, p->topleafs[min].frequency, p->topleafs[min].word);
					if(p->topleafs[j].frequency < p->topleafs[min].frequency)
						min = j;
				}
				
				if(l->frequency >= p->topleafs[min].frequency )
				{//if the word we test has greater or equal frequency change them 
					p->topleafs[min].frequency = l->frequency;
					strcpy(p->topleafs[min].word, s);
				}
				else//no need to climb to parent
					break;
			}
		}
		
		sort(p->topleafs);//here we sort the top N leafs after all modifications and changes and we proceed to parent
		
		p = p->parent;
	}
}


struct node* insert(struct node *h, char *c, int value)
{
	struct leaf *l;
	
	int i, j;
	
	l=NULL;
	
	if (strlen(c) == 0)
		return h;
	if (h == NULL)
		h = new_node(h);
	struct node *p = h;

	for (i = 0; i < strlen(c); ++i)
    {
		
		if (p->array[c[i] - 'a'] == NULL) 
		{
			p->array[c[i] - 'a'] = malloc(sizeof (struct node));
			p->array[c[i] -'a']->letter = c[i];
			
			p->array[c[i] - 'a']->parent = p; //assigning parents to internal nodes
			
			for(j=0; j<N; j++)
				p->array[c[i] - 'a']->topleafs[j].frequency = -1;
		}
		p = p->array[c[i] - 'a'];
	}
	
	p->num_leafs = 0;
	
	l = malloc(sizeof( struct leaf));//allocate space for leaf
	l->frequency = -1;//reset frequency
	l->parent = p;//connect leaf with parent
	p->fyllo = l;//connect parent with leaf
	
	
	update_top_leafs(p, c);
	
	for(i=0; i<strlen(c); i++)//incrementing how many leafs has every internal node across  the path of the new word
	{
		p->num_leafs++;
		p = p->parent;
	}
	
	return h;
}


void search(struct node *h, char *s, char *dat, char *ac_word, int *found, int *cl, int *stop)
{
	int i, j, choice, rem_chars=0, legal_choice=0, limit=-1;
	char l[100] = {0}, action[10];
	char a[2], c, word[32];
	
	
	*found = -1;
	
	strcpy(l, dat);
	if (strlen(s) > 0) {
		a[0] = s[0];
		a[1] = '\0';
		if(h->array[a[0]-'a'] != NULL) //if word has not finished proceed to the next character in the trie
		{
			strcat(dat, a);
			search(h->array[a[0]-'a'], s+1, dat, ac_word, &(*found), &(*cl), &(*stop));
		} 
		else
		{
			if(*stop != 1)
				printf("No Match\n");
			strcat(dat, s);
			*found  = 0;//the word we are looking for doesnt exists
		}
	}
	else 
	{
			if(*stop == 1)
			{
				*stop = 2;
				return;
			}
			fprintf(stderr, "Match ");
			
			for(i=0; i<N; i++)
			{
				if(h->topleafs[i].frequency != -1)//print choices to autocomplete
				{
					
					fprintf(stderr, "%s %d ", h->topleafs[i].word, h->topleafs[i].frequency);
				}
				else
				{
					limit = i;//if found empty slot save it for choice of autocomplete word and exit loop
					break;
				}
			}
			
			printf("\n\n");
			
			if(limit == -1)//if var limit has value -1 then there isnt any empty slot so the available choices/options are N words
				limit = N;
			
			while(legal_choice == 0)
			{
				scanf("%s", action);
				
				for(i=0; i<strlen(action); i++)
				{
					if(!isdigit(action[i]))
					{//The choice that we entered isnt numerical so ignore all options/words and continue
						*cl = 1;
						*found = 2;
						break;
					}
				}
				
				if(*found != 2)//action is number so convert it from arithmetical to integer
					choice = atoi(action);
				else
					break;
				
				if(choice > 0 && choice <= limit)//if choice is accurate
				{
					legal_choice = 1;
					*cl = 1;
					choice--;
					
					
					rem_chars = strlen(h->topleafs[choice].word) - strlen(dat);
					strcpy(word, h->topleafs[choice].word);
				
					for(j=0; j<rem_chars; j++)//tree cursor is at the end of the prefix so jump "suffix" times to get last letter node
					{
						c = word[strlen(dat) + j];
						h = h->array[ c - 'a'];
					}
					update_top_leafs(h, word);
					strcpy(ac_word, word);
					*found = 1; //the word we are looking for exists
				}
			}
	}
}


void destroy_trie(struct node *h)//free memory from internal and leaf nodes
{
	int c;

	if ( h == NULL ) {
		return;
	}

	for( c = 0; c < 26; ++c ) 
	{
		if(h->array[c] ==NULL)
			continue;
		destroy_trie( h->array[c] );
	}

	if(h->fyllo != NULL)
		free(h->fyllo);
	free(h);	
	
}
