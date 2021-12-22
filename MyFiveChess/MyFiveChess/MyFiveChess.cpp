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
#include <algorithm>
#include <vector>

using namespace std;


#define GAP 49
#define CHESS_SIZE 41
#define LUPOINT 32
#define BG_SIZE 749

#define INF 2147483647
#define MAX_DEP 2

char Ai_Color = '1';

int Now_Score = 0;
bool Fiveexist;

IMAGE BOARD;
IMAGE WHITE_CHESS;
IMAGE BLACK_CHESS;

string rrow[15], lline[15], mdia[16], sdia[16];
string Map;

int isGameover = 0;

int lastPos;
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y); //x为要载入图片的X坐标，y为Y坐标
void initGraph();
void drawMap(int lastfill);
int getClick();
char xorColor(char ch);
int evaluateStr(string s);
int evaluatePos(int pos, char ch);
int negmax(char color, int alpha, int beta, int depth);
int getNextMove(int pos);
void setAi_Color(int x);
bool isappearFive(string s);
int evaluateMdia(int x);
int evaluateSdia(int x);
void placeChess(int x, char ch);
void initAI();

struct Point {
	int p, v;

	Point() { 
		p = 0, v = 0;
	}
	Point(int x, int y) {
		p = x, v = y;
	}

	bool operator < (const Point& node) const {
		return v < node.v;
	}
};

