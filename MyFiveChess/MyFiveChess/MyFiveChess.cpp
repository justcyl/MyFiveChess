#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <cstdio>
#include <iostream>
#include <time.h>
#include <easyx.h>
#include <conio.h>
#include <graphics.h> 
#include <windows.h>

using namespace std;


#define GAP 49
#define CHESS_SIZE 32
#define LUPOINT 32
#define BG_SIZE 749

#define INF 2147483647
#define MAX_DEP 1

char Ai_Color = '1';

int Now_Score = 0;
bool Fiveexist;

IMAGE BOARD;
IMAGE WHITE_CHESS;
IMAGE BLACK_CHESS;

string Map;

int isGameover = 0;

void drawAlpha(IMAGE* picture, int  picture_x, int picture_y); //x为要载入图片的X坐标，y为Y坐标
void init();
void drawMap(int lastfill);
int getClick();
char xorColor(char ch);
int evaluateStr(string s);
int fillBoard(int pos, char ch);
int negmax(char color, int alpha, int beta, int depth);
int getNextMove(int pos);
bool isappearFive(string s);

int main() {
	
	
	init();
	putimage(0, 0, &BOARD);
	
	//Map.assign(255, '0');

	while (!isGameover) {
		
		int player_fill = getClick();

		Now_Score += fillBoard(player_fill, xorColor(Ai_Color));
		drawMap(player_fill);

		if (Fiveexist) {
			isGameover = xorColor(Ai_Color) - '0';
			break;
		}
		
		//Now_Score += fillBoard(6, xorColor(Ai_Color));
		int AI_fill = getNextMove(player_fill);

		Now_Score += fillBoard(AI_fill, Ai_Color);
		drawMap(AI_fill);

		//printf("%d", AI_fill);
		if (Fiveexist) {
			isGameover = Ai_Color - '0';
			break;
		}
	
	}

	if(isGameover == Ai_Color - '0') MessageBox(GetForegroundWindow(), L"Ai赢了", L"游戏结束", 1);
	else  MessageBox(GetForegroundWindow(), L"人类赢了", L"游戏结束", 1);
	/*
	Map.assign(255, '0');
	Map[0] = '1', Map[1] = '1', Map[2] = '1', Map[3] = '1';
	Now_Score += fillBoard(4, '1');
	if (Fiveexist) {
		printf("yes");
	}*/
	return 0;
}
char xorColor(char ch){
	if (ch == '0') return ch;
	else return '3' - ch + '0';
}
void init() {
	initgraph(BG_SIZE, BG_SIZE);

	loadimage(&BOARD, L"board.jpg", BG_SIZE, BG_SIZE);
	putimage(0, 0, &BOARD);


	loadimage(&WHITE_CHESS, L"white_chess.png", CHESS_SIZE, CHESS_SIZE);
	//drawAlpha(&WHITE_CHESS, CHESS_SIZE, CHESS_SIZE);


	loadimage(&BLACK_CHESS, L"black_chess.png", CHESS_SIZE, CHESS_SIZE);
	//drawAlpha(&BLACK_CHESS, CHESS_SIZE, CHESS_SIZE);

	Map.assign(255, '0');

}
void drawMap(int lastfill) {
	BeginBatchDraw();
	putimage(0, 0, &BOARD);
	for (int i = 0; i < 225; i++) {
		int px = (i / 15) * GAP + LUPOINT, py = (i % 15) * GAP + LUPOINT;
		switch (Map[i])
		{
		case'1':
			drawAlpha(&BLACK_CHESS, py - CHESS_SIZE / 2, px - CHESS_SIZE / 2); break;
		case'2':
			drawAlpha(&WHITE_CHESS, py - CHESS_SIZE / 2, px - CHESS_SIZE / 2); break;
		default:
			break;
		}
	}
	setcolor(GREEN);
	rectangle((lastfill % 15) * GAP + LUPOINT - CHESS_SIZE / 2 - 3, (lastfill / 15) * GAP + LUPOINT - CHESS_SIZE / 2 -3, (lastfill % 15) * GAP + LUPOINT + CHESS_SIZE / 2 + 3, (lastfill / 15) * GAP + LUPOINT + CHESS_SIZE / 2 + 3);
	//rectangle(CHESS_SIZE + 6, CHESS_SIZE + 6, (lastfill % 15) * GAP + LUPOINT - CHESS_SIZE / 2 - 3, (lastfill / 15) * GAP + LUPOINT - CHESS_SIZE / 2 - 3);

	EndBatchDraw();
}
int getClick()//如果是黑 1  如果是白  2
{
	MOUSEMSG msg;
	while (1)
	{
		msg = GetMouseMsg();
		switch (msg.uMsg)
		{
		case WM_LBUTTONDOWN:
			//    msg.y/SIZE-->行下标
			//    msg.x/SIZE-->列下标
			int cx = round((double)(msg.y - LUPOINT) / (double)GAP), cy = round((double)(msg.x - LUPOINT) / (double)GAP);
			if (cx < 0 || cy < 0) continue;
			if (cx > 14 || cy > 14) continue;
			if (abs(cx * GAP + LUPOINT - msg.y) > CHESS_SIZE / 2) continue;
			if (abs(cy * GAP + LUPOINT - msg.x) > CHESS_SIZE / 2) continue;

			if (Map[cx * 15 + cy] == '0')
			{
				//Map[cx * 15 + cy] =(char)( xorColor(Ai_Color));
				return cx * 15 + cy;//返回点击的位置
			}

			break;
		}
	}
}
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //x为载入图片的X坐标，y为Y坐标
{

	// 变量初始化
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标

	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}

