#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include "trie.h"
#include "sort.h"


#define LINESIZE 128
#define BACKSPACE 0x7f
#define MAX 500

struct node* read_keys(struct node *h, char *file)//read all lines from file and insert them into trie
{
	char s[MAX];
	FILE *a = fopen(file, "r");
	if (a == NULL)
		printf("Error while opening file");
	else 
	{
		fscanf(a, "%s", s);//to avoid inserting the last word twice
		while (feof(a) == 0) {
			h = insert(h, s, 1);
			fscanf(a, "%s", s);
		}
	}
	fclose(a);
	return h;
}



int getchar_silent()
{
    int ch;
    struct termios oldt, newt;

    /* Retrieve old terminal settings */
    tcgetattr(STDIN_FILENO, &oldt);

    /* Disable canonical input mode, and input character echoing. */
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );

    /* Set new terminal settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    /* Read next character, and then switch to old terminal settings. */
    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}


int main (int argv, const char *args[])
{
    int next, i=0, j, z=0, k, rem_chars=0, found_path=0, found=-1, cl=0;
    int r=0, stop=0, tab=0;
	FILE *fp;
    struct winsize w;
    char s[MAX], sentence[2000], ss[MAX], prev;
    char dict_path[500], word[32], new_words[2000][32];
    char *prefix;
    struct node *h = 0, *temp;
    struct leaf *l;
    
    
    for(k=0; k<argv; k++)//searching dictionary path
    {	
		if(strcmp(args[k], "-d")==0)//it needs full path e.x. /home/john/Dekstop/Syspro/Project1/dict
		{
			strcpy(dict_path, args[k+1]);
			found_path = 1;
		}
	}
    
    if(found_path != 1)
		strcpy(dict_path,"/home/john/Desktop/dict");//default path for dict is at Desktop
		
	
	h = read_keys(h, dict_path);
	
	//sort(h->array[0]->array[25]->array[20]->topleafs);
	

    /* Keep reading one character at a time */
    while ((next = getchar_silent()) != EOF)
    {		
		if(i>0)
			prev = sentence[i-1];
			
		rem_chars = 0;
		
        /* Print normal characters */
        if (isalpha(next) || ispunct(next) || next == ' ' || next == '\n')
        {
			if(cl != 1)//to avoid a \n after we select with enter the choice of the word we want to autocomplete
				putchar(next);
			
            if(isalpha(next) || ispunct(next) || next == ' ')
            {
				sentence[z] = next;//build up the entire sentence
				z++;
			}
           
			if(isalpha(next))
			{		
				s[i]=tolower(next);//avoid uppercase letters
				i++;
			}
			else if(next == ' ')
			{	
				
				if(tab != 1 || prev != ' ')
				{
					char dat[100] = "";
					stop = 1;
					
					s[i] = '\0';//end of word that we will search
					
					search(h, s, dat, word, &found, &cl, &stop);//here search function works differently (simplier), 
					//it's only searches if the word is in tree and returns a special value in variable stop
					
					if(stop == 1)//after space we check if the previous word does not belongs to the tree we insert it
					{
						h = insert(h, s, 1);
						strcpy(new_words[r], s);
						r++;
						
					}
					else if (stop == 2)
					{
						temp = h;
						for(j=0; j<strlen(s); j++)
							temp = temp->array[s[j] - 'a'];
						if(temp->fyllo == NULL)
						{
							temp->num_leafs = 0;
							l = malloc(sizeof( struct leaf));//allocate space for leaf
							l->frequency = -1;//reset frequency
							l->parent = temp;//connect leaf with parent
							temp->fyllo = l;//connect parent with leaf
						}
						update_top_leafs(temp, s);
					}
				}
				tab = 0;
			
				for(j=0; j<MAX; j++)//re-initialization of strings for the next word
					s[j] = 0;
				for(j=0; j<32; j++)
					word[j] = 0;
				i=0;
				stop = 0;
			
			}
			else if(next == '\n' && cl != 1)
			{
				for(j=0; j<2000; j++)
					sentence[j] = 0;
				for(j=0; j<MAX; j++)
					s[j] = 0;
				for(j=0; j<32; j++)
					word[j] = 0;
				i=0;
				z=0;
			}
			
			cl = 0;
			
            continue;
        }

        /* Treat special characters differently */
        switch(next) {

        case '\t':              /* Just read a tab */
			tab = 1;
			
            /* Get terminal window size */
            ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

            /* Fill next line with asterisks */
            putchar('\n');
            for (int i = 0; i < w.ws_col; i++)
                putchar('*');
            putchar('\n');
            
			char dat[100] = "";
			s[i] = '\0';
			
			search(h, s, dat, word, &found, &cl, &stop);
			
			printf("\n");
			if(found == 1)//if found in order to appear the whole sentence complete
			{//take the prefix and the suffix of the word subtract them, and fill the remaining characters to sentence
				rem_chars = strlen(word) - strlen(s);
				for(j=0; j<rem_chars; j++)
				{
					sentence[z] = word[strlen(s)+j];
					z++;
				}
				found = 0;
				fprintf(stderr,"%s", sentence);
			}
			else if(found == 0)//if it does not exists insert this word to trie
			{
				h = insert(h, s, 1);
				strcpy(new_words[r], s);
				r++;
				fprintf(stderr,"%s", sentence);
			}
			else if(found == 2)//if  user typed illegal characters just print the sentence 
			    fprintf(stderr, "%s", sentence);
	
			if(found != 2)
			{
				for(j=0; j<MAX; j++)
					s[j] = 0;
				for(j=0; j<32; j++)
					word[j] = 0;
				i=0;
			}
			
            break;

        case BACKSPACE:         /* Just read a backspace */

            /* Move the cursor one step back, print space, and move
             * cursor back once more to imitate backward character
             * deletion.
             */
			
			z--;
			i--;
			
			if(isalpha(sentence[i]))
				s[i]=0;
			sentence[z] = 0;
			
            printf("\b \b");
            break;

        case VEOF:              /* Just read EOF (Ctrl-D) */

            /* The EOF character is recognized only in canonical input
             * mode. In noncanonical input mode, all input control
             * characters such as Ctrl-D are passed to the application
             * program exactly as typed. Thus, Ctrl-D now produces
             * this character instead.
             */

            printf("\n\nExiting. Bye...");
            goto THE_END;
        default:
            continue;
        }
    }
THE_END:
    putchar('\n');
    
    fp = fopen(dict_path,"a+");
    
    for(i=0; i<r; i++)//appending new words that dont exist in dict 
		fprintf(fp, "%s\n", new_words[i]);
		
	fclose(fp);
    
    destroy_trie(h);
    
    return EXIT_SUCCESS;
}

