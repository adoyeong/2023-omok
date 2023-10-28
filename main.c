#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "model.h"
#include "omok_learn.h"
int main(int argc, char*argv[])
{
	int i, num, how_many = 0;
	int a = 1;

	FILE *file_w = fopen("./data/win.txt", "r");
	FILE *file_n = fopen("./data/num.txt", "r");
	int w, n;
	double wd, nd, result;
	for(i=0; i<81; i++)
	{
		num = i;
		fprintf(stdout, "%d", num/27);
		num = num % 27;
		fprintf(stdout, "%d", num/9);
		num = num % 9;
		fprintf(stdout, "%d", num/3);
		num = num % 3;
		fprintf(stdout, "%d : ", num);
		fscanf(file_w, "%u", &w);
		fscanf(file_n, "%u", &n);
		wd = (double)w;
		nd = (double)n;
		result = wd / nd;
		fprintf(stdout, "%lf\n", result);
	}
	fclose(file_w);
	fclose(file_n);

	if(argc > 3)
	{
		fprintf(stdout, "[syntax error]\n");
		fprintf(stdout, "syntax : omok <num of learn> <is it first time(0 or 1)>\n");
		fprintf(stdout, "sample : omok 100 0\n");
		return 1;
	}
	if(argc >= 2 && 0 < atoi(argv[1])) how_many = atoi(argv[1]);
	if(argc == 3 && 1 == atoi(argv[2]))
	{
		FILE *file = fopen("./data/win.txt", "w");
		if(file == NULL)
		{
			perror("Cannot make file");
			return 1;
		}
		for(i=0; i<81; i++) fprintf(file, "1 ");
		fclose(file);
		file = fopen("./data/num.txt", "w");
		if(file == NULL)
		{
			perror("Cannot make file");
			return 1;
		}
		for(i=0; i<81; i++) fprintf(file, "1 ");
		fclose(file);
	}
	for(i = 0; i<how_many; i++)
	{
		fprintf(stdout, "[%d/%d]\n", i, how_many);
		a = omok_learn(0);
		if(a != 0) fprintf(stdout, "Error : omok_learn()\n");
	}
	int winner = battle_c(1);
	fprintf(stdout, "\n\nWinner is %d\n", winner);
	winner = battle_p(0, 1);
	fprintf(stdout, "\n\nWinner is %d\n", winner);
	winner = battle_p(0, 2);
	return 0;
}
