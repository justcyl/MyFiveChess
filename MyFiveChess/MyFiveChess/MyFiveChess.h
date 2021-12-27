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
#include "UI.h"
#define INF 50000000


using namespace std;

int negmax(char color, int alpha, int beta, int depth);
int getNextMove(int pos);

void updatePlaced(int x, int steps);

void initAI();
void setlevel(int x);
void isAifirst(bool b);

struct HistoryItem {
	set<UI::Position> addedPosition;
	UI::Position removedPosition;
};