int main() {
	
	initAI();
	initGraph();

	
	while (!isGameover) {

		int player_place = getClick();

		Fiveexist = false;
		placeChess(player_place, xorColor(Ai_Color));
		//Map[player_fill] = xorColor(Ai_Color);

		drawMap(player_place);

		if (Fiveexist) {
			isGameover = xorColor(Ai_Color) - '0';
			break;
		}

		//Now_Score += evaluatePos(6, xorColor(Ai_Color));
		int AI_place = getNextMove(player_place);

		Fiveexist = false;
		placeChess(AI_place, (Ai_Color));
		//Map[AI_place] = Ai_Color;

		drawMap(AI_place);

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
	Now_Score += evaluatePos(4, '1');
	if (Fiveexist) {
		printf("yes");
	}*/
	/*
	{
		int Player_fill = 10;

		Now_Score += evaluatePos(Player_fill, xorColor(Ai_Color));
		Map[Player_fill] = xorColor(Ai_Color);
		int AI_fill = getNextMove(Player_fill);

		Now_Score += evaluatePos(AI_fill, Ai_Color);
		printf("%d\n", AI_fill);
	}*/
	return 0;
}
void initAI() {
	Map.assign(255, '0');
	setAi_Color(2);

	for (int i = 0; i < 15; i++) lline[i].append(15, '0'), rrow[i].append(15, '0');
	for (int i = 1; i < 16; i++) mdia[i].append(14, '0');
	mdia[0].append(15, '0');

	for (int i = 1; i < 14; i++) sdia[i].append(16, '0');
	sdia[0].append(17, '0');
}
char xorColor(char ch){
	if (ch == '0') return ch;
	else return '3' - ch + '0';
}
void initGraph() {
	initgraph(BG_SIZE, BG_SIZE);

	loadimage(&BOARD, L"board.jpg", BG_SIZE, BG_SIZE);
	putimage(0, 0, &BOARD);


	loadimage(&WHITE_CHESS, L"white_chess.png", CHESS_SIZE, CHESS_SIZE);
	//drawAlpha(&WHITE_CHESS, CHESS_SIZE, CHESS_SIZE);


	loadimage(&BLACK_CHESS, L"black_chess.png", CHESS_SIZE, CHESS_SIZE);
	//drawAlpha(&BLACK_CHESS, CHESS_SIZE, CHESS_SIZE);

	putimage(0, 0, &BOARD);

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
int evaluateBoard();
const int fx[] = {0, 1, 1, 1};
const int fy[] = {1, 0, 1, -1};

string stringtable[16] = 
{"11111", "011110", "011100", "001110", "011010", "010110", "11110", "01111", "11011", "10111", "11101", "001100", "001010", "010100", "000100", "001000"};

int scoretable[16] = 
{50000, 4320, 720, 720, 720, 720, 720, 720, 720, 720, 720, 120, 120, 120, 20, 20};


void setAi_Color(int x) {
	Ai_Color = x + '0';
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < stringtable[i].size(); j++) {
			if (stringtable[i][j] != '0') stringtable[i][j] = Ai_Color;
		}
	}
}

 
int evaluateBoard() {
	int ans = 0;
	for (int i = 0; i < 15; i++) ans += evaluateStr(lline[i]) + evaluateStr(rrow[i]);
	for (int i = 0; i < 16; i++) ans += evaluateMdia(i);
	for (int i = 0; i < 14; i++) ans += evaluateSdia(i);
	return ans;
}
int evaluateStr(string s){
	if (s.size() < 5) return 0;

	int begin = -1;
	int sum = 0;
	for (int i = 0; i < 16; i++){
		int count = 0;
		while((begin=s.find(stringtable[i], begin+1))!=string::npos){
			count++;
        }
		sum += count * scoretable[i];
	}
	if (isappearFive(s)) Fiveexist = true;
	
	for (int j = 0; j < s.size(); j++) s[j] = xorColor(s[j]);

	for (int i = 0; i < 16; i++) {
		int count = 0;
		while ((begin = s.find(stringtable[i], begin + 1)) != string::npos) {
			count++;
		}
		sum -= count * scoretable[i];
	}
	
	if (isappearFive(s)) Fiveexist = true;
	return sum;
}
bool isappearFive(string s) {
	return s.find(stringtable[0], 0) != string::npos;
}
int evaluateMdia(int x) {
	if (x == 0 || x == 15) return evaluateStr(mdia[x]);
	else return evaluateStr(mdia[x].substr(0, 15 - x)) + evaluateStr(mdia[x].substr(15 - x));
}
int evaluateSdia(int x) {
	if (x == 0) return evaluateStr(sdia[x].substr(1, 15));
	else return evaluateStr(sdia[x].substr(0, x)) + evaluateStr(sdia[x].substr(x));
}
int evaluatePos(int pos, char ch) {
	int ans = 0;
	char t;

	t = lline[pos / 15][pos % 15];
	lline[pos / 15][pos % 15] = ch;
	ans += evaluateStr(lline[pos / 15]);
	lline[pos / 15][pos % 15] = t;
	ans -= evaluateStr(lline[pos / 15]);

	t = rrow[pos % 15][pos / 15];
	rrow[pos % 15][pos / 15] = ch;
	ans += evaluateStr(rrow[pos % 15]);
	rrow[pos % 15][pos / 15] = t;
	ans -= evaluateStr(rrow[pos % 15]);

	t = mdia[pos % 16][pos / 16];
	mdia[pos % 16][pos / 16] = ch;
	ans += evaluateMdia(pos % 16);
	mdia[pos % 16][pos / 16] = t;
	ans -= evaluateMdia(pos % 16);

	t = sdia[pos % 14][pos / 14];
	sdia[pos % 14][pos / 14] = ch;
	ans += evaluateSdia(pos % 14);
	sdia[pos % 14][pos / 14] = t;
	ans -= evaluateSdia(pos % 14);

	return ans;
}
int negmax(char color, int alpha, int beta, int depth) {
	if (depth == MAX_DEP) {
		//printf("%s %d\n", Map, Now_Score);
		return evaluateBoard();
		//return 1;
	}
	//int cnt = 0;
	//Point* np = new Point[225];
	/*
	vector<Point> np;
	for (int i = 0; i < 225; i++) {
		if (Map[i] != '0') continue;
		int delta = -evaluatePos(i, color); //排序从大到小
		Point anewPoint = Point(i, delta, Fiveexist);
		np.push_back(anewPoint);
	}
	sort(np.begin(), np.end());
	int w = 1, js = 1;

	while (w < np.size()) {
		if (np[w - 1].v != np[w].v) js++;	
		w++;
		if (js == 10) break;
	}*/
	//if (depth == 0)
	/*
	vector<Point> np;
	for (int i = 0; i < 225; i++) {
		if (Map[i] != '0') continue;
		int delta = -evaluatePos(i, color); //排序从大到小
		Point anewPoint = Point(i, delta);
		np.push_back(anewPoint);
	}
	sort(np.begin(), np.end());
	*/
	for (int i = 0; i < 225; i++) {
		if (Map[i] != '0') continue;
		placeChess(i, color);
		int val = -negmax(xorColor(color), -beta, -alpha, depth + 1);
		placeChess(i, '0');


		if (val > alpha) {
			if (depth == 0) lastPos = i;
			if (val >= beta) {
				return beta;
			}
			alpha = val;
		}
	}

	return alpha;
}

int getNextMove(int pos){
	lastPos = 1;
	int val = -negmax((Ai_Color), -INF, INF, 0);

	return lastPos;
}
void placeChess(int x, char ch) {
	Map[x] = ch;

	Now_Score += evaluatePos(x, ch);

	lline[x / 15][x % 15] = ch;
	rrow[x % 15][x / 15] = ch;
	mdia[x % 16][x / 16] = ch;
	sdia[x % 14][x / 14] = ch;
}
