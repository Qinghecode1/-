/*
*������־
* 1����������Ŀ
* 2�������زģ�https://github.com/xiaoxichen2001/Plants-vs-Zombies��
* 3��ʵ�ֻ�����Ϸ����
* 4��ʵ����Ϸ������ֲ����
* 5��ʵ����Ϸֲ�￨��
*/
#include<stdio.h>
#include<graphics.h>
#include "tools.h"
#define WIN_WIDTH 900//ͼ�λ����ڵĿ��
#define WIN_HEIGHT 600//ͼ�λ����ڵĸ߶�
IMAGE imgBg;//����ͼƬ
IMAGE imgbar;//ֲ����
void gameinit() {
	//���ر���ͼƬ
	//������Ŀ���ƣ�ѡ�����ԣ�ѡ��߼������ַ�����Ϊ���ֽ��ַ���
	loadimage(&imgBg, "res/bg.jpg");
	loadimage(&imgbar, "res/bar5.png");
	//������Ϸͼ�λ�����
	initgraph(WIN_WIDTH, WIN_HEIGHT);
}
void updateWindows() {
	putimage(0, 0, &imgBg);
	putimagePNG(250, 0, &imgbar);
}
int main() {
	gameinit();
	updateWindows();
	system("pause");
	return 0;
}