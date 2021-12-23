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
#include <map>
#include <set>

using namespace std;


#define GAP 49
#define CHESS_SIZE 43
#define LUPOINT 32
#define BG_SIZE 749

#define INF 247483647
#define MAX_DEP 6

char Ai_Color = '1';

IMAGE BOARD;
IMAGE WHITE_CHESS;
IMAGE BLACK_CHESS;

string rrow[15], lline[15], mdia[16], sdia[16];
string Map;

int isGameover = 0;

typedef struct Position {
	int x, y;
	int score;

	Position() {
		this->x = 0;
		this->y = 0;
		score = 0;
	}
	Position(int x, int y) {
		this->x = x;
		this->y = y;
		score = 0;
	}
	Position(int x, int y, int score) {
		this->x = x;
		this->y = y;
		this->score = score;
	}
	bool operator <(const Position& pos) const {
		if (score != pos.score) {
			return score > pos.score;
		}
		if (x != pos.x) {
			return x < pos.x;
		}
		else {
			return y < pos.y;
		}
	}

}Position;


struct HistoryItem {
	set<Position> addedPosition;
	Position removedPosition;
};

const int fx[] = { 0, 1, 1, 1 };
const int fy[] = { 1, 0, 1, -1 };

string stringtable[16] =
{ "11111", "011110", "011100", "001110", "011010", "010110", "11110", "01111", "11011", "10111", "11101", "001100", "001010", "010100", "000100", "001000" };

int scoretable[16] =
{ INF, 4320, 720, 720, 720, 720, 720, 720, 720, 720, 720, 120, 120, 120, 20, 20 };

int lastPos;
inline void drawAlpha(IMAGE* picture, int  picture_x, int picture_y); //x为要载入图片的X坐标，y为Y坐标
inline void initGraph();
inline void drawMap(int lastfill);
inline int getClick();
inline char xorColor(char ch);
inline int evaluateStr(string s);
inline int evaluatePos(int pos, char ch);
inline int negmax(char color, int alpha, int beta, int depth);
inline int getNextMove(int pos);
inline void setAi_Color(int x);
inline bool isappearFive(string s);
inline int evaluateMdia(int x);
inline int evaluateSdia(int x);
inline int placeChess(int x, char ch);
inline void initAI();
inline bool isInBoard(int x, int y);
inline Position getPos(int x);
inline int getNum(Position t);
inline void updateMap(int x);
inline int evaluateBoard();
inline void isAifirst(bool b);
const int nearX[] = { 0, 0, 1, 1, 1, -1, -1, -1};
const int nearY[] = { 1, -1, 0, 1, -1, 0, 1, -1};

map<string, int> existStr;

class PossiblePositionClass
{
public:
	void AddPossiblePosition(const Position& pos);
	void Rollback();
	const set<Position>& GetCurrentPossiblePositions();
private:
	set<Position> currentPossiblePositions;
	vector<HistoryItem> history;

};
PossiblePositionClass npp;

