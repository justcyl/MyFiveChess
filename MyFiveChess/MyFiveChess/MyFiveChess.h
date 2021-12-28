#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <cstdio>
#include <iostream>
#include <time.h>
#include <windows.h>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <stack>
#include <cassert>
#include "UI.h"
#define INF 50000000


using namespace std;

struct HashItem {
	long long checksum;
	int depth;
	int score;
	enum Flag { ALPHA = 0, BETA = 1, EXACT = 2, EMPTY = 3 } flag;
};

struct HistoryItem {
	set<UI::Position> addedPosition;
	UI::Position removedPosition;
};

int negmax(char color, int alpha, int beta, int depth);
int getNextMove(int pos);

void updatePlaced(int x, int steps);

void initAI();
void setlevel(int x);
void isAifirst(bool b);

void recordHashItem(int depth, int score, HashItem::Flag flag);
int getHashItemScore(int depth, int alpha, int beta);
long long random64();
void randomBoardZobristValue();
void initCurrentZobristValue();