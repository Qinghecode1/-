/*
*开发日志
* 1、创建新项目
* 2、导入素材（https://github.com/xiaoxichen2001/Plants-vs-Zombies）
* 3、实现基本游戏场景
* 4、实现游戏顶部的植物栏
* 5、实现游戏植物卡牌
* 6、实现植物卡牌的拖动和放置
* 7、实现植物的摇摆，优化渲染顺序
*/
#include<stdio.h>
#include<iostream>
#include<graphics.h>
#include "tools.h"
#define WIN_WIDTH 900//图形化窗口的宽度
#define WIN_HEIGHT 600//图形化窗口的高度
enum { wan_dou, xiang_ri_kui, zhiwu_count };
IMAGE imgBg;//背景图片
IMAGE imgbar;//植物栏
IMAGE imgCards[zhiwu_count];//植物卡
IMAGE* imgZhiWu[zhiwu_count][20];

int curX, curY;//当前选中的植物，在移动过程中的坐标
int curZhiWu;// 0:没有选中，1：选中第一种植物

struct zhiwu{
	int type;//植物类型，0：没有植物，1：第一种植物
	int frameIndex;//序列帧的序号
};
struct zhiwu map[3][9];//植物种植的格子，有3行9列

bool fileExist(const char* name) {
	FILE* fp = fopen(name, "r");
	if (fp) {
		fclose(fp);
	}
	return fp != NULL;
}

void gameinit() {
	//加载背景图片
	//右键单击项目名称，选择属性，选择高级，把字符集改为多字节字符集
	loadimage(&imgBg, "res/bg.jpg");
	//加载植物栏
	loadimage(&imgbar, "res/bar5.png");

	memset(imgZhiWu, 0, sizeof(imgZhiWu));
	memset(map, 0, sizeof(map));//一开始没有种植植物，故全设为0
	//初始化游戏卡牌
	char name[64];
	for (int i = 0; i < zhiwu_count; i++) {
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], name);
		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);   
			//判断文件是否存在
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
	//创建游戏图形化窗口
	initgraph(WIN_WIDTH, WIN_HEIGHT,1);
}


void updateWindows() {
	//开始缓冲
	BeginBatchDraw();
	//背景
	putimage(0, 0, &imgBg);
	//工具栏
	putimagePNG(250, 0, &imgbar);
	//植物卡
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

	//渲染拖动过程中的植物
	if (curZhiWu && curX != 0 && curY != 0) {
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, imgZhiWu[curZhiWu - 1][0]);

	}
	//结束双缓冲
	EndBatchDraw();
}

void userClick() {
	ExMessage msg;//消息结构体，这里是为了获取鼠标消息
	static int status = 0;//因为要按下选中再拖动，故需要一个状态变量
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
				int row = (msg.y - 179) / 102;//行
				int col = (msg.x - 256) / 81;//列
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
//启动菜单
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
		//getDelay返回计算这个函数两次调用之间的间隔
		timer += getDelay();
		if (timer > 60) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			flag = false;
			updateGame();//更新游戏数据，现在的作用是使植物实现动态摇晃效果
		}
		updateWindows();
	
	}
	
	system("pause");
	return 0;
}