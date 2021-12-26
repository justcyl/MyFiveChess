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
#define MAX_DEP 7

using namespace std;

struct HistoryItem {
	set<UI::Position> addedPosition;
	UI::Position removedPosition;
};