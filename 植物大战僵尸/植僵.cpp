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
#include<time.h>
#include "tools.h"
#include"vector2.h"
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib");

#define WIN_WIDTH 900//图形化窗口的宽度
#define WIN_HEIGHT 600//图形化窗口的高度
enum { wan_dou, xiang_ri_kui, zhiwu_count };
enum{SUNSHINE_DOWN,SUNSHINE_GROUND,SUNSHINE_COLLECT,SUNSHINE_PRODUCT};
IMAGE imgBg;//背景图片
IMAGE imgbar;//植物栏
IMAGE imgCards[zhiwu_count];//植物卡
IMAGE imgbulletnormal;//植物子弹
IMAGE* imgZhiWu[zhiwu_count][20];//植物卡动态帧
IMAGE imgSunshineBall[29];//阳光动态帧
IMAGE imgZM[22];//僵尸动态帧
IMAGE imgZMDead[20];//僵尸死亡帧
IMAGE imgZMEat[21];
IMAGE imgZmStand[11];

int sunshine;//阳光值
int curX, curY;//当前选中的植物，在移动过程中的坐标
int curZhiWu;// 0:没有选中，1：选中第一种植物
//植物
struct zhiwu{
	int type;//植物类型，0：没有植物，1：第一种植物
	int frameIndex;//序列帧的序号
	bool catched;//是否被僵尸捕获
	int deadtime;//死亡计数器
	int timer;
	int x, y;
};
//植物子弹
struct bullet {
	int x, y;
	int row;
	bool used;
	int speed;
	bool blast;//是否发生爆炸
	int frameIndex;
};
//僵尸
struct zm {
	int x, y;
	int frameIndex;
	bool used;
	float speed;//僵尸的速度
	int row;
	int blood;
	bool dead;
	bool eating;//正在吃植物
};
//阳光
struct sunshineBall {
	int x, y;//实时位置
	int frameIndex;//阳光序列帧的序号
	int dextY;//目标位置
	bool used;//判断是否在使用
	int timer;//阳光在最终位置停留的时间
	//xoff和yoff用于实现阳光飞跃
	float xoff;//x轴方向偏移量
	float yoff;

	float t;//贝塞尔曲线的时间点(0...1)
	vector2 p1, p2, p3, p4;
	vector2 pCur;//当前时刻阳光球的位置
	float speed;
	int status;
};
//实例化
struct zhiwu map[3][9];//植物种植的格子，有3行9列
struct sunshineBall balls[10];
struct zm zms[10];
struct bullet bullets[10];
IMAGE imgBullBlast[4];

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
	sunshine = 50;
	memset(balls, 0, sizeof(balls));
	for (int i = 0; i < 29; i++) {
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&imgSunshineBall[i], name);
	}

	//配置随机种子
	srand(time(NULL));
	//创建游戏图形化窗口
	initgraph(WIN_WIDTH, WIN_HEIGHT);
	//设置字体
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWidth = 15;
	strcpy(f.lfFaceName, "Segoe UI Black");
	//f.lfQuality = ANTIALIASED_QUALITY;//字体抗锯齿
	settextstyle(&f);
	setbkmode(TRANSPARENT);//设置透明背景
	setcolor(BLACK);

	//初始化僵尸数据
	memset(zms, 0, sizeof(zms));
	for (int i = 0; i < 22; i++) {
		sprintf_s(name, sizeof(name), "res/zm/%d.png", i + 1);
		loadimage(&imgZM[i], name);
	}
	loadimage(&imgbulletnormal, "res/bullets/bullet_normal.png");
	memset(bullets, 0, sizeof(bullets));
	//初始化子弹的图片帧
	loadimage(&imgBullBlast[3], "res/bullets/bullet_blast.png");
	for (int i = 0; i < 3; i++) {
		float k = (i + 1) * 0.2;
		loadimage(&imgBullBlast[i], "res / bullets / bullet_blast.png",
			imgBullBlast[3].getwidth() * k,
			imgBullBlast[3].getheight() * k, true);
	}
	//初始化僵尸的死亡图片帧
	for (int i = 0; i < 20; i++) {
		sprintf_s(name, sizeof(name), "res/zm_dead/%d.png", i + 1);
		loadimage(&imgZMDead[i], name);
	}
	//僵尸吃
	for (int i = 0; i < 21; i++) {
		sprintf_s(name, sizeof(name), "res/zm_eat/%d.png", i + 1);
		loadimage(&imgZMEat[i], name);
	}
	//僵尸站立
	for (int i = 0; i < 11; i++) {
		sprintf_s(name, sizeof(name), "res/zm_stand/%d.png", i + 1);
		loadimage(&imgZmStand[i], name);
	}
}

