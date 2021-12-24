#pragma once
#include <time.h>
#include <easyx.h>
#include <conio.h>
#include <graphics.h> 
#include <windows.h>
#include <string>

using namespace std;

namespace UI {
	typedef struct Position {
		int x, y;
		int score;

		Position() {
			this->x = 0;
			this->y = 0;
			score = 0;
		}
		Position(int x, int y) {
			this->x = x;
			this->y = y;
			score = 0;
		}
		Position(int x, int y, int score) {
			this->x = x;
			this->y = y;
			this->score = score;
		}
		bool operator <(const Position& pos) const {
			if (score != pos.score) {
				return score > pos.score;
			}
			if (x != pos.x) {
				return x < pos.x;
			}
			else {
				return y < pos.y;
			}
		}

	}Position;

	Position getPos(int x);
	void initGraph();
	int getClick(string Map);
	void drawAlpha(IMAGE* picture, int  picture_x, int picture_y); //x为要载入图片的X坐标，y为Y坐标
	void drawMap(string Map, int lastfill);
	void gameOver(bool isAiwin);
	int getNum(Position t);
	char xorColor(char ch);
};
