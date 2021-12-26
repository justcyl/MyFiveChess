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
		int value;

		Position() {
			this->x = 0;
			this->y = 0;
			value = 0;
		}
		Position(int x, int y) {
			this->x = x;
			this->y = y;
			value = 0;
		}
		Position(int x, int y, int value) {
			this->x = x;
			this->y = y;
			this->value = value;
		}
		bool operator <(const Position& pos) const {
			if (value != pos.value) {
				return value > pos.value;
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
	void drawMap(string Map, int lastfill, int steps);
	void gameOver(bool isAiwin);
	int getNum(Position t);
	char xorColor(char ch);
	void setAicolor(char c);
	void printVal(int val);
	void printTime(int ti);
	string Myitoa(int x);
};