void drawZM() {
	int zmCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < zmCount; i++) {
		if (zms[i].used) {
			//IMAGE* img = (zms[i].dead) ? imgZMDead : imgZM;
			IMAGE* img = NULL;
			if (zms[i].dead) {
				img = imgZMDead;
			}
			else if (zms[i].eating) {
				img = imgZMEat;
			}
			else {
				img = imgZM;
			}
			img += zms[i].frameIndex;
			putimagePNG(zms[i].x, zms[i].y - img->getheight(), img);
		}
		
	}
}
//渲染阳光
void drawSunshines() {
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			//putimagePNG(balls[i].x, balls[i].y, &imgSunshineBall[balls[i].frameIndex]);
			putimagePNG(balls[i].pCur.x, balls[i].pCur.y, &imgSunshineBall[balls[i].frameIndex]);
		}
	}
	//阳光值
	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunshine);
	//在指定位置输出字符
	outtextxy(278, 67, scoreText);
}

void drawCards() {
	for (int i = 0; i < zhiwu_count; i++) {
		int x = 338 + i * 64;
		int y = 6;
		putimage(x, y, &imgCards[i]);
	}
}

void drawZhiWu() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				//int x = 256 + j * 81;
				//int y = 179 + i * 102 + 14;
				int ZhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				//putimagePNG(x, y, imgZhiWu[ZhiWuType][index]);
				putimagePNG(map[i][j].x, map[i][j].y, imgZhiWu[ZhiWuType][index]);
			}
		}
	}

	//渲染拖动过程中的植物
	if (curZhiWu && curX != 0 && curY != 0) {
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, imgZhiWu[curZhiWu - 1][0]);

	}
}

void drawBullets() {
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	for (int i = 0; i < bulletMax; i++) {
		if (bullets[i].used == false)continue;
		if (bullets[i].blast) {
			IMAGE* img1 = &imgBullBlast[bullets[i].frameIndex];
			putimagePNG(bullets[i].x, bullets[i].y, img1);
		}
		else {
			putimagePNG(bullets[i].x, bullets[i].y, &imgbulletnormal);
		}
	}
}

void updateWindows() {
	//开始缓冲
	BeginBatchDraw();
	//背景
	putimage(-112, 0, &imgBg);
	//工具栏
	putimagePNG(250, 0, &imgbar);
	//植物卡
	drawCards();
	//植物
	drawZhiWu();
	//渲染阳光
	drawSunshines();
	//渲染僵尸
	drawZM();
	//渲染植物子弹
	drawBullets();
	

	
	//结束双缓冲
	EndBatchDraw();
}
//设置阳光球的偏移量
void setmove(int i) {
	float destY = 0;
	float destX = 262;
	float angle = atan((balls[i].y - destY) / (balls[i].x - destX));
	balls[i].xoff = 15 * cos(angle);
	balls[i].yoff = 15 * sin(angle);
}

