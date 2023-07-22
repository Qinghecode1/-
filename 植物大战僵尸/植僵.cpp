/*
*开发日志
* 1、创建新项目
* 2、导入素材（https://github.com/xiaoxichen2001/Plants-vs-Zombies）
* 3、实现基本游戏场景
*/
#include<stdio.h>
#include<graphics.h>
#define WIN_WIDTH 900//图形化窗口的宽度
#define WIN_HEIGHT 600//图形化窗口的高度
IMAGE imgBg;//背景图片
void gameinit() {
	//加载背景图片
	//单击项目名称，选择属性，选择高级，把字符集改为多字节字符集
	loadimage(&imgBg, "res/bg.jpg");
	//创建游戏图形化窗口
	initgraph(WIN_WIDTH, WIN_HEIGHT);
}
void updateWindows() {
	putimage(0, 0, &imgBg);
}
int main() {
	gameinit();
	updateWindows();
	system("pause");
	return 0;
}