﻿#include "MyFiveChess.h"
#include "UI.h"
#include "AssessSystem.h"

using namespace std;


int MAX_TRY = 10;

int winner = 0;

int lastPos;

AssessSystem::PossiblePositionClass npp;

int main() {

	UI::initGraph();
	initAI();

	int totSteps = 0;

	while (!winner) {
		int player_fill;
		player_fill = UI::getClick(AssessSystem::getMap());
		AssessSystem::updateBoard(player_fill, '2');
		totSteps++;
		updatePlaced(player_fill, 0);

		if (winner) break;

		DWORD startTime = GetTickCount64();
		int AI_fill = getNextMove(player_fill);
		DWORD endTime = GetTickCount64();
		UI::printTime(endTime - startTime);
		AssessSystem::updateBoard(AI_fill, '1');
		totSteps++;
		if (winner != 2) updatePlaced(AI_fill, totSteps);
		
	}
	
	UI::gameOver(winner == 1);

	

	return 0;
}
void isAifirst(bool b) {

	if (!b) {
		UI::setAicolor('2');
		return;
	}
	else {
		UI::setAicolor('1');
		AssessSystem::updateBoard(112, ('1'));
		updatePlaced(112, 1);
	}
}
void initAI() {
	isAifirst(MessageBox(GetForegroundWindow(), L"您执黑？", L"新游戏", 1) - 1);
} 

int negmax(char color, int alpha, int beta, int depth) {
	if (AssessSystem::getRoleColor(color) >= 50000) {
		return INF + MAX_DEP - (depth)+1;
	}
	if (AssessSystem::getRoleColor(UI::xorColor(color)) >= 50000) {
		return -INF - MAX_DEP + (depth)-1;
	}
	if (depth == MAX_DEP) {
		return AssessSystem::getRoleColor(color) - AssessSystem::getRoleColor(UI::xorColor(color));
	}
	
	int cnt = 0;
	set<UI::Position> PossiblePosition;
	const set<UI::Position>& tmpPossiblePositions = npp.GetCurrentPossiblePositions();

	for (set<UI::Position>::iterator iter = tmpPossiblePositions.begin(); iter != tmpPossiblePositions.end(); iter++)
	{
		int t = AssessSystem::getValue(UI::getNum(*iter));

		PossiblePosition.insert(UI::Position(iter->x, iter->y, t));
	}

	while(!PossiblePosition.empty()) {
		int X = getNum(*PossiblePosition.begin());
		PossiblePosition.erase(PossiblePosition.begin());

		AssessSystem::updateBoard(X, color);
		npp.AddPossiblePosition(UI::getPos(X));

		int val = -negmax(UI::xorColor(color), -beta, -alpha, depth + 1);

		AssessSystem::updateBoard(X, '0');
		npp.Rollback();


		if (val > alpha) {
			if (depth == 0) lastPos = X;
			if (val >= beta) {
				return beta;
			}
			alpha = val;
		}
		if (cnt++ > MAX_TRY) {
			break;
		}
	}

	return alpha;
}

int getNextMove(int pos){
	lastPos = 1;
	int val = negmax('1', -INF - MAX_DEP - 1, INF + MAX_DEP + 1, 0);
	if (val == INF + MAX_DEP) {
		winner = 1;
	}
	else if (val == -INF - MAX_DEP - 1) {
		winner = 2;
	}
	UI::printVal(val);
	return lastPos;
}

void updatePlaced(int lastFill, int steps) {
	UI::drawMap(AssessSystem::getMap(), lastFill, steps);
	npp.AddPossiblePosition(UI::getPos(lastFill));
}