void collectSunshine(ExMessage *msg) {
	int count = sizeof(balls) / sizeof(balls[0]);
	int w = imgSunshineBall[0].getwidth();
	int h = imgSunshineBall[0].getheight();
	for (int i = 0; i < count; i++) {
		if (balls[i].used) {
			//int x = balls[i].x;
			//int y = balls[i].y;
			int x = balls[i].pCur.x;
			int y = balls[i].pCur.y;
			if (msg->x && msg->x < x + w && msg->y>y && msg->y < y + h) {
				//balls[i].used = false;
				balls[i].status = SUNSHINE_COLLECT;
				//sunshine += 25;
				//mciSendString("play res/sunshine.mp3", 0, 0, 0);
				PlaySound("res/sunshine.wav", NULL, SND_FILENAME | SND_ASYNC);
				//通过是否有偏移量
				//balls[i].xoff = 6;
				balls[i].p1 = balls[i].pCur;
				balls[i].p4 = vector2(262,0);
				balls[i].t = 0;
				float distance = dis(balls[i].p1 - balls[i].p4);
				float off = 16;
				balls[i].speed = 1.0 / (distance / off);
				break;
			}
		}
	}
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
			else {
				collectSunshine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1) {
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP && status == 1) {
			if (msg.x > 144 && msg.y > 179 && msg.y < 489) {
				int row = (msg.y - 179) / 102;//行
				int col = (msg.x - 144) / 81;//列
				if (map[row][col].type == 0) {
					map[row][col].type = curZhiWu;
					map[row][col].frameIndex = 0;
					map[row][col].x = 144 + col * 81;
					map[row][col].y = 179 + row * 102 + 14;
				}
				std::cout << row << "," << col << std::endl;
				
			}
			curZhiWu = 0;
			status = 0;
		}
	}
}
//阳光的创建
void createSunshine() {
	static int count = 0;
	static int fre = 10;
	count++;
	if (count >= fre) {
		fre = 100 + rand() % 200;
		count = 0;
		//从阳光池中取一个可以使用的
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		int i;
		for (i = 0; i < ballMax && balls[i].used; i++);
		if (i >= ballMax)
			return;
		balls[i].used = true;
		balls[i].frameIndex = 0;
		//balls[i].x = 262 + rand() % 560;
		//balls[i].y = 60;
		//balls[i].dextY = 200 + (rand() % 4) * 90;
		balls[i].timer = 0;
		balls[i].status = SUNSHINE_DOWN;
		balls[i].t = 0;
		balls[i].p1 = vector2(150 + rand() % 672, 60);
		balls[i].p4 = vector2(balls[i].p1.x, 200 + (rand() % 4) * 90);
		int off = 2;
		float distance = balls[i].p4.y - balls[i].p1.y;
		balls[i].speed = 1.0 / (distance / off);
	}
	//向日葵生产阳光
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type == xiang_ri_kui + 1) {
				map[i][j].timer++;
				if (map[i][j].timer > 100) {
					map[i][j].timer = 0;

					int k;
					for (k = 0; k < ballMax && balls[k].used; k++);
					if (k >= ballMax)return;

					balls[k].used = true;
					balls[k].p1 = vector2(map[i][j].x, map[i][j].y);
					int w = (100 + rand() % 50) * (rand() % 2 ? 1 : -1);
					balls[k].p4 = vector2(map[i][j].x + w,
						map[i][j].y + imgZhiWu[xiang_ri_kui][0]->getheight() - 
						imgSunshineBall[0].getheight());
					balls[k].p2 = vector2(balls[k].p1.x + w * 0.3, balls[k].p1.y - 100);
					balls[k].p3 = vector2(balls[k].p1.x + w * 0.7, balls[k].p1.y - 100);
					balls[k].status = SUNSHINE_PRODUCT;
					balls[k].speed = 0.05;
					balls[k].t = 0;
				}
			}
		}
	}
}
//阳光的更新
void updateSunshine() {
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;
			if (balls[i].status == SUNSHINE_DOWN) {
				struct sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t >= 1) {
					sun->status = SUNSHINE_GROUND;
					sun->timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_GROUND) {
				balls[i].timer++;
				if (balls[i].timer > 100) {
					balls[i].used = false;
					balls[i].timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_COLLECT) {
				struct sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t > 1) {
					sun->used = false;
					sunshine += 25;
				}
			}
			else if (balls[i].status == SUNSHINE_PRODUCT) {
				struct sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = calcBezierPoint(sun->t, sun->p1, sun->p2, sun->p3, sun->p4);
				if (sun->t > 1) {
					sun->status = SUNSHINE_GROUND;
					sun->timer = 0;
				}
			}

			/*if (balls[i].timer == 0) {
				balls[i].y += 4;
			}
			if (balls[i].y >= balls[i].dextY) {
				balls[i].timer++;
				if (balls[i].timer > 100) {
					balls[i].used = false;
				}
				
			}*/
		}
		//else if (balls[i].xoff) {
		//	setmove(i);
		//	balls[i].x -= balls[i].xoff;
		//	balls[i].y -= balls[i].yoff;
		//	if (balls[i].y < 0 || balls[i].x < 262) {
		//		balls[i].xoff = 0;
		//		balls[i].yoff = 0;
		//		sunshine += 25;
		//	}
		//}
	}
}
//僵尸的创建
void createZM() {
	static int count = 0;
	static int zmfre = 10;
	count++;
	if (count >= zmfre) {
		count = 0;
		zmfre = 100 + rand() % 50;
		int zmMax = sizeof(zms) / sizeof(zms[0]);
		int i;
		for (i = 0; i < zmMax && zms[i].used; i++);
		if (i < zmMax) {
			memset(&zms[i], 0, sizeof(zms[i]));
			zms[i].used = true;
			zms[i].x = WIN_WIDTH;
			zms[i].row = rand() % 3;
			zms[i].y = 172 + (1 + zms[i].row) * 100;
			zms[i].speed = 0.6;
			zms[i].blood = 100;
			zms[i].dead = false;
		}
	}

}
//僵尸的更新
void updateZM() {
	int zmMax = sizeof(zms) / sizeof(zms[0]);
	//更新僵尸的位置
	static int count1 = 0;
	count1++;
	if (count1 > 1) {
		count1 = 0;
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				zms[i].x -= zms[i].speed;
				if (zms[i].x < 140) {
					std::cout << "GAME OVER" << std::endl;
					MessageBox(NULL, "over", "over", 0);
					zms[i].used = false;
					exit(0);//待优化
				}
			}
		}
	}
	
	//改变僵尸图片帧
	static int count2 = 0;
	count2++;
	if (count2 > 2) {
		count2 = 0;
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				if (zms[i].dead) {
					
					zms[i].frameIndex++;
					if (zms[i].frameIndex >= 20) {
						zms[i].used = false;
					}
				}
				else if (zms[i].eating) {
					zms[i].frameIndex = (zms[i].frameIndex + 1) % 21;
				}
				else {
					zms[i].frameIndex = (zms[i].frameIndex + 1) % 22;
				}
				
			}
		}
	}


}


