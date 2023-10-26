#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "model.h"
char tab[TAB_MAX][TAB_MAX] = {0, };
int line(int row, int col, int v, int who)
{
	int r, c;
	int result = 0;
	if(v == 0)
	{
		r = 1;
		c = 0;
	}
	else if(v == 1)
	{
		r = 0;
		c = 1;
	}	
	else if(v == 2)
	{
		r = 1;
		c = 1;
	}
	else if(v == 3)
	{
		r = 1;
		c = -1;
	}
	else
	{
		fprintf(stdout, "line() : v is undefined value ( %d )\n", v);
		return 1;
	}
	while(row < TAB_MAX && col < TAB_MAX && col >= 0 && tab[row][col] == who)
	{
		result++;
		row +=r;
		col +=c;
	}
	if(result >= 5) return 0;
	return 1;
}
int referee(int who)
{
	int i, j;
	int result = 1;
	for(i=0; i<TAB_MAX; i++)
	{
		for(j=0; j<TAB_MAX;j++)
		{
			if(tab[i][j] == who)
			{
				result *= line(i, j, 0, who);
				result *= line(i, j, 1, who);
				result *= line(i, j, 2, who);
				result *= line(i, j, 3, who);
				if(result == 0) return 1;
			}
		}
	}
	return 0;
}
void clear_tab()
{
	int i, j;
	for(i=0; i<TAB_MAX; i++)
	{
		for(j=0; j<TAB_MAX; j++)
		{
			if(tab[i][j] == 1) tab[i][j] = 2;
			else if(tab[i][j] == 2) tab[i][j] = 1;
		}
	}
	return;
}
void table_log(int turn)
{
	int i, j;
	if(turn == 2) clear_tab();
	FILE *file = fopen("./data/table_log.txt", "a");
	for(i=0; i<TAB_MAX; i++)
	{
		for(j=0; j<TAB_MAX; j++)
		{
			if(tab[i][j] == 0)
			{
				fprintf(file, ". ");
				continue;
			}
			fprintf(file, "%d ", tab[i][j]);
		}
		fprintf(file, "\n");
	}
	fprintf(file, "\n");
	fclose(file);
	if(turn == 2) clear_tab();
	return;
}
void update(int cnt, int winner)
{
	//printf("####### winner : %d\n", winner);
	unsigned int num[81] = {0, }, win[81] = {0, };
	int i, j;
	int row, col, state[8];
	double prob; 
	FILE *file_l = fopen("./data/log.txt", "r");
        FILE *file_n = fopen("./data/num.txt", "r");
        FILE *file_w = fopen("./data/win.txt", "r");
	for(i=0; i<81; i++)
	{
		fscanf(file_n, "%u", &num[i]);
		fscanf(file_w, "%u", &win[i]);
	}
	fclose(file_n);
	fclose(file_w);
	file_n = fopen("./data/num.txt", "w");
	file_w = fopen("./data/win.txt", "w");
	for(i=0; i<cnt; i++)
	{
		fscanf(file_l, "%d %d %lf", &row, &col, &prob);
		for(j=0; j<8; j++) fscanf(file_l, "%d", &state[j]);
		for(j=0; j<7; j++)
		{
			for(int k=j+1; k<8; k++)
			{
				if(state[j] == state[k]) state[k] = -1;
			}
		}
		for(j=0; j<8; j++)
		{
			if((state[j] & 0x00ff00) != 0) state[j] = -1;
			else
			{
				num[state[j]] += 1;
			}
		}
		if((i % 2) == 0 && winner == 1)
		{
			for(j=0; j<8; j++)
			{
				if(state[j] == -1) continue;
				else win[state[j]] += 1;
			}
		}
		else if((i % 2) == 1 && winner == 2)
		{
			for(j=0; j<8; j++)
			{
				if(state[j] == -1) continue;
				else win[state[j]] += 1;
			}
		}
	}
	for(i=0; i<81; i++)
	{
		fprintf(file_n, "%u ", num[i]);
		fprintf(file_w, "%u ", win[i]);
	}
	fclose(file_n);
	fclose(file_w);
	fclose(file_l);
	return;
}
int main(int argc, char *argv[])
{
	int learn_num;
	if(argc == 2 && 0 < atoi(argv[1]) && atoi(argv[1]) < 100000) learn_num = atoi(argv[1]);
	else learn_num = 1000;
	FILE * file = fopen("./data/log.txt", "w");
	fclose(file);
	file = fopen("./data/table_log.txt", "w");
	fclose(file);
	char first = 0;
	int i, result, row, col;
	int turn = 1, cnt = 0;
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
		for(i=0; i<81; i++) fprintf(file, "1 ");
		fclose(file);
		file = fopen("./data/num.txt", "w");
		if(file == NULL)
		{
			perror("Cannot make/open file");
			return 1;
		}
		for(i = 0; i<81; i++) fprintf(file, "1 ");
		fclose(file);
	}
	for(i=0; i<learn_num; i++)
	{
		if(i%100 == 0) fprintf(stdout, "%d\n", i);
		while(referee(1) == 0 && referee(2) == 0)
		{
			cnt++;
			result = play(tab);
			col = (result & 0x0f);
			row = (result >> 8);
			tab[row][col] = 1;
			//table_log(turn);
			clear_tab();
			turn = (turn ^ 0x03);
		}
		//printf("COMPLETE!\n");
		update(cnt, (turn & 0x03));
		//printf("%d player win!! (%d times)\n", (turn & 0x03), cnt);
		turn = 1;
		cnt = 0;
		memset(tab, 0, sizeof(tab));
	}
	while(referee(1) == 0 && referee(2) == 0)
        {
		cnt++;
		result = play(tab);
		col = (result & 0x0f);
		row = (result >> 8);
		tab[row][col] = 1;
		table_log(turn);
		clear_tab();
		turn = (turn ^ 0x03);
	}
	printf("COMPLETE!\n");
	update(cnt, (turn ^ 0x03));
	printf("%d player win!! (%d times)\n", (turn ^ 0x03), cnt);
	return 0;
}
