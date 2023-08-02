/*
*������־
* 1����������Ŀ
* 2�������زģ�https://github.com/xiaoxichen2001/Plants-vs-Zombies��
* 3��ʵ�ֻ�����Ϸ����
* 4��ʵ����Ϸ������ֲ����
* 5��ʵ����Ϸֲ�￨��
* 6��ʵ��ֲ�￨�Ƶ��϶��ͷ���
* 7��ʵ��ֲ���ҡ�ڣ��Ż���Ⱦ˳��
*/
#include<stdio.h>
#include<iostream>
#include<graphics.h>
#include "tools.h"
#define WIN_WIDTH 900//ͼ�λ����ڵĿ��
#define WIN_HEIGHT 600//ͼ�λ����ڵĸ߶�
enum { wan_dou, xiang_ri_kui, zhiwu_count };
IMAGE imgBg;//����ͼƬ
IMAGE imgbar;//ֲ����
IMAGE imgCards[zhiwu_count];//ֲ�￨
IMAGE* imgZhiWu[zhiwu_count][20];

int curX, curY;//��ǰѡ�е�ֲ����ƶ������е�����
int curZhiWu;// 0:û��ѡ�У�1��ѡ�е�һ��ֲ��

struct zhiwu{
	int type;//ֲ�����ͣ�0��û��ֲ�1����һ��ֲ��
	int frameIndex;//����֡�����
};
struct zhiwu map[3][9];//ֲ����ֲ�ĸ��ӣ���3��9��

bool fileExist(const char* name) {
	FILE* fp = fopen(name, "r");
	if (fp) {
		fclose(fp);
	}
	return fp != NULL;
}

void gameinit() {
	//���ر���ͼƬ
	//�Ҽ�������Ŀ���ƣ�ѡ�����ԣ�ѡ��߼������ַ�����Ϊ���ֽ��ַ���
	loadimage(&imgBg, "res/bg.jpg");
	//����ֲ����
	loadimage(&imgbar, "res/bar5.png");

	memset(imgZhiWu, 0, sizeof(imgZhiWu));
	memset(map, 0, sizeof(map));//һ��ʼû����ֲֲ���ȫ��Ϊ0
	//��ʼ����Ϸ����
	char name[64];
	for (int i = 0; i < zhiwu_count; i++) {
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], name);
		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);   
			//�ж��ļ��Ƿ����
			if (fileExist(name)) {
				imgZhiWu[i][j] = new IMAGE;
				loadimage(imgZhiWu[i][j], name);
			}
			else {
				break;
			}
		}
	}
	curZhiWu = 0;
	//������Ϸͼ�λ�����
	initgraph(WIN_WIDTH, WIN_HEIGHT,1);
}


void updateWindows() {
	//��ʼ����
	BeginBatchDraw();
	//����
	putimage(0, 0, &imgBg);
	//������
	putimagePNG(250, 0, &imgbar);
	//ֲ�￨
	for (int i = 0; i < zhiwu_count; i++) {
		int x = 338 + i * 64;
		int y = 6;
		putimage(x, y, &imgCards[i]);
	}
	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				int x = 256 + j * 81;
				int y = 179 + i * 102 + 14;
				int ZhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				putimagePNG(x, y, imgZhiWu[ZhiWuType][index]);
			}
		}
	}

	//��Ⱦ�϶������е�ֲ��
	if (curZhiWu && curX != 0 && curY != 0) {
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, imgZhiWu[curZhiWu - 1][0]);

	}
	//����˫����
	EndBatchDraw();
}

void userClick() {
	ExMessage msg;//��Ϣ�ṹ�壬������Ϊ�˻�ȡ�����Ϣ
	static int status = 0;//��ΪҪ����ѡ�����϶�������Ҫһ��״̬����
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN) {
			if (msg.x > 338 && msg.x < 338 + 65 * zhiwu_count && msg.y < 96) {
				int index = (msg.x - 338) / 65;
				status = 1;
				curZhiWu = index + 1;
				curX = msg.x;
				curY = msg.y;
				//std::cout << index << std::endl;
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1) {
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP) {
			if (msg.x > 256 && msg.y > 179 && msg.y < 489) {
				int row = (msg.y - 179) / 102;//��
				int col = (msg.x - 256) / 81;//��
				if (map[row][col].type == 0) {
					map[row][col].type = curZhiWu;
					map[row][col].frameIndex = 0;
				}
				std::cout << row << "," << col << std::endl;
				
			}
			curZhiWu = 0;
			status = 0;
		}
	}
}

void updateGame() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				map[i][j].frameIndex++;
				int ZhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				if (imgZhiWu[ZhiWuType][index] == NULL) {
					map[i][j].frameIndex = 0;
				}
			}
		}
	}
}
//�����˵�
void StartUI() {
	IMAGE imgBg, imgMenu1, imgMenu2;
	loadimage(&imgBg, "res/menu.png");
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");
	int flag = 0;
	while (1) {
		BeginBatchDraw();
		putimage(0, 0, &imgBg);
		putimagePNG(474, 75, flag ? &imgMenu2 : &imgMenu1);
		ExMessage msg;
		if (peekmessage(&msg)) {
			static bool judge = false;
			if (msg.message == WM_LBUTTONDOWN 
				&& msg.x > 474 && msg.x < 474 + 300
				&& msg.y > 75 && msg.y < 75 + 140) {
				judge = true;
			}
			else if(msg.message == WM_LBUTTONDOWN
				&& msg.x < 474 && msg.x > 474 + 300
				&& msg.y < 75 && msg.y > 75 + 140) {
				judge = false;
			}
			if (//msg.message == WM_LBUTTONDOWN 
				msg.x > 474 && msg.x < 474 + 300
				&& msg.y > 75 && msg.y < 75 + 140) {
				flag = 1;
				if (msg.message == WM_LBUTTONUP && judge == true) {
					return;
				}

			}
			else {
				flag = 0;
			}
		}
		EndBatchDraw();
	}
}

int main() {
	gameinit();
	StartUI();
	int timer = 0;
	bool flag = true;
	while (1) {
		userClick();
		//getDelay���ؼ�������������ε���֮��ļ��
		timer += getDelay();
		if (timer > 60) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			flag = false;
			updateGame();//������Ϸ���ݣ����ڵ�������ʹֲ��ʵ�ֶ�̬ҡ��Ч��
		}
		updateWindows();
	
	}
	
	system("pause");
	return 0;
}