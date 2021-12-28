#include "MyFiveChess.h"
#include "UI.h"
#include "AssessSystem.h"
#define UNKNOWN_SCORE (10000001)
#define HASH_ITEM_INDEX_MASK (0xffff)

using namespace std;

int MAX_DEP = 7;
int MAX_TRY = 10;

int winner = 0, totSteps = 0;;

int totTime = 0;
int lastPos;

//保存棋局的哈希表条目


long long boardZobristValue[2][15][15];
long long currentZobristValue;
HashItem hashItems[HASH_ITEM_INDEX_MASK + 1];

AssessSystem::PossiblePositionClass npp;

int main() {
	printf("Powered by cyl.\n");
	printf("Initializating...");
	system("cls");
	

	initAI();
	

	while (!winner) {
		int player_fill;
		player_fill = UI::getClick(AssessSystem::getMap());
		AssessSystem::updateBoard(player_fill, '2');
		totSteps++;
		currentZobristValue ^= boardZobristValue[1][player_fill / 15][player_fill % 15];
		updatePlaced(player_fill, 0);

		  

		DWORD startTime = GetTickCount64();
		int AI_fill = getNextMove(player_fill);
		DWORD endTime = GetTickCount64();
		UI::printTime(endTime - startTime);

		AssessSystem::updateBoard(AI_fill, '1');
		totSteps++;
		currentZobristValue ^= boardZobristValue[0][AI_fill / 15][AI_fill % 15];
		if (winner != 2) updatePlaced(AI_fill, totSteps);

		totTime += endTime - startTime;
	}
	
	UI::gameOver(winner == 1);

	printf("总用时%d ms\n", totTime);
	system("pause");
	return 0;
}
void isAifirst(bool b) {
	
	if (!b) {
		UI::setAicolor('2');
		return;
	}
	else {
		totSteps = 1;
		UI::setAicolor('1');
		currentZobristValue ^= boardZobristValue[0][7][7];
		AssessSystem::updateBoard(112, ('1'));
		updatePlaced(112, 1);
	}
}
void setlevel(int x) {
	switch (x)
	{
	case 1:
		MAX_DEP = 4; break;
	case 2:
		MAX_DEP = 7; break;
	default:
		break;
	}
}
void initAI() {
	for (int i = 0; i < HASH_ITEM_INDEX_MASK + 1; i++) {
		hashItems[i].flag = HashItem::EMPTY;
	}
	randomBoardZobristValue();
	currentZobristValue = random64();
	AssessSystem::dfsStr("");

	UI::initGraph();
	int t1 = MessageBox(GetForegroundWindow(), L"您执黑？", L"新游戏", 1) - 1;
	setlevel(MessageBox(GetForegroundWindow(), L"简单难度？", L"难度设置", 1));
	isAifirst(t1);
} 

int negmax(char color, int alpha, int beta, int depth) {
	
	if (AssessSystem::getRoleColor(color) >= 50000) {
		return INF + MAX_DEP - (depth) + 1;
	}
	if (AssessSystem::getRoleColor(UI::xorColor(color)) >= 50000) {
		return -INF - MAX_DEP + (depth) - 1;
	}
	if (alpha > INF + MAX_DEP - (depth) ) {
		return alpha;
	}
	if (depth == MAX_DEP) {
		int t = AssessSystem::getRoleColor(color) - AssessSystem::getRoleColor(UI::xorColor(color));
		recordHashItem(depth, t, HashItem::EXACT);
		return t;
	}
	HashItem::Flag flag = HashItem::ALPHA;
	int sscore = getHashItemScore(depth, alpha, beta);
	if (sscore != UNKNOWN_SCORE && depth != 0) {
		return sscore;
	}
	

	int cnt = 0, lastVal = -1;
	set<UI::Position> PossiblePosition;
	const set<UI::Position>& tmpPossiblePositions = npp.GetCurrentPossiblePositions();

	for (set<UI::Position>::iterator iter = tmpPossiblePositions.begin(); iter != tmpPossiblePositions.end(); iter++)
	{
		int t = AssessSystem::getValue(UI::getNum(*iter));

		PossiblePosition.insert(UI::Position(iter->x, iter->y, t));
	}

	while(!PossiblePosition.empty()) {
		if (++cnt > MAX_TRY + 1) {
			if (lastVal != PossiblePosition.begin()->value)
				break;
		}

		int X = getNum(*PossiblePosition.begin()), val = alpha;
		lastVal = PossiblePosition.begin()->value;
		PossiblePosition.erase(PossiblePosition.begin());

		currentZobristValue ^= boardZobristValue[color - '1'][X / 15][X % 15];
		AssessSystem::updateBoard(X, color);
		npp.AddPossiblePosition(UI::getPos(X));

		//if (-negmax(UI::xorColor(color), -alpha - 1, -alpha, depth + 1) > alpha) {
			val = -negmax(UI::xorColor(color), -beta, -alpha, depth + 1);
		//}
		currentZobristValue ^= boardZobristValue[color - '1'][X / 15][X % 15];
		AssessSystem::updateBoard(X, '0');
		npp.Rollback();


		if (val > alpha) {
			flag = HashItem::EXACT;
			if (depth == 0) lastPos = X;
			if (val >= beta) {
				recordHashItem(depth, beta, HashItem::BETA);
				return beta;
			}
			alpha = val;
		}
		
		
	}
	recordHashItem(depth, alpha, flag);
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

//记录计算结果在哈希表中
void recordHashItem(int depth, int score, HashItem::Flag flag) {
	int index = (int)(currentZobristValue & HASH_ITEM_INDEX_MASK);
	HashItem* phashItem = &hashItems[index];

	if (phashItem->flag != HashItem::EMPTY && phashItem->depth > depth) {
		return;
	}

	phashItem->checksum = currentZobristValue;
	phashItem->score = score;
	phashItem->flag = flag;
	phashItem->depth = depth;
}


//在哈希表中取得计算好的局面的分数
int getHashItemScore(int depth, int alpha, int beta) {
	int index = (int)(currentZobristValue & HASH_ITEM_INDEX_MASK);
	HashItem* phashItem = &hashItems[index];

	if (phashItem->flag == HashItem::EMPTY)
		return UNKNOWN_SCORE;

	if (phashItem->checksum == currentZobristValue) {
		if (phashItem->depth <= depth) {
			if (phashItem->flag == HashItem::EXACT) {
				return phashItem->score;
			}
			if (phashItem->flag == HashItem::ALPHA && phashItem->score <= alpha) {
				return alpha;
			}
			if (phashItem->flag == HashItem::BETA && phashItem->score >= beta) {
				return beta;
			}
		}
	}

	return UNKNOWN_SCORE;
}

//生成64位随机数
long long random64() {
	return (long long)rand() | ((long long)rand() << 15) | ((long long)rand() << 30) | ((long long)rand() << 45) | ((long long)rand() << 60);
}

//生成zobrist键值
void randomBoardZobristValue() {
	int i, j, k;
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 15; j++) {
			for (k = 0; k < 15; k++) {
				boardZobristValue[i][j][k] = random64();
			}
		}
	}
}

//初始化初始局面的zobrist值
void initCurrentZobristValue() {
	currentZobristValue = random64();
}
