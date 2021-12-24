#include "MyFiveChess.h"
#include "UI.h"
#include "AssessSystem.h"

using namespace std;

string Map;


int MAX_TRY = 10;

char Ai_Color;

int winner = 0;

int lastPos;

AssessSystem::PossiblePositionClass npp;

int negmax(char color, int alpha, int beta, int depth);
int getNextMove(int pos);

int placeChess(int x, char ch);
void updateMap(int x);

void initAI();
void isAifirst(bool b);


void debug(bool b) {
	if (!b) return;
	char map1[] = "020000000000000000000000000000000000000000000000000000000000000000000111000000000002100000010021221100000020101211200000021212111200000021212200000000122221000000000010222100000000002110200000000010000010000000000000000000000000000000000000000000000000000";
	map1[UI::getNum(UI::Position(0, 1))] = '0'; map1[UI::getNum(UI::Position(4, 10))] = '0'; map1[UI::getNum(UI::Position(4, 11))] = '0'; map1[UI::getNum(UI::Position(6, 8))] = '0';
	for (int i = 0; i < 225; i++) {
		if (map1[i] == '0') continue;
		int t = placeChess(i, map1[i]);
		updateMap(i);
	}
} 

int main() {

	UI::initGraph();
	initAI();
	
	debug(false);
	

	//int totSteps = 0;
	int deltaScore;

	while (!winner) {
		int player_fill;
		player_fill = UI::getClick(Map);
		deltaScore = placeChess(player_fill, UI::xorColor(Ai_Color));
		updateMap(player_fill);

		if (winner) break;

		int AI_fill = getNextMove(player_fill);
		deltaScore = placeChess(AI_fill, (Ai_Color));
		updateMap(AI_fill);


		/*
		if (totSteps > MAX_TRY) {
			MAX_TRY += 10;
		}*/
	}
	
	UI::gameOver(winner == Ai_Color - '0');

	

	return 0;
}
void isAifirst(bool b) {

	if (!b) {
		AssessSystem::initAssessSys('2');
		Ai_Color = '2';
		return;
	}
	else {
		AssessSystem::initAssessSys('1');
		Ai_Color = '1';
		placeChess(112, (Ai_Color));
		updateMap(112);
	}
}
void initAI() {
	Map.assign(255, '0'); 

	isAifirst(MessageBox(GetForegroundWindow(), L"您执黑？", L"新游戏", 1) - 1);
} 

inline int negmax(char color, int alpha, int beta, int depth) {
	if (depth == MAX_DEP) {
		return AssessSystem::evaluateBoard();
	}
	
	int cnt = 0;
	set<UI::Position> PossiblePosition;
	const set<UI::Position>& tmpPossiblePositions = npp.GetCurrentPossiblePositions();

	for (set<UI::Position>::iterator iter = tmpPossiblePositions.begin(); iter != tmpPossiblePositions.end(); iter++)
	{
		int t = AssessSystem::evaluatePos(UI::getNum(*iter), color) * (color == Ai_Color ? 1 : -1);
		if (t == INF) {
			if (depth == 0) lastPos = getNum(*iter);
			return t + MAX_DEP - depth;
		}
		PossiblePosition.insert(UI::Position(iter->x, iter->y, t));
	}

	while(!PossiblePosition.empty()) {
		int X = getNum(*PossiblePosition.begin());
		PossiblePosition.erase(PossiblePosition.begin());

		placeChess(X, color);
		npp.AddPossiblePosition(Map, UI::getPos(X));

		int val = -negmax(UI::xorColor(color), -beta, -alpha, depth + 1);

		placeChess(X, '0');
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
	int val = -negmax((Ai_Color), -INF - MAX_DEP - 1, INF + MAX_DEP + 1, 0);
	return lastPos;
}
int placeChess(int x, char ch) {
	Map[x] = ch;
	int t =  AssessSystem::updateScore(x, ch);

	if (t == INF) winner = Ai_Color - '0';
	else if (t == -INF) winner = UI::xorColor(Ai_Color) - '0';
	else winner = 0;

	return t;
}
void updateMap(int x) {
	UI::drawMap(Map, x);
	npp.AddPossiblePosition(Map, UI::getPos(x));
}