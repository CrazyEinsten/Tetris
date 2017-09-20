#include <conio.h>
#include <graphics.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <winsock.h>
#include <Windows.h>
#include <process.h>
#include <mmsystem.h>
#include <malloc.h>
#include <dos.h>
#include <time.h>

#pragma comment(lib,"WINMM.LIB")
#pragma comment(lib,"ws2_32.lib")
#define MAXSTAR 200
#define test
#define BACKGROUND_X 20
#define BACKGROUND_Y 35
#define BLOCK_W 15
#define START_X 50
#define START_Y 50
#define X0 7
#define Y0 1

WSADATA wsa;
struct sockaddr_in clientaddr;

int scores = 0;
int color[7] = { RED,LIGHTRED,YELLOW,GREEN,LIGHTGREEN,LIGHTBLUE,BLUE };
int blocks[28][4][4] =//存放方块的形态
{
	{ 1,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0 },//* *
	{ 1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0 },//*
	{ 0,1,0,0,0,1,0,0,1,1,0,0,0,0,0,0 },//*
	{ 1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0 },//*

	{ 1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0 },//* *
	{ 0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0 },//  *
	{ 1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0 },//  *
	{ 1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0 },//  *

	{ 1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0 },//*
	{ 0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0 },//*
	{ 1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0 },//*
	{ 0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0 },//*

	{ 0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0 },//  *
	{ 0,1,0,0,0,1,1,0,0,1,0,0,0,0,0,0 },//* * *
	{ 0,0,0,0,1,1,1,0,0,1,0,0,0,0,0,0 },//  *
	{ 0,1,0,0,1,1,0,0,0,1,0,0,0,0,0,0 },//* * *

	{ 1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0 },//
	{ 1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0 },//
	{ 1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0 },//* *
	{ 1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0 },//* *	

	{ 1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0 },//  *
	{ 0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0 },//* *
	{ 1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0 },//*
	{ 0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0 },//

	{ 0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0 },//*
	{ 0,1,0,0,0,1,1,0,0,0,1,0,0,0,0,0 },//* *
	{ 0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0 },//  *
	{ 0,1,0,0,0,1,1,0,0,0,1,0,0,0,0,0 }//
};
void level_update();
int background[BACKGROUND_X][BACKGROUND_Y] = { 0 };
void scores_update();
void InitBackground()
{
	setlinecolor(BLACK);
	for (int i = 0; i < BACKGROUND_X; i++)
		for (int j = 0; j < BACKGROUND_Y; j++)
			if (i == BACKGROUND_X - 1 || i == 0 || j == BACKGROUND_Y - 1 || j == 0)
				background[i][j] = 4;
	LOGFONT k;
	gettextstyle(&k);
	k.lfHeight = 20;
	settextcolor(WHITE);
	_tcscpy_s(k.lfFaceName, _T("黑体"));
	k.lfQuality = ANTIALIASED_QUALITY;
	outtextxy(400, 60, "THE NEXT TYPE");
	outtextxy(400, 225, "GRADE");
	outtextxy(420, 245, "0");
	outtextxy(400, 265, "LEVEL");
	outtextxy(400, 285, "倔强青铜");
	outtextxy(400, 350, "操作提示");
	outtextxy(400, 380, "A: 左移");
	outtextxy(400, 410, "D: 左移");
	outtextxy(400, 440, "W: 变形");
	outtextxy(400, 470, "S: 快速下落");
	outtextxy(400, 500, "空格: 暂停/开始");

}

typedef struct ROCKTYPE {
	int **type;
	struct ROCKTYPE *next;
}RT;

//-------------------------------------------------------------------------------------------------

RT **rt;
RT *PRT;

int flag;

