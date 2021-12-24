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
	int getClick(string Map)//����Ǻ� 1  ����ǰ�  2
	{
		MOUSEMSG msg;
		while (1)
		{
			msg = GetMouseMsg();
			switch (msg.uMsg)
			{
			case WM_LBUTTONDOWN:
				//    msg.y/SIZE-->���±�
				//    msg.x/SIZE-->���±�
				int cx = round((double)(msg.y - LUPOINT) / (double)GAP), cy = round((double)(msg.x - LUPOINT) / (double)GAP);
				if (cx < 0 || cy < 0) continue;
				if (cx > 14 || cy > 14) continue;
				if (abs(cx * GAP + LUPOINT - msg.y) > CHESS_SIZE / 2) continue;
				if (abs(cy * GAP + LUPOINT - msg.x) > CHESS_SIZE / 2) continue;

				if (Map[cx * 15 + cy] == '0')
				{
					//Map[cx * 15 + cy] =(char)( xorColor(Ai_Color));
					return cx * 15 + cy;//���ص����λ��
				}

				break;
			}
		}
	}
	void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //xΪ����ͼƬ��X���꣬yΪY����
	{

		// ������ʼ��
		DWORD* dst = GetImageBuffer();    // GetImageBuffer()���������ڻ�ȡ��ͼ�豸���Դ�ָ�룬EASYX�Դ�
		DWORD* draw = GetImageBuffer();
		DWORD* src = GetImageBuffer(picture); //��ȡpicture���Դ�ָ��
		int picture_width = picture->getwidth(); //��ȡpicture�Ŀ�ȣ�EASYX�Դ�
		int picture_height = picture->getheight(); //��ȡpicture�ĸ߶ȣ�EASYX�Դ�
		int graphWidth = getwidth();       //��ȡ��ͼ���Ŀ�ȣ�EASYX�Դ�
		int graphHeight = getheight();     //��ȡ��ͼ���ĸ߶ȣ�EASYX�Դ�
		int dstX = 0;    //���Դ������صĽǱ�

		// ʵ��͸����ͼ ��ʽ�� Cp=��p*FP+(1-��p)*BP �� ��Ҷ˹���������е���ɫ�ĸ��ʼ���
		for (int iy = 0; iy < picture_height; iy++)
		{
			for (int ix = 0; ix < picture_width; ix++)
			{
				int srcX = ix + iy * picture_width; //���Դ������صĽǱ�
				int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA��͸����
				int sr = ((src[srcX] & 0xff0000) >> 16); //��ȡRGB���R
				int sg = ((src[srcX] & 0xff00) >> 8);   //G
				int sb = src[srcX] & 0xff;              //B
				if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
				{
					dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //���Դ������صĽǱ�
					int dr = ((dst[dstX] & 0xff0000) >> 16);
					int dg = ((dst[dstX] & 0xff00) >> 8);
					int db = dst[dstX] & 0xff;
					draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //��ʽ�� Cp=��p*FP+(1-��p)*BP  �� ��p=sa/255 , FP=sr , BP=dr
						| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //��p=sa/255 , FP=sg , BP=dg
						| (sb * sa / 255 + db * (255 - sa) / 255);              //��p=sa/255 , FP=sb , BP=db
				}
			}
		}
	}
	void gameOver(bool isAiwin) {
		if (isAiwin) MessageBox(GetForegroundWindow(), L"AiӮ��", L"��Ϸ����", 1);
		else  MessageBox(GetForegroundWindow(), L"����Ӯ��", L"��Ϸ����", 1);
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
