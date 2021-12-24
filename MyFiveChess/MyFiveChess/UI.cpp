#pragma once
#include "UI.h"
#include <string>
#include <cstdio>
using namespace std;

const int GAP = 49;
const int CHESS_SIZE = 45;
const int LUPOINT = 32;
const int BG_SIZE = 749;


IMAGE BOARD;
IMAGE WHITE_CHESS;
IMAGE BLACK_CHESS;

namespace UI {
	void initGraph() {
		initgraph(BG_SIZE, BG_SIZE);

		loadimage(&BOARD, L"board.jpg", BG_SIZE, BG_SIZE);
		putimage(0, 0, &BOARD);


		loadimage(&WHITE_CHESS, L"white_chess.png", CHESS_SIZE, CHESS_SIZE);


		loadimage(&BLACK_CHESS, L"black_chess.png", CHESS_SIZE, CHESS_SIZE);

		putimage(0, 0, &BOARD);

	}
	void drawMap(string Map, int lastfill) {
		BeginBatchDraw();
		putimage(0, 0, &BOARD);
		for (int i = 0; i < 225; i++) {
			int px = (i / 15) * GAP + LUPOINT, py = (i % 15) * GAP + LUPOINT;
			switch (Map[i])
			{
			case'1':
				drawAlpha(&BLACK_CHESS, py - CHESS_SIZE / 2, px - CHESS_SIZE / 2); break;
			case'2':
				drawAlpha(&WHITE_CHESS, py - CHESS_SIZE / 2, px - CHESS_SIZE / 2); break;
			default:
				break;
			}
		}
		setcolor(0x00FF00);
		rectangle((lastfill % 15) * GAP + LUPOINT - CHESS_SIZE / 2 - 2, (lastfill / 15) * GAP + LUPOINT - CHESS_SIZE / 2 - 2, (lastfill % 15) * GAP + LUPOINT + CHESS_SIZE / 2 + 2, (lastfill / 15) * GAP + LUPOINT + CHESS_SIZE / 2 + 2);

		EndBatchDraw();
	}
	int getClick(string Map)//如果是黑 1  如果是白  2
	{
		MOUSEMSG msg;
		while (1)
		{
			msg = GetMouseMsg();
			switch (msg.uMsg)
			{
			case WM_LBUTTONDOWN:
				//    msg.y/SIZE-->行下标
				//    msg.x/SIZE-->列下标
				int cx = round((double)(msg.y - LUPOINT) / (double)GAP), cy = round((double)(msg.x - LUPOINT) / (double)GAP);
				if (cx < 0 || cy < 0) continue;
				if (cx > 14 || cy > 14) continue;
				if (abs(cx * GAP + LUPOINT - msg.y) > CHESS_SIZE / 2) continue;
				if (abs(cy * GAP + LUPOINT - msg.x) > CHESS_SIZE / 2) continue;

				if (Map[cx * 15 + cy] == '0')
				{
					//Map[cx * 15 + cy] =(char)( xorColor(Ai_Color));
					return cx * 15 + cy;//返回点击的位置
				}

				break;
			}
		}
	}
	void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //x为载入图片的X坐标，y为Y坐标
	{

		// 变量初始化
		DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
		DWORD* draw = GetImageBuffer();
		DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
		int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
		int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
		int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
		int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
		int dstX = 0;    //在显存里像素的角标

		// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
		for (int iy = 0; iy < picture_height; iy++)
		{
			for (int ix = 0; ix < picture_width; ix++)
			{
				int srcX = ix + iy * picture_width; //在显存里像素的角标
				int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
				int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
				int sg = ((src[srcX] & 0xff00) >> 8);   //G
				int sb = src[srcX] & 0xff;              //B
				if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
				{
					dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
					int dr = ((dst[dstX] & 0xff0000) >> 16);
					int dg = ((dst[dstX] & 0xff00) >> 8);
					int db = dst[dstX] & 0xff;
					draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
						| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
						| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
				}
			}
		}
	}
	void gameOver(bool isAiwin) {
		if (isAiwin) MessageBox(GetForegroundWindow(), L"Ai赢了", L"游戏结束", 1);
		else  MessageBox(GetForegroundWindow(), L"人类赢了", L"游戏结束", 1);
		closegraph();

		//cout << Map << endl;
	}
	int getNum(Position t) {
		return t.x * 15 + t.y;
	}
	Position getPos(int x) {
		Position res(x / 15, x % 15);
		return res;
	}
	char xorColor(char ch) {
		if (ch == '0') return ch;
		else return '3' - ch + '0';
	}
};