void InitRocktype()
{
	int tmp = 0, j = 0;
	rt = (RT**)malloc(sizeof(RT**) * 7);
	for (int i = 0; i < 7; i++)
		rt[i] = (RT*)malloc(sizeof(RT*) * 4);
	for (int i = 0; i < 7; i++)
		for (j = 0; j < 4; j++)
		{
			rt[i][j].type = (int**)malloc(sizeof(int*) * 4);
			rt[i][j].next = (RT*)malloc(sizeof(RT*));
			for (int s = 0; s < 4; s++)
				rt[i][j].type[s] = (int*)malloc(sizeof(int) * 4);
		}
	RT *next = NULL;
	for (int i = 0; i < 7; i++)
	{
		for (j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
				for (int m = 0; m < 4; m++)
					rt[i][j].type[k][m] = blocks[tmp][k][m];
			rt[i][j].next = next;
			next = &rt[i][j];
			tmp++;
		}
		rt[i][0].next = next;
	}
}

int getmax_X(int x, int y)//得到最大横向坐标
{
	int i, j, t = 0;
	for (i = 0; i<4; i++)
		for (j = 0; j<4; j++)
			if (background[x + i][y + j] == 1 && t <= x + i)
				t = x + i;
	return t;
}

int getmax_Y(int x, int y)//得到最大纵向坐标
{
	int i, j, t = 0;
	for (i = 0; i<4; i++)
		for (j = 0; j<4; j++)
			if (background[x + i][y + j] == 1 && t <= y + j)
				t = y + j;
	return t;
}

int getmin_X(int x, int y)//得到最小横向坐标
{
	int i, j, t = x + 3;
	for (i = 0; i<4; i++)
		for (j = 0; j<4; j++)
			if (background[x + i][y + j] == 1 && t >= x + i)
				t = x + i;
	return t;
}

int moveable_right(int x, int y)
{
	int i, j;
	for (i = 0; i<4; i++)
		for (j = 0; j<4; j++)
			if (background[getmax_X(x, y) + 1][y] == 4 || background[x + i][y + j] + background[x + i + 1][y + j] == 3)
				return 0;
	return 1;
}

int moveable_left(int x, int y)
{
	int i, j;
	for (i = 0; i<getmax_X(x, y); i++)
		for (j = 0; j<4; j++)
			if (background[getmin_X(x, y) - 1][y] == 4 || background[x + i][y + j] + background[x + i - 1][y + j] == 3)
				return 0;
	return 1;
}

int moveable_down(int x, int y)
{
	int i, j;
	for (i = 0; i<4; i++)
		for (j = 0; j<4; j++)
			if (background[x][getmax_Y(x, y) + 1] == 4 || (background[x + i][y + j] == 1 && background[x + i][y + j] + background[x + i][y + j + 1] == 3))
				return 0;
	return 1;
}

void Wall_one()
{
	int x = START_X, y = START_Y;
	setfillcolor(YELLOW);
	for (int j = 0; j < BACKGROUND_Y; )
	{
		for (int i = 0; i < BACKGROUND_X; i++)
		{
			if (background[i][j] == 4)
				fillrectangle(x, y, x + BLOCK_W, y + BLOCK_W);
			x += BLOCK_W;
		}
		j++;
		y = START_Y + j * BLOCK_W;
		x = START_X;
	}
}

void draw(int x, int y)
{
	int i, j;
	setlinecolor(BLACK);
	setfillcolor(LIGHTRED);
	for (i = 0; i<4; i++)
		for (j = 0; j<4; j++)
		{
			if (PRT->type[i][j] == 1)
			{
				fillrectangle(START_X + (x + j) * BLOCK_W, START_Y + (y + i) * BLOCK_W, START_X + BLOCK_W + (x + j) * BLOCK_W, START_Y + BLOCK_W + (y + i) * BLOCK_W);
				background[x + j][y + i] = 1;//改变数组的内存值
			}
		}
}

void clear(int x, int y)//每次clear后原来大数组区域内对应的内容重新置0。
{
	int i, j;
	setfillcolor(BLACK);
	for (i = 0; i<4; i++)
		for (j = 0; j<4; j++)
			if (background[x + j][y + i] == 1)
			{
				solidrectangle((x + j) * BLOCK_W + START_X, (y + i) * BLOCK_W + START_Y, (x + j) * BLOCK_W + START_X + BLOCK_W, (y + i) * BLOCK_W + START_Y + BLOCK_W);
				background[x + j][y + i] = 0;
			}
}

