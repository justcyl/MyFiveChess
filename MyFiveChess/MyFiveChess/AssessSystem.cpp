#include "AssessSystem.h"

namespace AssessSystem {
	string Map(255, '0');
	string stringtable[16] =
	{ "11111", "011110", "011100", "001110", "011010", "010110", "11110", "01111", "11011", "10111", "11101", "001100", "001010", "010100", "000100", "001000" };

	int scoretable[16] =
	{ 50000, 4320, 720, 720, 720, 720, 720, 720, 720, 720, 720, 120, 120, 120, 20, 20 };

	int  scores[2][72];

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
		if (c == '1') return allScore[0];
		else return allScore[1];
	}

	char isRole(char x, char y) {
		if (x == '0') {
			return x;
		}
		else return x == y ? '1' : '2';
	}

	int evaluatePoint(int x) {
		int result;
		int i, j;
		int role;

		UI::Position p = UI::getPos(x);

		result = 0;
		role = '1';


		string lines[4];
		string lines1[4];
		for (i = max(0, p.x - 5); i < min(15, p.x + 6); i++) {
			if (i != p.x) {
				lines[0].push_back(Map[i * 15 + p.y] == role ? '1' : Map[i * 15 + p.y] == '0' ? '0' : '2');
				lines1[0].push_back(Map[i * 15 + p.y] == role ? '2' : Map[i * 15 + p.y] == '0' ? '0' : '1');
			}
			else {
				lines[0].push_back('1');
				lines1[0].push_back('1');
			}
		}
		for (j = max(0, p.y - 5); j < min(15, p.y + 6); j++) {
			if (j != p.y) {
				lines[1].push_back(Map[p.x * 15 + j] == role ? '1' : Map[p.x * 15 + j] == '0' ? '0' : '2');
				lines1[1].push_back(Map[p.x * 15 + j] == role ? '2' : Map[p.x * 15 + j] == '0' ? '0' : '1');
			}
			else {
				lines[1].push_back('1');
				lines1[1].push_back('1');
			}
		}
		for (i = p.x - min(min(p.x, p.y), 5), j = p.y - min(min(p.x, p.y), 5); i < min(15, p.x + 6) && j < min(15, p.y + 6); i++, j++) {
			if (i != p.x) {
				lines[2].push_back(Map[i * 15 + j] == role ? '1' : Map[i * 15 + j] == '0' ? '0' : '2');
				lines1[2].push_back(Map[i * 15 + j] == role ? '2' : Map[i * 15 + j] == '0' ? '0' : '1');
			}
			else {
				lines[2].push_back('1');
				lines1[2].push_back('1');
			}
		}
		for (i = p.x + min(min(p.y, 15 - 1 - p.x), 5), j = p.y - min(min(p.y, 15 - 1 - p.x), 5); i >= max(0, p.x - 5) && j < min(15, p.y + 6); i--, j++) {
			if (i != p.x) {
				lines[3].push_back(Map[i * 15 + j] == role ? '1' : Map[i * 15 + j] == '0' ? '0' : '2');
				lines1[3].push_back(Map[i * 15 + j] == role ? '2' : Map[i * 15 + j] == '0' ? '0' : '1');
			}
			else {
				lines[3].push_back('1');
				lines1[3].push_back('1');
			}
		}

		for (i = 0; i < 4; i++) {
			result += evaluateStr(lines[i]) + evaluateStr(lines1[i]);
		}

		return result;
	}

	int getValue(int x) {
		return evaluatePoint(x);
	}

	void updateBoard(int x, char ch) {
		string lines[4];
		string lines1[4];
		int i, j;
		char role = '1';

		UI::Position p = UI::getPos(x);


		Map[x] = ch;


		//竖
		for (i = 0; i < 15; i++) {

			lines[0].push_back(Map[i * 15 + p.y] == role ? '1' : Map[i * 15 + p.y] == '0' ? '0' : '2');
			lines1[0].push_back(Map[i * 15 + p.y] == role ? '2' : Map[i * 15 + p.y] == '0' ? '0' : '1');


		}
		//横
		for (i = 0; i < 15; i++) {

			lines[1].push_back(Map[p.x * 15 + i] == role ? '1' : Map[p.x * 15 + i] == '0' ? '0' : '2');
			lines1[1].push_back(Map[p.x * 15 + i] == role ? '2' : Map[p.x * 15 + i] == '0' ? '0' : '1');

		}
		//反斜杠
		for (i = p.x - min(p.x, p.y), j = p.y - min(p.x, p.y); i < 15 && j < 15; i++, j++) {

			lines[2].push_back(Map[i * 15 + j] == role ? '1' : Map[i * 15 + j] == '0' ? '0' : '2');
			lines1[2].push_back(Map[i * 15 + j] == role ? '2' : Map[i * 15 + j] == '0' ? '0' : '1');

		}
		//斜杠
		for (i = p.x + min(p.y, 15 - 1 - p.x), j = p.y - min(p.y, 15 - 1 - p.x); i >= 0 && j < 15; i--, j++) {

			lines[3].push_back(Map[i * 15 + j] == role ? '1' : Map[i * 15 + j] == '0' ? '0' : '2');
			lines1[3].push_back(Map[i * 15 + j] == role ? '2' : Map[i * 15 + j] == '0' ? '0' : '1');

		}

		int lineScore[4];
		int line1Score[4];
		memset(lineScore, 0, sizeof(lineScore));
		memset(line1Score, 0, sizeof(line1Score));

		//计算分数
		for (i = 0; i < 4; i++) {
			lineScore[i] += evaluateStr(lines[i]);
			line1Score[i] += evaluateStr(lines1[i]);
		}

		int a = p.y;
		int b = 15 + p.x;
		int c = 2 * 15 + (p.y - p.x + 10);
		int d = 2 * 15 + 21 + (p.x + p.y - 4);
		//减去以前的记录
		for (i = 0; i < 2; i++) {
			allScore[i] -= scores[i][a];
			allScore[i] -= scores[i][b];
		}

		//scores顺序 竖、横、\、/
		scores[0][a] = lineScore[0];
		scores[1][a] = line1Score[0];
		scores[0][b] = lineScore[1];
		scores[1][b] = line1Score[1];


		//加上新的记录
		for (i = 0; i < 2; i++) {
			allScore[i] += scores[i][a];
			allScore[i] += scores[i][b];
		}

		if (p.y - p.x >= -10 && p.y - p.x <= 10) {

			for (i = 0; i < 2; i++)
				allScore[i] -= scores[i][c];


			scores[0][c] = lineScore[2];
			scores[1][c] = line1Score[2];

			for (i = 0; i < 2; i++)
				allScore[i] += scores[i][c];

		}

		if (p.x + p.y >= 4 && p.x + p.y <= 24) {

			for (i = 0; i < 2; i++)
				allScore[i] -= scores[i][d];

			scores[0][d] = lineScore[3];
			scores[1][d] = line1Score[3];

			for (i = 0; i < 2; i++)
				allScore[i] += scores[i][d];
		}

		
		
	}
};