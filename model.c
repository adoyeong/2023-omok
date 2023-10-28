#include <stdio.h>
#include <string.h>
#include "model.h"
#define TAB_MAX 15
char board[TAB_MAX][TAB_MAX];
double prob[TAB_MAX][TAB_MAX];
double prob2[TAB_MAX][TAB_MAX];
int t_boardle_row[8] = {1, 1, 1, 0, 0, -1, -1, -1};
int t_boardle_col[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
int inverse[81] = {0, 2, 1, 6,  8, 7, 3, 5, 4,
	18, 20, 19, 24, 26, 25, 21, 23, 22, 9, 11, 10, 15, 17, 16,
	12, 14, 13, 54, 56, 55, 60, 62, 61, 57, 59, 58, 72, 74, 73,
	78, 80, 79, 75, 77, 76, 63, 65, 64, 69, 71, 70, 66, 68, 67,
	27, 29, 28, 33, 35, 34, 30, 32, 31, 45, 47, 46, 51, 53, 52,
	48, 50, 49, 36, 38, 37, 42, 44, 43, 39, 41, 40};
unsigned int s_win[81] = {0, };
unsigned int s_num[81] = {0, };
double mem_val;
int inv;
int data_read()
{
	int i;
	FILE *file = fopen("./data/win.txt", "r");
	if (file == NULL) 
	{
		perror("No file(win.txt)");
		return 1;
	}
	for (i = 0; i < 81; i++)
	{
		fscanf(file, "%u", &s_win[i]);
	}
	fclose(file);
	file = fopen("./data/num.txt", "r");
	if(file == NULL)
	{
		perror("No file(num.txt)");
		return 1;
	}
	for(i = 0; i < 81; i++)
	{
		fscanf(file, "%u", &s_num[i]);
	}
	fclose(file);
	return 0;
}
int log_write(int r, int c, double prob, int state[])
{
	int i;
	FILE *file = fopen("./data/log.txt", "a");
	if(file == NULL)
	{
		perror("Cannot open file(log.txt)");
		return 1;
	}
	else
	{
		fprintf(file, "%2d %2d %.2lf ", r, c, prob);
		for(i= 0; i<8; i++) fprintf(file, "%d ", state[i]);
		fprintf(file, "\n");
		fclose(file);
	}
	return 0;

}
int state_cal(int row, int col, int type)
{
	int i, result = 0;
	int r = t_boardle_row[type];
	int c = t_boardle_col[type];
	int w = 27; //weight of state
	int flag = 0x0800;
	for(i=1; i<=4; i++)
	{
		row += r;
		col += c;
		if(row < 0 || row >= TAB_MAX || col < 0 || col >= TAB_MAX)
		{
			//printf("%d : %d %d no data\n", type, row, col);
			result =  result | flag; //flag means there is end of the boardle
			break;
		}
		result += w * board[row][col];
		w /=3;
		flag = flag >> 1;
	}
	return result;
}
double board_lookup(int s)
{
	int state = s;
	int i;
	int search = 81, flag = 0x0800;
	double result = 0;
	double win, num;
	double tmp;
	double min = 1;
	if((state & 0x0000ff00) != 0)
	{
		while((state & flag) == 0)
		{
			flag = flag >> 1;
			search /= 3;
		}
		state ^= flag;
		for(i=state; i<state+search; i++)
		{
			win = (double)s_win[i];
			num = (double)s_num[i];
			tmp = win / num;
			if(tmp < min) min = tmp;
		}
		result = min;
	}
	else
	{
		win = (double)s_win[state];
		num = (double)s_num[state];
		if(num <= 0) result = 0;
		else result = win / num;
	}
	return result;
}
int win_prob()
{
	int i, j, k;
	int state, flag;
	int mem_r = 0, mem_c = 0;
	int result = 0;
	int tmp_state[8] = {0, }, mem_state[8] = {0, };
	double tmp_prob[8] = {0. };
	double tmp_prob2[8] = {0, };
	double max, min;
	double tmp;
	double val;
	mem_val = -1.1;
	for(i=0; i<TAB_MAX; i++)
	{
		for(j=0; j<TAB_MAX; j++)
		{
			if(board[i][j] != 0) continue;
			prob[i][j] = 0;
			max = 0;
			val = 0;
			for(k=0; k<8; k++)
			{
				inv = 0;
				tmp_prob[k] = 0;
				state = state_cal(i, j, k);
				tmp_state[k] = state;
				//if(state & 0xff != 0 )printf("1. %d\n", state);
				tmp_prob[k] = board_lookup(state);
				//if(state & 0xff !=0) printf("[%d -> %lf]\n", state, tmp_prob[k]);
				flag = (state & 0x0ff00);
				//if(state & 0xff != 0) printf("2. %d\n", state);
				state = state & 0xff;
				state = inverse[state];
				state = state | flag;
				//if(state & 0xff != 0) printf("3. %d\n", state);
				tmp_prob2[k] = board_lookup(state);
				if(tmp_prob[k] > tmp_prob2[k]) tmp = tmp_prob[k];
				else
				{
					inv = 1;
					//if(state & 0xff != 0) printf("%d -> %lf\n", state, tmp_prob2[k]);
					tmp = tmp_prob2[k];
				}
				prob[i][j] +=tmp;
				if(max < tmp) max = tmp;
			}
			val = prob[i][j] + max;
			if(mem_val < val)
			{
				//printf("%f < %f\n", mem_val, prob[i][j]);
				mem_val = val;
				mem_r = i;
				mem_c = j;
				for(k=0; k<8; k++) mem_state[k] = tmp_state[k];
			}
		}
	}
	if(mem_r > 14 || mem_r < 0 || mem_c > 14 || mem_c < 0) printf("%d %d\n", mem_r, mem_c);
	result = mem_r;
	result = result << 8;
	result = result | mem_c;
	if(log_write(mem_r, mem_c, mem_val, mem_state)) perror("log write error");
	return result;
}
int play(char current[][TAB_MAX])
{
	//boardle value 0 : none, 1 : mine, 2: yours
	int i, j;
	for (i = 0; i < TAB_MAX; i++)
	{
		for(j=0; j < TAB_MAX; j++)
		{
			board[i][j] = current[i][j];
		}
	}
	if(data_read()) return -1;
	return win_prob();
}