void ridrow(int row)
{
	int i = 1;

	int all = 0;
	setfillcolor(BLACK);
	for (i = 1; i <= 18; i++)
	{
		fillrectangle(50 + i * 15, 50 + row * 15, 50 + i * 15 + 15, 50 + row * 15 + 15);

	}
	while (all != 18)
	{
		all = 0;
		for (i = 1; i <= 18; i++)
		{
			background[i][row] = background[i][row - 1];

			setlinecolor(BLACK);
			setfillcolor(BLACK);
			fillrectangle(50 + i * 15, 50 + row * 15 - 15, 50 + i * 15 + 15, 50 + row * 15);
			if (background[i][row] == 2)
			{
				setlinecolor(BLACK);
				setfillcolor(LIGHTRED);
				fillrectangle(50 + i * 15, 50 + row * 15, 50 + i * 15 + 15, 50 + row * 15 + 15);
			}
			if (background[i][row] == 0)

				all++;

		}
		row--;
	}
}

void fullrow()
{
	int i = 1;
	int all = 0;
	bool full = 1;
	int y = 33;
	while (all != 18)
	{
		all = 0;
		full = 1;
		for (i = 1; i <= 18; i++)
		{
			if (background[i][y] == 0)
			{
				full = 0;
				all++;
				//break;
			}
		}
		if (full)
		{
			ridrow(y);
			y++;
			scores_update();
		}
		y--;
	}
}

bool isGameover()
{
	int i = 0;
	bool top = 0;
	bool bottom = 0;
	for (i = 1; i <= BACKGROUND_X - 2; i++)
	{
		if (background[i][1] == 2)
			top = 1;
		if (background[i][BACKGROUND_Y - 2] == 2)
			bottom = 1;
	}
	if (top == 1 && bottom == 1)
		return 1;
	else
		return 0;
}

void Change(int x, int y)
{
	int key = 1;
	int xx = 0, yy = 0;
	xx = x + 4 > BACKGROUND_X ? BACKGROUND_X - x : 4;
	yy = y + 4> BACKGROUND_Y ? BACKGROUND_Y - y : 4;
	int tmpA[4][4] = { 0 };
	int tmpB[4][4] = { 0 };
	for (int i = 0; i < xx; i++)
		for (int j = 0; j < yy; j++)
			tmpA[i][j] = background[x + i][y + j];
	RT *tmp;
	tmp = PRT;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			tmpB[i][j] = tmp->type[i][j];
	for (int i = 0; i < xx; i++)
		for (int j = 0; j < yy; j++)
			if ((tmpB[i][j] == 1 && tmpA[i][j] == 4) || (tmpB[i][j] == 1 && tmpA[i][j] == 2))
				key = 0;
	if (key)
		PRT = PRT->next;
}

void fall()
{
	InitRocktype();
	int  i = 1, xx = 1, speed;
	clock_t oldtime;//生成方块的时间
	clock_t nowtime;//程序当前运行率时间
	srand((unsigned)time(0));
	while (1)
	{//1111
		xx = X0;
		flag = rand() % 6;
		PRT = &rt[flag][0];
		draw(X0, Y0);
		oldtime = nowtime = clock();
		for (i = Y0; i <= BACKGROUND_Y - 4; i++)
		{
			speed = 1000;
			//if (nowtime - oldtime >= speed)

			while (nowtime - oldtime <= speed)
			{
				nowtime = clock();
				if (_kbhit())
				{
					char ch;
					ch = _getch();
					switch (ch)
					{
					case 'W':
					case 'w':
					{
						Change(xx, i);
						break;
					}
					case 'a':
					case 'A':
					{
						if (moveable_left(xx, i))//
						{
							clear(xx, i);
							xx -= 1;
							draw(xx, i);
						}
						break;
					}
					case 'd':
					case 'D':
					{
						if (moveable_right(xx, i))//
						{
							clear(xx, i);
							xx += 1;
							draw(xx, i);
						}
						break;
					}
					case 's':
					case 'S':
					{
						speed = 5;
						break;
					}
					default:break;
					}
				}
			}
			if (!moveable_down(xx, i))
				break;
			clear(xx, i);
			draw(xx, i + 1);
			oldtime = nowtime;
		}
		for (int p = 1; p <= BACKGROUND_X - 1; p++)
			for (int q = 1; q <= BACKGROUND_Y - 1; q++)
				if (background[p][q] == 1)
					background[p][q] = 2;
		if (isGameover())
			break;
		fullrow();
	}//1111111111111
	free(rt);
}

