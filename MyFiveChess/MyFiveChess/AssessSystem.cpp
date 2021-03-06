#include "AssessSystem.h"

namespace AssessSystem {
	string Map(255, '0');
	string stringtable[16] =
	{ "11111", "011110", "011100", "001110", "011010", "010110", "11110", "01111", "11011", "10111", "11101", "001100", "001010", "010100", "000100", "001000" };

	int scoretable[16] =
	{ 50000, 4320, 720, 720, 720, 720, 720, 720, 720, 720, 720, 120, 120, 120, 20, 20 };


	const int nearX[] = { 0, 0, 1, 1, 1, -1, -1, -1 };
	const int nearY[] = { 1, -1, 0, 1, -1, 0, 1, -1 };

	const int fx[] = { 0, 1, 1, 1 };
	const int fy[] = { 1, 0, 1, -1 };

	int allScore[2];

	int lastScore[2][225] = {0};

	map<string, int> existStr;

	void PossiblePositionClass::AddPossiblePosition(const UI::Position& pos) {
		set<UI::Position> addingPositions;
		for (int i = 0; i < 8; i++) {
			if (!isInBoard(pos.x + nearX[i], pos.y + nearY[i])) {
				continue;
			}
			UI::Position now(pos.x + nearX[i], pos.y + nearY[i]);
			if (Map[UI::getNum(now)] == '0') {
				pair<set<UI::Position>::iterator, bool> insertResult = currentPossiblePositions.insert(now);
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

		for (set<UI::Position>::iterator iter = tHi.addedPosition.begin(); iter != tHi.addedPosition.end(); iter++) {
			currentPossiblePositions.erase(*iter);
		}

		if (tHi.removedPosition.x != -1) {
			currentPossiblePositions.insert(tHi.removedPosition);
		}
	}
	const set<UI::Position>& PossiblePositionClass::GetCurrentPossiblePositions() {
		return currentPossiblePositions;
	}

	bool isInBoard(int x, int y) {
		return (x >= 0 && x < 15 && y >= 0 && y < 15);
	}
	
	string getMap() {
		return Map;
	}
	int evaluateStr(string s) {
		if (s.size() < 5) return 0;

		map<string, int>::iterator iter = existStr.find(s);
		if (iter != existStr.end()) {
			return iter->second;
		}
		int begin = -1;
		int sum = 0;

		for (int i = 0; i < 16; i++) {
			int count = 0;
			while ((begin = s.find(stringtable[i], begin + 1)) != string::npos) {
				count++;
			}
			sum += count * scoretable[i];
		}
		existStr.insert(pair<string, int>(s, sum));
		return sum;
	}


	int getRoleColor(char c) {
		return allScore[c - '0' - 1];
	}

	char isRole(char x, char y) {
		if (x == '0') {
			return x;
		}
		else return x == y ? '1' : '2';
	}

	int evaluatePos(int x, char t, char role, int ss) {
		int result = 0;

		UI::Position p = UI::getPos(x);


		string l0[4];

		char oldC = Map[x];
		Map[x] = t;

		for (int i = max(0, p.x - ss); i <= min(14, p.x + ss); i++) {
			l0[0].push_back(isRole(Map[i * 15 + p.y], role));
		}
		for (int j = max(0, p.y - ss); j <= min(14, p.y + ss); j++) {
			l0[1].push_back(isRole(Map[p.x * 15 + j], role));
		}
		for (int i = p.x - min(min(p.x, p.y), ss), j = p.y - min(min(p.x, p.y), ss); i <= min(15, p.x + ss) && j <= min(14, p.y + ss); i++, j++) {
			l0[2].push_back(isRole(Map[i * 15 + j], role));
		}
		for (int i = p.x + min(min(p.y, 15 - 1 - p.x), ss), j = p.y - min(min(p.y, 15 - 1 - p.x), ss); i >= max(0, p.x - ss) && j <= min(14, p.y + ss); i--, j++) {
			l0[3].push_back(isRole(Map[i * 15 + j], role));
		}
		for (int i = 0; i < 4; i++) {
			result += evaluateStr(l0[i]);
		}

		Map[x] = oldC;
		return result;
	}

	int getValue(int x) {
		return evaluatePos(x, '1', '1', 5) + evaluatePos(x, '2', '2', 5);
	}

	void updateBoard(int x, char ch) {
		int t1 = evaluatePos(x, ch, '1', 15);
		allScore[0] += t1 - evaluatePos(x, Map[x], '1', 15);
		lastScore[0][x] = t1;

		int t2 = evaluatePos(x, ch, '2', 15);
		allScore[1] += t2 - evaluatePos(x, Map[x], '2', 15);
		lastScore[1][x] = t2;

		Map[x] = ch;
	}
	void dfsStr(string s) {
		if (s.size() > 11) return;
		if (s.size() > 4) {
			int begin = -1;
			int sum = 0;

			for (int i = 0; i < 16; i++) {
				int count = 0;
				while ((begin = s.find(stringtable[i], begin + 1)) != string::npos) {
					count++;
				}
				sum += count * scoretable[i];
			}
			existStr.insert(pair<string, int>(s, sum));
		}
		

		for (char ch = '0'; ch <= '2'; ch++) {
			s += ch;
			dfsStr(s);
			s.pop_back();
		}
	}
};