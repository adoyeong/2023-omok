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
	int i, j, cnt = 0;
	int result = 1;
	for(i=0; i<TAB_MAX; i++)
	{
		for(j=0; j<TAB_MAX;j++)
		{
			if(tab[i][j] != 0) cnt++;
			if(tab[i][j] == 1)
			{
				result *= line(i, j, 0, 1);
				result *= line(i, j, 1, 1);
				result *= line(i, j, 2, 1);
				result *= line(i, j, 3, 1);
				if(result == 0) return who;
			}
		}
	}
	if(cnt == TAB_MAX*TAB_MAX) return -1;
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
	FILE *file_w = NULL;
        if(winner != 0)
	{
		file_w = fopen("./data/win.txt", "r");
		for(i=0; i<81; i++)
		{
			fscanf(file_n, "%u", &num[i]);
			fscanf(file_w, "%u", &win[i]);
			if(num[i] > 100000)
			{
				num[i] = num[i] / 100;
				win[i] = (win[i] / 100);
			}
		}
	}
	else for(i=0; i<81; i++) fscanf(file_n, "%u", &num[i]);

	fclose(file_n);
	if(winner != 0) fclose(file_w);
	file_n = fopen("./data/num.txt", "w");
	if(winner != 0) file_w = fopen("./data/win.txt", "w");
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
				num[state[j]] += cnt;
			}
		}
		if(winner == 0) continue;
		if((i % 2) == winner - 1)
		{
			for(j=0; j<8; j++)
			{
				if(state[j] == -1) continue;
				else if(state[j] <0 || state[j] > 80)
				{
					fprintf(stdout, "Invalid state\n");
					continue;
				}
				else win[state[j]] += i;
			}
		}
	}
	if(winner != 0)
	{
		for(i=0; i<81; i++)
		{
			fprintf(file_n, "%u ", num[i]);
			fprintf(file_w, "%u ", win[i]);
		}
	}
	else for(i=0; i<81; i++) fprintf(file_n, "%u ", num[i]);
	fclose(file_n);
	if(winner != 0) fclose(file_w);
	fclose(file_l);
	return;
}
void log_reset(int opt)
{
	FILE *file = NULL;
	file = fopen("./data/log.txt", "w");
	if(file == NULL)
	{
		perror("file open failed(log.txt)");
	}
	else fclose(file);
	if(opt == 1)
	{
		FILE *file_tl = fopen("./data/table_log.txt", "w");
		if(file_tl == NULL)
		{
			perror("file open failed(table_log.txt)");
		}
		else fclose(file);
	}
	return;
}
int battle_c(int opt)
{
	int result = 0;
	int turn = 2;
	int ref = 0;
	int cnt = 0;
	int row, col;
	memset(tab, 0, sizeof(tab));
	log_reset(0);
	while(ref == 0)
	{
		clear_tab();
		turn = (turn ^ 0x03);
		cnt++;
		result = play(tab);
		col = (result & 0x0ff);
		row = (result >> 8);
		if(col < 0 || col > TAB_MAX-1 || row < 0 || row > TAB_MAX-1)
		{
			fprintf(stdout, "play function error : col or row is invalid value\n");
			fprintf(stdout, "%x -> %x, %x\n", result, row, col);
			return 1;
		}
		tab[row][col] = 1;
		if(opt==1) table_log(turn);
		ref = referee(turn);
	}
	if(ref == -1) ref = 0;
	update(cnt, ref);
	return ref;

}
int battle_p(int opt, int t)
{
	int result = 0;
	if(t != 1 && t != 2) return 0;
        int turn = 2;
        int ref = 0;
        int cnt = 0;
        int row, col, i, j;
        memset(tab, 0, sizeof(tab));
        log_reset(0);
        while(ref == 0)
        {
		clear_tab();
		turn = (turn ^ 0x03);
		cnt++;
		fprintf(stdout, "################<%d>##############\n", cnt);
		if(turn == t)
		{
			fprintf(stdout, "\n    ");
			for(i=0; i<TAB_MAX; i++) fprintf(stdout, "[%2d] ", i);
			for(i=0; i<TAB_MAX; i++)
			{
				fprintf(stdout, "\n[%2d] ", i);
				for(j=0; j<TAB_MAX; j++)
				{
					if(tab[i][j] == 0) fprintf(stdout, ".    ");
					else fprintf(stdout, "%d    ", tab[i][j]);
				}
			}
			if(inv == 1) fprintf(stdout, "danger : %lf",mem_val);
			else fprintf(stdout, "opportunity : %lf", mem_val);
			fprintf(stdout, "\n\nyour turn : ");
			fscanf(stdin, "%d %d", &row, &col);
			while(tab[row][col] != 0)
			{
				fprintf(stdout, "Invalid!\nyour turn : ");
				fscanf(stdin, "%d %d", &row, &col);
			}
		}
		else
		{
	        	result = play(tab);
	        	col = (result & 0x0ff);
        		row = (result >> 8);
		}
	        if(col < 0 || col > TAB_MAX-1 || row < 0 || row > TAB_MAX-1)
       		{
        		fprintf(stdout, "play function error : col or row is invalid value\n");
        		fprintf(stdout, "%x -> %x, %x\n", result, row, col);
        		return 1;
        	}
        	tab[row][col] = 1;
        	if(opt==1) table_log(turn);
		ref = referee(turn);
        }
        if(ref== -1) ref = 0;
	return ref;

}
int omok_learn(int times)
{
	log_reset(1);
	int learn_num;
	if(times < 100000 && times > 0) learn_num = times;
	else learn_num = 1000;
	int i;
	for(i=0; i<learn_num; i++) (void)battle_c(0);
	fprintf(stdout, "COMPLETE : learn %d times\n", learn_num);
	return 0;
}