void shoot() {
	bool lines[3] = { false };
	int zmCount = sizeof(zms) / sizeof(zms[0]);
	int dangerX = WIN_WIDTH - imgZM[0].getwidth() / 2;
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	for (int i = 0; i < zmCount; i++) {
		if (zms[i].used && zms[i].x < dangerX) {
			lines[zms[i].row] = true;//如果有僵尸且小于一定距离，则令lines为真
		}
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type == wan_dou + 1 && lines[i]) {
				static int count = 0;
				count++;
				if (count > 50) {
					count = 0;
					int k;
					for (k = 0; k < bulletMax && bullets[k].used; k++);
					if (k < bulletMax) {
						bullets[k].used = true;
						bullets[k].row = i;
						bullets[k].speed = 4;
						bullets[k].blast = false;
						bullets[k].frameIndex = 0;
						int zwX = 144 + j * 81;
						int zwY = 179 + i * 102 + 14;
						bullets[k].x = zwX + imgZhiWu[map[i][j].type - 1][0]->getwidth() - 10;
						bullets[k].y = zwY + 7;
					}
				}
			}
		}
	}
}
//子弹的更新
void updateBullet() {
	int countMax = sizeof(bullets) / sizeof(bullets[0]);
	for (int i = 0; i < countMax; i++) {
		if (bullets[i].used) {
			bullets[i].x += bullets[i].speed;
			if (bullets[i].x > WIN_WIDTH) {
				bullets[i].used = false;
			}
			if (bullets[i].blast) {
				bullets[i].frameIndex++;
				if (bullets[i].frameIndex > 3) {
					bullets[i].used = false;
				}
			}
		}
	}
}

void updateZhiWu() {
	static int count = 0;
	if (++count < 2)return;
	count = 0;
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

void checkBullet2Zm() {
	int bCount = sizeof(bullets) / sizeof(bullets[0]);
	int zmCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < bCount; i++) {
		if (bullets[i].used == false || bullets[i].blast)continue;
		for (int k = 0; k < zmCount; k++) {
			if (zms[k].used == false)continue;
			int x1 = zms[k].x + 80;
			int x2 = zms[k].x + 110;
			if (zms[k].dead == false && bullets[i].row == zms[k].row && bullets[i].x > x1 && bullets[i].x < x2) {
				zms[k].blood -= 20;
				bullets[i].blast = true;
				bullets[i].speed = 0;
				if (zms[k].blood <= 0) {
					zms[k].dead = true;
					zms[k].speed = 0;
					zms[k].frameIndex = 0;
				}
				break;
			}
		}
	}
}