///*---------------------------------------------------------------------------------------------
/*
int background2[BACKGROUND_X][BACKGROUND_Y] = { 0 };

RT **rt2;
RT *PRT2;
int flag2;

void InitBackground2()
{
setlinecolor(BLACK);
for (int i = 0; i <20; i++)
for (int j = 0; j <35; j++)
if (i == 20 - 1 || i == 0 || j == 35 - 1 || j == 0)
background2[i][j] = 4;
}

void InitRocktype2()
{
int tmp = 0, j = 0;
rt2 = (RT**)malloc(sizeof(RT**) * 7);
for (int i = 0; i < 7; i++)
rt2[i] = (RT*)malloc(sizeof(RT*) * 4);
for (int i = 0; i < 7; i++)
for (j = 0; j < 4; j++)
{
rt2[i][j].type = (int**)malloc(sizeof(int*) * 4);
rt2[i][j].next = (RT*)malloc(sizeof(RT*));
for (int s = 0; s < 4; s++)
rt2[i][j].type[s] = (int*)malloc(sizeof(int) * 4);
}
RT *next = NULL;
for (int i = 0; i < 7; i++)
{
for (j = 0; j < 4; j++)
{
for (int k = 0; k < 4; k++)
for (int m = 0; m < 4; m++)
rt2[i][j].type[k][m] = blocks[tmp][k][m];
rt2[i][j].next = next;
next = &rt2[i][j];
tmp++;
}
rt2[i][0].next = next;
}
}


int getmax_X2(int x, int y)//得到最大横向坐标
{
int i, j, t = 0;
for (i = 0; i<4; i++)
for (j = 0; j<4; j++)
if (background2[x + i][y + j] == 1 && t <= x + i)
t = x + i;
return t;
}

int getmax_Y2(int x, int y)//得到最大纵向坐标
{
int i, j, t = 0;
for (i = 0; i<4; i++)
for (j = 0; j<4; j++)
if (background2[x + i][y + j] == 1 && t <= y + j)
t = y + j;
return t;
}

int getmin_X2(int x, int y)//得到最小横向坐标
{
int i, j, t = x + 3;
for (i = 0; i<4; i++)
for (j = 0; j<4; j++)
if (background2[x + i][y + j] == 1 && t >= x + i)
t = x + i;
return t;
}

int moveable_right2(int x, int y)
{
int i, j;
for (i = 0; i<4; i++)
for (j = 0; j<4; j++)
if (background2[getmax_X2(x, y) + 1][y] == 4 || background2[x + i][y + j] + background2[x + i + 1][y + j] == 3)
return 0;
return 1;
}

int moveable_left2(int x, int y)
{
int i, j;
for (i = 0; i<getmax_X2(x, y); i++)
for (j = 0; j<4; j++)
if (background2[getmin_X(x, y) - 1][y] == 4 || background2[x + i][y + j] + background2[x + i - 1][y + j] == 3)
return 0;
return 1;
}

int moveable_down2(int x, int y)
{
int i, j;
for (i = 0; i<4; i++)
for (j = 0; j<4; j++)
if (background2[x][getmax_Y2(x, y) + 1] == 4 || (background2[x + i][y + j] == 1 && background2[x + i][y + j] + background2[x + i][y + j + 1] == 3))
return 0;
return 1;
}

void Wall_two()
{
int x = 450, y = 50;
setfillcolor(YELLOW);
for (int j = 0; j < 35; )
{
for (int i = 0; i < 20; i++)
{
if (background2[i][j] == 4)
fillrectangle(x, y, x + 15, y + 15);
x += 15;
}
j++;
y = 50 + j * 15;
x = 450;
}
}

void draw2(int x, int y)
{
int i, j;
setlinecolor(BLACK);
setfillcolor(color[flag2]);
for (i = 0; i<4; i++)
for (j = 0; j<4; j++)
{
if (PRT2->type[i][j] == 1)
{
fillrectangle(450 + (x + j) * 15, 50 + (y + i) * 15, 450 + 15 + (x + j) * 15, 50 + 15 + (y + i) * 15);
background2[x + j][y + i] = 1;//改变数组的内存值
}
}
}

void clear2(int x, int y)//每次clear后原来大数组区域内对应的内容重新置0。
{
int i, j;
setfillcolor(BLACK);
for (i = 0; i<4; i++)
for (j = 0; j<4; j++)
if (background2[x + j][y + i] == 1)
{
solidrectangle((x + j) * 15 + 450, (y + i) * 15 + 50, (x + j) * 15 + 450 + 15, (y + i) * 15 + 50 + 15);
background2[x + j][y + i] = 0;
//solidrectangle((x + j) * BLOCK_W + START_X, (y + i) * BLOCK_W + START_Y, (x + j) * BLOCK_W + START_X + BLOCK_W, (y + i) * BLOCK_W + START_Y + BLOCK_W);
//	background[x + j][y + i] = 0;
}
}

void ridrow2(int row)
{
int i = 1;

int all = 0;
setfillcolor(BLACK);
for (i = 1; i <= 18; i++)
{
fillrectangle(450 + i * 15, 50 + row * 15, 450 + i * 15 + 15, 50 + row * 15 + 15);

}
while (all != 18)
{
all = 0;
for (i = 1; i <= 18; i++)
{
background2[i][row] = background2[i][row - 1];

setlinecolor(BLACK);
setfillcolor(BLACK);
fillrectangle(450 + i * 15, 50 + row * 15 - 15, 450 + i * 15 + 15, 50 + row * 15);
if (background2[i][row] == 2)
{
setlinecolor(BLACK);
setfillcolor(WHITE);
fillrectangle(450 + i * 15, 50 + row * 15, 450 + i * 15 + 15, 50 + row * 15 + 15);
}
if (background2[i][row] == 0)

all++;

}
row--;
}
}

void fullrow2()
{
int i = 1;
int all = 0;
bool full = 1;
int y = 33;
while (all != 18)
{
all = 0;
full = 1;
for (i = 1; i <= 18; i++)
{
if (background2[i][y] == 0)
{
full = 0;
all++;
//break;
}
}
if (full)
{
ridrow2(y);
y++;
}
y--;
}
}

bool isGameover2()
{
int i = 0;
bool top = 0;
bool bottom = 0;
for (i = 1; i <= 20 - 2; i++)
{
if (background2[i][1] == 2)
top = 1;
if (background2[i][35 - 2] == 2)
bottom = 1;
}
if (top == 1 && bottom == 1)
return 1;
else
return 0;
}

void Change2(int x, int y)
{
int key = 1;
int xx = 0, yy = 0, i;
xx = x + 4 > 20 ? 20 - x : 4;
yy = y + 4> 35 ? 35 - y : 4;
int tmpA[4][4] = { 0 };
int tmpB[4][4] = { 0 };
for (i = 0; i < xx; i++)
for (int j = 0; j < yy; j++)
tmpA[i][j] = background2[x + i][y + j];
RT *tmp;
tmp = PRT2;
for (i = 0; i < 4; i++)
for (int j = 0; j < 4; j++)
tmpB[i][j] = tmp->type[i][j];
for (i = 0; i < xx; i++)
for (int j = 0; j < yy; j++)
if ((tmpB[i][j] == 1 && tmpA[i][j] == 4) || (tmpB[i][j] == 1 && tmpA[i][j] == 2))
key = 0;
if (key)
PRT2 = PRT2->next;
}

void fall2()
{
InitRocktype2();
int  i = 1, xx = 1, speed;
clock_t oldtime;//生成方块的时间
clock_t nowtime;//程序当前运行率时间
srand((unsigned)time(0));
while (1)
{//1111
xx = 7;
flag2 = rand() % 6;
PRT2 = &rt2[flag2][0];
draw2(7, 1);
oldtime = nowtime = clock();
for (i = 1; i <= 35 - 4; i++)
{
speed = 1000;
//if (nowtime - oldtime >= speed)

while (nowtime - oldtime <= speed)
{
nowtime = clock();
if (_kbhit())
{
char ch1, ch2;
ch1 = _getch();
if (ch1<0) { ch2 = _getch(); }
switch (ch2)
{
case 72:

{
Change2(xx, i);
break;
}
case 75:

{
if (moveable_left2(xx, i))//
{
clear2(xx, i);
xx -= 1;
draw2(xx, i);
}
break;
}
case 77:

{
if (moveable_right2(xx, i))//
{
clear2(xx, i);
xx += 1;
draw2(xx, i);
}
break;
}
case 80:

{
speed = 5;
break;
}
default:break;
}
}
}
if (!moveable_down2(xx, i))
break;
clear2(xx, i);
draw2(xx, i + 1);
oldtime = nowtime;
}
for (int p = 1; p <= 20 - 1; p++)
for (int q = 1; q <= 35 - 1; q++)
if (background2[p][q] == 1)
background2[p][q] = 2;
if (isGameover2())
break;
fullrow2();
}//1111111111111
free(rt2);
}
*/
//---------------------------------------------------------------------------------------------------