int main() {
	
	initAI();
	initGraph();
	
	
	isAifirst(false);
	
	
	/*
	int colorr = xorColor(Ai_Color);
	char Map1[] = "000000000000000000000000000000000000000010000000000000002000000000000021200000000000101020000000002210100000000211112000000000112210000000000022020000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	//Map1[143] = '0'; Map1[144] = '0'; Map1[146] = '0'; Map1[83] = '0'; Map1[40] = '0'; Map1[88] = '0';
	for (int i = 0; i < 225; i++) {
		if (Map1[i] == '0') continue;
		placeChess(i, Map1[i]);
		updateMap(i);
	}
	*/
	
		

	while (!isGameover) {
		int player_fill;
		player_fill = getClick();
		int deltaScore = placeChess(player_fill, xorColor(Ai_Color));
		updateMap(player_fill);

		if (deltaScore == -INF) {
			isGameover = xorColor(Ai_Color) - '0';
			break;
		}
		
		int AI_fill = getNextMove(player_fill);
		//int AI_fill = getClick();
		deltaScore = placeChess(AI_fill, (Ai_Color));
		updateMap(AI_fill);


		if (deltaScore ==  INF) {
			isGameover = Ai_Color - '0';
			break;
		}
	}
	
	

	if(isGameover == Ai_Color - '0') MessageBox(GetForegroundWindow(), L"Ai赢了", L"游戏结束", 1);
	else  MessageBox(GetForegroundWindow(), L"人类赢了", L"游戏结束", 1);
	closegraph();

	cout << Map << endl;
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
inline void isAifirst(bool b) {
	setAi_Color(1);
	if (!b) {
		setAi_Color(2);
		return;
	}
	else {
		setAi_Color(1);
		placeChess(112, (Ai_Color));
		updateMap(112);
	}
}

void PossiblePositionClass::AddPossiblePosition(const Position& pos) {
	set<Position> addingPositions;
	for (int i = 0; i < 8; i++) {
		if (!isInBoard(pos.x + nearX[i], pos.y + nearY[i])) {
			continue;
		}
		Position now(pos.x + nearX[i], pos.y + nearY[i]);
		if (Map[getNum(now)] == '0') {
			pair<set<Position>::iterator, bool> insertResult = currentPossiblePositions.insert(now);
			if (insertResult.second) {
				addingPositions.insert(now);
			}
		}
	}

	HistoryItem tHi;
	tHi.addedPosition = addingPositions;
	
	if (currentPossiblePositions.find(pos) != currentPossiblePositions.end()) {
		currentPossiblePositions.erase(pos);
		tHi.removedPosition = pos;
	}
	else {
		tHi.removedPosition.x = -1;
	}
	history.push_back(tHi);
}
void PossiblePositionClass::Rollback() {
	if (currentPossiblePositions.empty()) {
		return;
	}
	HistoryItem tHi = history[history.size() - 1];
	history.pop_back(); 

	for (set<Position>::iterator iter = tHi.addedPosition.begin(); iter != tHi.addedPosition.end(); iter++) {
		currentPossiblePositions.erase(*iter);
	}

	if (tHi.removedPosition.x != -1) {
		currentPossiblePositions.insert(tHi.removedPosition);
	}
}
const set<Position>& PossiblePositionClass::GetCurrentPossiblePositions() {
	return currentPossiblePositions;
}

inline bool isInBoard(int x, int y) {
	return (x >= 0 && x < 15 && y >= 0 && y < 15);
}
inline Position getPos(int x) {
	Position res(x / 15, x % 15);
	return res;
}
inline int getNum(Position t){
	return t.x * 15 + t.y;
}
inline void initAI() {
	Map.assign(255, '0');
	

	for (int i = 0; i < 15; i++) lline[i].append(15, '0'), rrow[i].append(15, '0');
	for (int i = 1; i < 16; i++) mdia[i].append(14, '0');
	mdia[0].append(15, '0');

	for (int i = 1; i < 14; i++) sdia[i].append(16, '0');
	sdia[0].append(17, '0');
}
inline char xorColor(char ch){
	if (ch == '0') return ch;
	else return '3' - ch + '0';
}
inline void initGraph() {
	initgraph(BG_SIZE, BG_SIZE);

	loadimage(&BOARD, L"board.jpg", BG_SIZE, BG_SIZE);
	putimage(0, 0, &BOARD);


	loadimage(&WHITE_CHESS, L"white_chess.png", CHESS_SIZE, CHESS_SIZE);
	//drawAlpha(&WHITE_CHESS, CHESS_SIZE, CHESS_SIZE);


	loadimage(&BLACK_CHESS, L"black_chess.png", CHESS_SIZE, CHESS_SIZE);
	//drawAlpha(&BLACK_CHESS, CHESS_SIZE, CHESS_SIZE);

	putimage(0, 0, &BOARD);

}
inline void drawMap(int lastfill) {
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
inline int getClick()//如果是黑 1  如果是白  2
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
inline void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //x为载入图片的X坐标，y为Y坐标
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


inline void setAi_Color(int x) {
	Ai_Color = x + '0';
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < stringtable[i].size(); j++) {
			if (stringtable[i][j] != '0') stringtable[i][j] = Ai_Color;
		}
	}
}

 
inline int evaluateBoard() {
	int ans = 0;
	for (int i = 0; i < 15; i++) {
		int t = evaluateStr(lline[i]);
		if (abs (t) == INF) return t;
		else ans += t;
	}
	for (int i = 0; i < 15; i++) {
		int t = evaluateStr(rrow[i]);
		if (abs(t) == INF) return t;
		else ans += t;
	}
	for (int i = 0; i < 16; i++) {
		int t = evaluateMdia(i);
		if (abs(t) == INF) return t;
		else ans += t;
	}	
	for (int i = 0; i < 14; i++) {
		int t = evaluateSdia(i);
		if (abs(t) == INF) return t;
		else ans += t;
	}	
	return ans;
}
inline int evaluateStr(string s){
	if (s.size() < 5) return 0;

	map<string, int>::iterator iter = existStr.find(s);
	if (iter != existStr.end()) {
		return iter->second;
	}

	int begin = -1;
	int sum = 0;

	if (!isappearFive(s)) {
		for (int i = 1; i < 16; i++) {
			int count = 0;
			while ((begin = s.find(stringtable[i], begin + 1)) != string::npos) {
				count++;
			}
			sum += count * scoretable[i];
		}
		for (int j = 0; j < s.size(); j++) s[j] = xorColor(s[j]);

		if (!isappearFive(s)) {
			for (int i = 1; i < 16; i++) {
				int count = 0;
				while ((begin = s.find(stringtable[i], begin + 1)) != string::npos) {
					count++;
				}
				sum -= count * scoretable[i];
			}
		}
		else sum = -INF;

		for (int j = 0; j < s.size(); j++) s[j] = xorColor(s[j]);
	}
	else sum = INF;
	
	
	existStr.insert(pair<string, int>(s, sum));
	return sum;
}
inline bool isappearFive(string s) {
	return s.find(stringtable[0], 0) != string::npos;
}
inline int evaluateMdia(int x) {
	if (x == 0 || x == 15) return evaluateStr(mdia[x]);
	else return evaluateStr(mdia[x].substr(0, 15 - x)) + evaluateStr(mdia[x].substr(15 - x));
}
inline int evaluateSdia(int x) {
	if (x == 0) return evaluateStr(sdia[x].substr(1, 15));
	else return evaluateStr(sdia[x].substr(0, x)) + evaluateStr(sdia[x].substr(x));
}
inline int evaluatePos(int pos, char ch) {
	int ans = 0, w;

	lline[pos / 15][pos % 15] = ch;
	w = evaluateStr(lline[pos / 15]);
	if (abs(w) == INF) {
		lline[pos / 15][pos % 15] = '0';
		return w;
	}
	else ans += w;

	lline[pos / 15][pos % 15] = '0';
	ans -= evaluateStr(lline[pos / 15]);

	rrow[pos % 15][pos / 15] = ch;
	w = evaluateStr(rrow[pos % 15]);
	if (abs(w) == INF) {
		rrow[pos % 15][pos / 15] = '0';
		return w;
	}
	else ans += w;
	rrow[pos % 15][pos / 15] = '0';
	ans -= evaluateStr(rrow[pos % 15]);

	mdia[pos % 16][pos / 16] = ch;
	w = evaluateMdia(pos % 16);
	if (abs(w) == INF) {
		mdia[pos % 16][pos / 16] = '0';
		return w;
	}
	else ans += w;
	mdia[pos % 16][pos / 16] = '0';
	ans -= evaluateMdia(pos % 16);

	sdia[pos % 14][pos / 14] = ch;
	w = evaluateSdia(pos % 14);
	if (abs(w) == INF) {
		sdia[pos % 14][pos / 14] = '0';
		return w;
	}
	else ans += w;
	sdia[pos % 14][pos / 14] = '0';
	ans -= evaluateSdia(pos % 14);

	return ans;
}
inline int negmax(char color, int alpha, int beta, int depth) {
	if (depth == MAX_DEP) {
		return evaluateBoard();
	}
	
	int cnt = 0;
	set<Position> PossiblePosition;
	const set<Position>& tmpPossiblePositions = npp.GetCurrentPossiblePositions();

	for (set<Position>::iterator iter = tmpPossiblePositions.begin(); iter != tmpPossiblePositions.end(); iter++) 
	{
		int t = evaluatePos(getNum(*iter), color) * (color == Ai_Color ? 1 : -1);
		if (t == INF) {return t;}
		PossiblePosition.insert(Position(iter->x, iter->y, t));
	}

	while(!PossiblePosition.empty()) {
		int X = getNum(*PossiblePosition.begin());
		PossiblePosition.erase(PossiblePosition.begin());

		placeChess(X, color);
		npp.AddPossiblePosition(getPos(X));

		int val = -negmax(xorColor(color), -beta, -alpha, depth + 1);

		placeChess(X, '0');
		npp.Rollback();


		if (val > alpha) {
			if (depth == 0) lastPos = X;
			if (val >= beta) {
				return beta;
			}
			alpha = val;
		}
		if (cnt++ > 10) {
			break;
		}
	}

	return alpha;
}