void checkZm2ZhiWu() {
	int zCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < zCount; i++) {
		if (zms[i].dead)continue;

		int row = zms[i].row;
		for (int k = 0; k < 9; k++) {
			if (map[row][k].type == 0) {
				continue;
			}
			int zhiwux = 144 + k * 81;
			int x1 = zhiwux + 10;
			int x2 = zhiwux + 60;
			int x3 = zms[i].x + 80;
			if (x3 > x1 && x3 < x2) {
				if (map[row][k].catched) {
					map[row][k].deadtime++;
					if (map[row][k].deadtime > 100) {
						map[row][k].deadtime = 0;
						map[row][k].type = 0;
						zms[i].eating = false;
						zms[i].frameIndex = 0;
						zms[i].speed = 0.6;
					}
				}
				else {
					map[row][k].catched = true;
					map[row][k].deadtime = 0;
					zms[i].eating = true;
					zms[i].speed = 0;
					zms[i].frameIndex = 0;
				}
			}
		}
	}
}

void collisionCheck() {
	checkBullet2Zm();//子弹对僵尸的碰撞检测
	checkZm2ZhiWu();//僵尸对植物的碰撞检测
}

void updateGame() {
	updateZhiWu();
	createSunshine();//创建阳光
	updateSunshine();//更新阳光
	createZM();//创建僵尸
	updateZM();//更新僵尸
	shoot();//发射子弹
	updateBullet();

	collisionCheck();//实现豌豆子弹和僵尸的碰撞检测
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
				&& ((msg.x < 474 || msg.x > 474 + 300)
				|| (msg.y < 75 ||  msg.y > 75 + 140))) {
				judge = false;
			}
			if (//msg.message == WM_LBUTTONDOWN 
				msg.x > 474 && msg.x < 474 + 300
				&& msg.y > 75 && msg.y < 75 + 140) {
				flag = 1;
				if (msg.message == WM_LBUTTONUP && judge == true) {
					EndBatchDraw();
					break;
				}

			}
			else {
				flag = 0;
			}
		}
		EndBatchDraw();
	}
}
//音乐初始化
void musicinit() {
	mciSendString("play res/bg.mp3", 0, 0, 0);
}

void viewScence() {
	int xMin = WIN_WIDTH - imgBg.getwidth();
	vector2 points[9] = {
		{550,80},{530,160},{630,170},{530,200},{515,270},
		{565,370},{605,340},{705,280},{690,340}
	};
	int index[9];
	for (int i = 0; i < 9; i++) {
		index[i] = rand() % 11;
	}
	//开始巡场
	int count = 0;
	for (int x = 0; x >= xMin; x-=2) {
		BeginBatchDraw();
		putimage(x, 0, &imgBg);
		count++;
		for (int k = 0; k < 9; k++) {
			putimagePNG(points[k].x - xMin + x,points[k].y,&imgZmStand[index[k]]);
			if (count >= 10) {
				index[k] = (index[k] + 1) % 11;
			}
		}
		if (count >= 10)count = 0;
		EndBatchDraw();
		Sleep(5);
	}
	//停留2秒
	for (int i = 0; i < 100; i++) {
		BeginBatchDraw();
		putimage(xMin, 0, &imgBg);
		count++;
		for (int k = 0; k < 9; k++) {
			putimagePNG(points[k].x, points[k].y, &imgZmStand[index[k]]);
			if (count >= 10) {
				index[k] = (index[k] + 1) % 11;
			}
			
		}
		if (count >= 10)count = 0;
		EndBatchDraw();
		Sleep(20);
	}
	//缓慢切回原位
	for (int x = xMin; x <= -112; x += 2) {
		BeginBatchDraw();
		putimage(x, 0, &imgBg);
		count++;
		for (int k = 0; k < 9; k++) {
			putimagePNG(points[k].x - xMin + x, points[k].y, &imgZmStand[index[k]]);
			if (count >= 10) {
				index[k] = (index[k] + 1) % 11;
			}
		}
		if (count >= 10)count = 0;
		EndBatchDraw();
		Sleep(5);
	}
}

int main() {
	gameinit();
	StartUI();
	viewScence();
	musicinit();
	int timer = 0;
	bool flag = true;
	while (1) {
		userClick();
		//getDelay返回计算这个函数两次调用之间的间隔
		timer += getDelay();
		if (timer > 40) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			flag = false;
			updateGame();//更新游戏数据
		}
		updateWindows();
	
	}
	
	system("pause");
	return 0;
}