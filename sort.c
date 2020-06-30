#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

void sort(struct topleaf topleafs[N])
{
	int i, j;
	int freq;
	char word[32];
	

	
	for(i=0; i<N-1; i++)//sorting algorithm
	{
		for(j=0; j < N -1 -i; j++)
		{
			if(topleafs[j].frequency == -1)
				continue;
			if(strcmp(topleafs[j].word, topleafs[j+1].word) < 0)
			{
				strcpy(word, topleafs[j+1].word);
				freq = topleafs[j+1].frequency;
				
				strcpy(topleafs[j+1].word, topleafs[j].word);
				topleafs[j+1].frequency = topleafs[j].frequency;
				
				strcpy(topleafs[j].word, word);
				topleafs[j].frequency = freq;
				
			}
		}
	}
}