/*

UINT ThreadTP(PVOID pmid)
{
//InitBackground2();
//Wall_two();
//fall2();
return 0;
}

UINT ThreadFP(PVOID pimd)
{
InitBackground();
Wall_one();
fall();
return 0;
}

void TwoPlayers()
{
HANDLE *handle;
handle = (HANDLE*)malloc(2 * sizeof(HANDLE));
handle[0] = (HANDLE)_beginthreadex(0, 0, (unsigned int(__stdcall *)(void *))ThreadTP, 0, 0, 0);
handle[1] = (HANDLE)_beginthreadex(0, 0, (unsigned int(__stdcall *)(void *))ThreadFP, 0, 0, 0);
WaitForMultipleObjects(2, handle, 1, INFINITE);
CloseHandle(handle);
}
*/


void scores_update()
{
	char buffer[10];
	scores += 10;
	sprintf_s(buffer, "%ld", scores);
	outtextxy(390, 245, "            ");
	outtextxy(400, 245, buffer);
	level_update();
}

void level_update()
{
	if (scores >= 1000 && scores<3000)
	{
		outtextxy(325, 285, "           ");
		outtextxy(330, 285, "华贵铂金");
	}
	else if (scores >= 3000 && scores<5000)
	{
		outtextxy(325, 285, "           ");
		outtextxy(330, 285, "璀璨钻石");
	}
	else if (scores >= 5000 && scores<100000)
	{
		outtextxy(325, 285, "           ");
		outtextxy(330, 285, "超凡大师");
	}
	else if (scores >= 100000)
	{
		outtextxy(325, 285, "           ");
		outtextxy(330, 285, "最强王者");
	}
	else;
}

