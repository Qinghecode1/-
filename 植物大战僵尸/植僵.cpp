/*
*开发日志
* 1、创建新项目
* 2、导入素材（https://github.com/xiaoxichen2001/Plants-vs-Zombies）
* 3、实现基本游戏场景
* 4、实现游戏顶部的植物栏
* 5、实现游戏植物卡牌
*/
#include<stdio.h>
#include<graphics.h>
#include "tools.h"
#define WIN_WIDTH 900//图形化窗口的宽度
#define WIN_HEIGHT 600//图形化窗口的高度
IMAGE imgBg;//背景图片
IMAGE imgbar;//植物栏
void gameinit() {
	//加载背景图片
	//单击项目名称，选择属性，选择高级，把字符集改为多字节字符集
	loadimage(&imgBg, "res/bg.jpg");
	loadimage(&imgbar, "res/bar5.png");
	//创建游戏图形化窗口
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