#include "AssessSystem.h"

namespace AssessSystem {
	string stringtable[16] =
	{ "11111", "011110", "011100", "001110", "011010", "010110", "11110", "01111", "11011", "10111", "11101", "001100", "001010", "010100", "000100", "001000" };

	int scoretable[16] =
	{ INF, 4320, 720, 720, 720, 720, 720, 720, 720, 720, 720, 120, 120, 120, 20, 20 };


	const int nearX[] = { 0, 0, 1, 1, 1, -1, -1, -1 };
	const int nearY[] = { 1, -1, 0, 1, -1, 0, 1, -1 };

	const int fx[] = { 0, 1, 1, 1 };
	const int fy[] = { 1, 0, 1, -1 };

	int allscore[2];

	string rrow[15], lline[15], mdia[16], sdia[16];

	map<string, int> existStr;

	void PossiblePositionClass::AddPossiblePosition(string Map, const UI::Position& pos) {
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
	
	int evaluateStr(string s) {
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
			for (int j = 0; j < s.size(); j++) s[j] = UI::xorColor(s[j]);

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

			for (int j = 0; j < s.size(); j++) s[j] = UI::xorColor(s[j]);
		}
		else sum = INF;


		existStr.insert(pair<string, int>(s, sum));
		return sum;
	}
	bool isappearFive(string s) {
		return s.find(stringtable[0], 0) != string::npos;
	}
	int evaluateMdia(int x) {
		if (x == 0 || x == 15) return evaluateStr(mdia[x]);
		else return evaluateStr(mdia[x].substr(0, 15 - x)) + evaluateStr(mdia[x].substr(15 - x));
	}
	int evaluateSdia(int x) {
		if (x == 0) return evaluateStr(sdia[x].substr(1, 15));
		else return evaluateStr(sdia[x].substr(0, x)) + evaluateStr(sdia[x].substr(x));
	}
	int evaluatePos(int pos, char ch) {
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

	int evaluateBoard() {
		int ans = 0;
		for (int i = 0; i < 15; i++) {
			int t = evaluateStr(lline[i]);
			if (abs(t) == INF) return t;
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
	void initAssessSys(char Ai_color) {
		for (int i = 0; i < 15; i++) lline[i].append(15, '0'), rrow[i].append(15, '0');
		for (int i = 1; i < 16; i++) mdia[i].append(14, '0');
		mdia[0].append(15, '0');

		for (int i = 1; i < 14; i++) sdia[i].append(16, '0');
		sdia[0].append(17, '0');

		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < stringtable[i].size(); j++) {
				if (stringtable[i][j] != '0') stringtable[i][j] = Ai_color;
			}
		}
	}

	int updateScore(int x, char ch) {
		int t = evaluatePos(x, ch);
		lline[x / 15][x % 15] = ch;
		rrow[x % 15][x / 15] = ch;
		mdia[x % 16][x / 16] = ch;
		sdia[x % 14][x / 14] = ch;

		return t;
	}
};