void OnePlayer()
{
	InitBackground();
	Wall_one();
	fall();
}



void InitWSA()
{
	WSAStartup(MAKEWORD(2, 2), &wsa);
	memset(&clientaddr, 0, sizeof(clientaddr));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(1234);
	clientaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
}

struct STAR
{
	double	x;
	int		y;
	double	step;
	int		color;
};

STAR star[MAXSTAR];

// 初始化星星
void InitStar(int i)
{
	star[i].x = 0;
	star[i].y = rand() % 600;
	star[i].step = (rand() % 5000) / 1000.0 + 1;
	star[i].color = (int)(star[i].step * 255 / 6.0 + 0.5);	// 亮度正比于速度。
	star[i].color = RGB(star[i].color, star[i].color, star[i].color);
}
int key = 1;
// 移动星星
void MoveStar(int i)
{
	putpixel((int)star[i].x, star[i].y, 0);
	// 计算新位置
	star[i].x += star[i].step;
	if (star[i].x >= 347 && star[i].x <= 503 && star[i].y >= 297 && star[i].y <= 353)
		star[i].y = 365;
	else if (star[i].x >= 347 && star[i].x <= 503 && star[i].y >= 377 && star[i].y <= 433)
		star[i].y = 445;
	else if (star[i].x >= 347 && star[i].x <= 503 && star[i].y >= 457 && star[i].y <= 513)
		star[i].y = 525;
	else if (star[i].x >= 297 && star[i].x <= 553 && star[i].y >= 134 && star[i].y <= 219)
		star[i].y = 256;
	if (star[i].x > 800)	InitStar(i);
	// 画新星星
	putpixel((int)star[i].x, star[i].y, star[i].color);
}
void stars()
{
	srand((unsigned)time(NULL));
	for (int i = 0; i < MAXSTAR; i++)
	{
		InitStar(i);
		star[i].x = rand() % 800;
	}
	// 绘制星空，按下鼠标键退出
	while (key)
	{
		for (int i = 0; i < MAXSTAR ; i++)
			MoveStar(i);
		Sleep(20);
	}
}