inline int getNextMove(int pos){
	lastPos = 1;
	int val = -negmax((Ai_Color), -INF+1, INF-1, 0);

	return lastPos;
}
inline int placeChess(int x, char ch) {
	Map[x] = ch;

	int t = evaluatePos(x, ch);

	lline[x / 15][x % 15] = ch;
	rrow[x % 15][x / 15] = ch;
	mdia[x % 16][x / 16] = ch;
	sdia[x % 14][x / 14] = ch;
	
	return t;
}
void updateMap(int x) {
	drawMap(x);
	npp.AddPossiblePosition(getPos(x));
}

//int cnt = 0;
	//Point* np = new Point[225];

	/*vector<Position> np;
	for (int i = 0; i < 225; i++) {
		if (Map[i] != '0') continue;
		int delta = -evaluatePos(i, color); //排序从大到小
		Position anewPoint = getPos(i);
		np.push_back(anewPoint);
	}
	sort(np.begin(), np.end());
	/*int w = 1, js = 1;

	while (w < np.size()) {
		if (np[w - 1].v != np[w].v) js++;
		w++;
		if (js == 10) break;
	}
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
/*int totsteps = 1;
	Position sr[15];
	sr[1] = Position(8, 8);
	sr[2] = Position(7, 6);
	sr[3] = Position(6, 8);
	sr[4] = Position(9, 7);
	sr[5] = Position(6, 9);
	sr[6] = Position(7, 11);
	sr[7] = Position(8, 9);
	sr[8] = Position(4, 10);
	sr[9] = Position(4, 12);
	sr[10] = Position(9, 10);
	sr[11] = Position(3, 11);
	sr[12] = Position(5, 13);
	sr[13] = Position(9, 8);*/