#pragma once
#include <string>
#include <set>
#include "MyFiveChess.h"
using namespace std;

namespace AssessSystem {

	int evaluateStr(string str);
	int getRoleColor(char c);
	void updateBoard(int x, char c);
	char isRole(char x, char y);
	int evaluatePos(int x, char c, char role, int ss);
	int getValue(int x);
	string getMap();

	class PossiblePositionClass
	{
	public:
		void AddPossiblePosition(const UI::Position& pos);
		void Rollback();
		const set<UI::Position>& GetCurrentPossiblePositions();
	private:
		set<UI::Position> currentPossiblePositions;
		vector<HistoryItem> history;

	};
	void dfsStr(string s);
	bool isInBoard(int x, int y);

};