LOGFONT f;

void UI_MAIN()
{
	cleardevice();
	gettextstyle(&f);
	f.lfHeight = 50;
	_tcscpy_s(f.lfFaceName, _T("黑体"));
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);
	outtextxy(300, 150, _T("俄罗斯方块"));
	f.lfHeight = 35;
	settextstyle(&f);
	rectangle(350, 300, 500, 350);
	outtextxy(390, 310, _T("登录"));
	rectangle(350, 380, 500, 430);
	outtextxy(390, 390, _T("注册"));
	rectangle(350, 460, 500, 510);
	outtextxy(390, 470, _T("退出"));
}

int UI_SIGN(SOCKET sock)
{
	send(sock, "sign", 1024, 0);
	char name[1024] = { 0 }, num[1024] = { 0 }, rev[1024] = { 0 };
	InputBox(name, 10, _T("请输入用户名"), "注册");
	send(sock, name, 1024, 0);
	recv(sock, rev, 1024, 0);
	if (strcmp(rev, "用户名已被注册") == 0)
	{
		MessageBox(0, "用户名已被注册", "注册", MB_OK| MB_SETFOREGROUND);
		return 0;
	}
	InputBox(num, 20, _T("请输入密码"), "注册");
	send(sock, num, 1024, 0);
	InputBox(num, 20, _T("请再次输入密码"), "注册");
	send(sock, num, 1024, 0);
	recv(sock, rev, 1024, 0);
	if (strcmp(rev, "注册成功") == 0)
		MessageBox(0,"注册成功","注册",MB_OK|MB_SETFOREGROUND);
	else
		MessageBox(0, "密码错误", "注册", MB_OK| MB_SETFOREGROUND);
	return 0;
}

void UI_LOGIN(SOCKET sock)
{
	send(sock, "login", 1024, 0);
	char name[1024] = { 0 }, num[1024] = { 0 }, rev[1024] = { 0 };
	InputBox(name, 10, _T("请输入用户名"), "登录");
	send(sock, name, 1024, 0);
	recv(sock, rev, 1024, 0);
	if (strcmp(rev, "用户名错误") == 0)
	{
		MessageBox(0, "用户名错误", "登录", MB_OK| MB_SETFOREGROUND);
		return;
	}
	InputBox(num, 20, _T("请输入密码"), "登录");
	send(sock, num, 1024, 0);
	recv(sock, rev, 1024, 0);
	if (strcmp(rev, "登陆成功") == 0)
	{
		MessageBox(0, "登陆成功", "登陆", MB_OK | MB_SETFOREGROUND);
		key = 0;
	}
	else
		MessageBox(0, "密码错误", "登陆", MB_OK| MB_SETFOREGROUND);
}

