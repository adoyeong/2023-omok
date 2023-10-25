#include <stdio.h>
#include "model.h"
int main()
{
	char first = 0;
	int i;
	fprintf(stdout, "First Time? [y/n] : ");
	fscanf(stdin, "%c", &first);
	if(first == 'y')
	{
		FILE *file = fopen("./data/win.txt", "w");
		if(file == NULL)
		{
			perror("Cannot make/open file");
			return 1;
		}
		for(i=0; i<81; i++) fprintf(file, "0 ");
		fclose(file);
		file = fopen("./data/num.txt", "w");
		if(file == NULL)
		{
			perror("Cannot make/open file");
			return 1;
		}
		for(i = 0; i<81; i++) fprintf(file, "0 ");
		fclose(file);
	}
	char tab[TAB_MAX][TAB_MAX] = {0, };
	int result = play(tab);
	printf("result : %d\n", result);
	return 0;
}
