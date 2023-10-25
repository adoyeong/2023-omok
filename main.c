#include <stdio.h>
#include <string.h>
#define TAB_MAX 15
char tab[TAB_MAX][TAB_MAX];
double prob[TAB_MAX][TAB_MAX];
int t_table_row[8] = {1, 1, 1, 0, 0, -1, -1, -1};
int t_table_col[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
unsigned int s_win[81] = {0, };
unsigned int s_num[81] = {0, };
int data_read()
{
	int i;
	FILE *file = fopen("./data/win.txt", "r");
	if (file == NULL) 
	{
		perror("No file(win.txt)!");
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
		perror("No file(num.txt)!");
		return 1;
	}
	for(i = 0; i < 81; i++)
	{
		fscanf(file, "%u", &s_num[i]);
	}
	fclose(file);
	return 0;
}
int state_cal(int row, int col, int type)
{
	int i, result = 0;
	int r = t_table_row[type];
	int c = t_table_col[type];
	int w = 27; //weight of state
	int flag = 0x0800;
	for(i=1; i<=4; i++)
	{
		row += r;
		col += c;
		if(row < 0 || row >= TAB_MAX || col < 0 || col >= TAB_MAX)
		{
			result =  result | flag; //flag means there is end of the table
			break;
		}
		result += w * tab[row][col];
		w /=3;
		flag = flag >> 1;
	}
	return result;
}
double tab_lookup(int state)
{
	double result = 0;
	double win, num;
	if((state & 0x0000ff00) != 0)
	{
		int i;
		int flag = 0x0800;
		int search = 81;
		while((state & flag) == 0)
		{
			flag = flag >> 1;
			search /= 3;
		}
		for(i=state; i<state+search; i++)
		{
			win = (double)s_win[i];
			num = (double)s_num[i];
			if(num > 0) result = result + win / num; 
		}
		result = result / (double)search;
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
	int state;
	int mem_r, mem_c, mem_val;
	int result;
	mem_val = -1;
	for(i=0; i<TAB_MAX; i++)
	{
		for(j=0; j<TAB_MAX; j++)
		{
			prob[i][j] = 0;
			for(k=0; k<8; k++)
			{
				state = state_cal(i, j, k);
				prob[i][j] += tab_lookup(state);
			}
			prob[i][j] /= 8;
			if(mem_val < prob[i][j])
			{
				mem_val = prob[i][j];
				mem_r = i;
				mem_c = j;
			}
		}
	}
	result = i;
	result = result << 8;
	result = result | j;
	return result;
}
int main(char current[][TAB_MAX])
{
	//table value 0 : none, 1 : mine, 2: yours
	int i, j;
	for (i = 0; i < TAB_MAX; i++)
	{
		for(j=0; j < TAB_MAX; j++)
		{
			tab[i][j] = current[i][j];
		}
	}
	if(data_read()) return -1;
	return 0;

}