UINT Thread(LPVOID param)
{
	stars();
	return 0;
}

UINT Sound(LPVOID param)
{
	PlaySound("E:\\vs\\DIY\\C_C++\\Project2\\Project2\\The Cab - Angel With A Shotgun.WAV", NULL, SND_ASYNC | SND_LOOP);
	return 0;
}


void Chose()
{
	loadimage(NULL, "C:\\Users\\dell\\Desktop\\登录界面\\a.jpg", 800, 600);
	setbkmode(TRANSPARENT);
	char output[20] = "俄罗斯方块";
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 50;
	_tcscpy_s(f.lfFaceName, _T("黑体"));
	f.lfQuality = ANTIALIASED_QUALITY;
	settextcolor(WHITE);
	settextstyle(&f);
	outtextxy(300, 150, _T(output));


	settextstyle(40, 20, _T("黑体"));
	setfillcolor(RED);
	//solidrectangle(335,260,490,320);
	outtextxy(335, 270, "单人模式");
	//solidrectangle(335,330,490,390);
	outtextxy(335, 340, "双人模式");
	//solidrectangle(335,400,490,460);
	outtextxy(335, 410, "人机对战");



	MOUSEMSG a;
	char buffer[20] = "single";
	char buffer1[20] = "double";
	char buffer2[20] = "return";
	while (1)
	{
		//	MouseHit();
		a = GetMouseMsg();
		if (a.mkLButton&&a.x >= 320 && a.y >= 240 && a.x <= 475 && a.y <= 300)
		{
			settextstyle(10, 10, _T("黑体"));
			outtextxy(0, 20, buffer);
			cleardevice();
			OnePlayer();
			break;
		}
		if (a.mkLButton&&a.x >= 320 && a.y >= 370 && a.x <= 475 && a.y <= 430)
		{
			settextstyle(10, 10, _T("黑体"));
			outtextxy(0, 40, buffer1);
			break;
		}
		if (a.mkLButton&&a.x >= 320 && a.y >= 490 && a.x <= 475 && a.y <= 550)
		{
			settextstyle(10, 10, _T("黑体"));
			outtextxy(0, 60, buffer2);
			closegraph();
			break;
		}
	}
	system("pause");
	closegraph();
}



void signin()
{
	initgraph(800, 600);
	UI_MAIN();
	_beginthreadex(0, 0, (unsigned int(__stdcall *)(void *))Sound, 0, 0, 0);
	_beginthreadex(0, 0, (unsigned int(__stdcall *)(void *))Thread, 0, 0, 0);
	InitWSA();
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	int flag = connect(sock, (struct sockaddr *)&clientaddr, sizeof(struct sockaddr));
	if (flag == SOCKET_ERROR)
	{

		printf("连接出错");
		closesocket(sock);
		system("pause");
		return;
	}
	while (1)
	{
		MOUSEMSG msg = GetMouseMsg();
		if (msg.uMsg == WM_LBUTTONDOWN)
		{
			if (msg.x >= 350 && msg.x <= 500 && msg.y >= 300 && msg.y <= 350)
			{
				UI_LOGIN(sock);
				cleardevice();
				Chose();
			}
			if (msg.x >= 350 && msg.x <= 500 && msg.y >= 380 && msg.y <= 430)
			{
				UI_SIGN(sock);
			}
			if (msg.x >= 350 && msg.x <= 500 && msg.y >= 460 && msg.y <= 510)
				exit(1);
		}
	}


	system("pause");
	closegraph();
}



#ifdef	test
void main()
{
	signin();
}
#endif
