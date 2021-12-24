#pragma once
#include <string>
#include <set>
#include "MyFiveChess.h"
using namespace std;

namespace AssessSystem {

	bool isappearFive(string s);
	int evaluateMdia(int x);
	int evaluateSdia(int x);

	int evaluateStr(string s);
	int evaluatePos(int pos, char ch);
	int evaluateBoard();
	void initAssessSys(char Ai_color);
	int updateScore(int x, char c);
	

	class PossiblePositionClass
	{
	public:
		void AddPossiblePosition(string Map,const UI::Position& pos);
		void Rollback();
		const set<UI::Position>& GetCurrentPossiblePositions();
	private:
		set<UI::Position> currentPossiblePositions;
		vector<HistoryItem> history;

	};
	
	bool isInBoard(int x, int y);

};