const int fx[] = {0, 1, 1, 1};
const int fy[] = {1, 0, 1, -1};

string stringtable[16] = 
{"11111", "011110", "011100", "001110", "011010", "010110", "111110", "011111", "11011", "10111", "11101", "001100", "001010", "010100", "000100", "001000"};

int scoretable[16] = 
{50000, 4320, 720, 720, 720, 720, 720, 720, 720, 720, 720, 120, 120, 120, 20, 20};

int evaluateStr(string s){
	int begin = -1;
	int sum = 0;
	for (int i = 0; i < 16; i++){
		int count = 0;
		while((begin=s.find(stringtable[i], begin+1))!=string::npos){
			count++;
        }
		sum += count * scoretable[i];
	}
	return sum;
}
bool isappearFive(string s) {
	return s.find(stringtable[0], 0) != string::npos;
}
int fillBoard(int pos, char ch) {
	Fiveexist = false;

	
	int tx = pos / 15, ty = pos % 15;
	int centre;
	int delta = 0;

	for (int k = 0; k < 4; k++){
		string patch = "";
		for (int j = -5; j <= 5; j++){
			int nx = tx + fx[k] * j, ny = ty + fy[k] * j;
			if (nx < 0 || nx > 14) continue;
			if (ny < 0 || ny > 14) continue;

			patch.append(1, Map[nx * 15 + ny]);

			if (!j) centre = patch.size() - 1;
		}
		delta += -evaluateStr(patch);
		patch[centre] = ch;
		delta += evaluateStr(patch);

		if (isappearFive(patch)) Fiveexist = true;
	}

	for (int k = 0; k < 4; k++) {
		string patch = "";
		for (int j = -5; j <= 5; j++) {
			int nx = tx + fx[k] * j, ny = ty + fy[k] * j;
			if (nx < 0 || nx > 14) continue;
			if (ny < 0 || ny > 14) continue;

			patch.append(1, Map[nx * 15 + ny]);

			if (!j) centre = patch.size() - 1;
		}
		delta += -evaluateStr(patch);
		patch[centre] = ch;
		delta += evaluateStr(patch);

		if (isappearFive(patch)) Fiveexist = true;
	}
	for (int k = 0; k < 4; k++) {
		string patch = "";
		for (int j = -5; j <= 5; j++) {
			int nx = tx + fx[k] * j, ny = ty + fy[k] * j;
			if (nx < 0 || nx > 14) continue;
			if (ny < 0 || ny > 14) continue;

			patch.append(1, xorColor(Map[nx * 15 + ny]));

			if (!j) centre = patch.size() - 1;
		}
		delta -= -evaluateStr(patch);
		patch[centre] = xorColor(ch);
		delta -= evaluateStr(patch);

		if (isappearFive(patch)) Fiveexist = true;
	}

	Map[pos] = ch;

	return delta;
}
int negmax(char color, int alpha, int beta, int depth) {
	if (depth == MAX_DEP) {
		return Now_Score;
	}
	for (int i = 0; i < 225; i++) {
		if (Map[i] != '0') continue;
		Now_Score += fillBoard(i, color);
		int val = -negmax(xorColor(color), -beta, -alpha, depth + 1);
		Now_Score += fillBoard(i, '0');

		if (val > alpha) {
			if (val >= beta) {
				return beta;
			}
			alpha = val;
		}
	}
	return alpha;
}

int getNextMove(int pos){
	

	int  NM_max = -INF, ans = 0;
	for (int i = 0; i < 225; i++) {
		if (Map[i] != '0') continue;
		Now_Score += fillBoard(i, Ai_Color);
		int val = -negmax(xorColor(Ai_Color), -INF, INF, 0);
		Now_Score += fillBoard(i, '0');
		printf("%d %d\n", i, val);
		if (NM_max < val) {
			NM_max = val;
			ans = i;
		}
		
	}

	return ans;
}
