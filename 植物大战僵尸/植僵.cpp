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
#include<time.h>
#include "tools.h"
#include"vector2.h"
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib");

#define WIN_WIDTH 900//ͼ�λ����ڵĿ��
#define WIN_HEIGHT 600//ͼ�λ����ڵĸ߶�
enum { wan_dou, xiang_ri_kui, zhiwu_count };
enum{SUNSHINE_DOWN,SUNSHINE_GROUND,SUNSHINE_COLLECT,SUNSHINE_PRODUCT};
IMAGE imgBg;//����ͼƬ
IMAGE imgbar;//ֲ����
IMAGE imgCards[zhiwu_count];//ֲ�￨
IMAGE imgbulletnormal;//ֲ���ӵ�
IMAGE* imgZhiWu[zhiwu_count][20];//ֲ�￨��̬֡
IMAGE imgSunshineBall[29];//���⶯̬֡
IMAGE imgZM[22];//��ʬ��̬֡
IMAGE imgZMDead[20];//��ʬ����֡
IMAGE imgZMEat[21];
IMAGE imgZmStand[11];

int sunshine;//����ֵ
int curX, curY;//��ǰѡ�е�ֲ����ƶ������е�����
int curZhiWu;// 0:û��ѡ�У�1��ѡ�е�һ��ֲ��
//ֲ��
struct zhiwu{
	int type;//ֲ�����ͣ�0��û��ֲ�1����һ��ֲ��
	int frameIndex;//����֡�����
	bool catched;//�Ƿ񱻽�ʬ����
	int deadtime;//����������
	int timer;
	int x, y;
};
//ֲ���ӵ�
struct bullet {
	int x, y;
	int row;
	bool used;
	int speed;
	bool blast;//�Ƿ�����ը
	int frameIndex;
};
//��ʬ
struct zm {
	int x, y;
	int frameIndex;
	bool used;
	float speed;//��ʬ���ٶ�
	int row;
	int blood;
	bool dead;
	bool eating;//���ڳ�ֲ��
};
//����
struct sunshineBall {
	int x, y;//ʵʱλ��
	int frameIndex;//��������֡�����
	int dextY;//Ŀ��λ��
	bool used;//�ж��Ƿ���ʹ��
	int timer;//����������λ��ͣ����ʱ��
	//xoff��yoff����ʵ�������Ծ
	float xoff;//x�᷽��ƫ����
	float yoff;

	float t;//���������ߵ�ʱ���(0...1)
	vector2 p1, p2, p3, p4;
	vector2 pCur;//��ǰʱ���������λ��
	float speed;
	int status;
};
//ʵ����
struct zhiwu map[3][9];//ֲ����ֲ�ĸ��ӣ���3��9��
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
	sunshine = 50;
	memset(balls, 0, sizeof(balls));
	for (int i = 0; i < 29; i++) {
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&imgSunshineBall[i], name);
	}

	//�����������
	srand(time(NULL));
	//������Ϸͼ�λ�����
	initgraph(WIN_WIDTH, WIN_HEIGHT);
	//��������
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWidth = 15;
	strcpy(f.lfFaceName, "Segoe UI Black");
	//f.lfQuality = ANTIALIASED_QUALITY;//���忹���
	settextstyle(&f);
	setbkmode(TRANSPARENT);//����͸������
	setcolor(BLACK);

	//��ʼ����ʬ����
	memset(zms, 0, sizeof(zms));
	for (int i = 0; i < 22; i++) {
		sprintf_s(name, sizeof(name), "res/zm/%d.png", i + 1);
		loadimage(&imgZM[i], name);
	}
	loadimage(&imgbulletnormal, "res/bullets/bullet_normal.png");
	memset(bullets, 0, sizeof(bullets));
	//��ʼ���ӵ���ͼƬ֡
	loadimage(&imgBullBlast[3], "res/bullets/bullet_blast.png");
	for (int i = 0; i < 3; i++) {
		float k = (i + 1) * 0.2;
		loadimage(&imgBullBlast[i], "res / bullets / bullet_blast.png",
			imgBullBlast[3].getwidth() * k,
			imgBullBlast[3].getheight() * k, true);
	}
	//��ʼ����ʬ������ͼƬ֡
	for (int i = 0; i < 20; i++) {
		sprintf_s(name, sizeof(name), "res/zm_dead/%d.png", i + 1);
		loadimage(&imgZMDead[i], name);
	}
	//��ʬ��
	for (int i = 0; i < 21; i++) {
		sprintf_s(name, sizeof(name), "res/zm_eat/%d.png", i + 1);
		loadimage(&imgZMEat[i], name);
	}
	//��ʬվ��
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
//��Ⱦ����
void drawSunshines() {
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			//putimagePNG(balls[i].x, balls[i].y, &imgSunshineBall[balls[i].frameIndex]);
			putimagePNG(balls[i].pCur.x, balls[i].pCur.y, &imgSunshineBall[balls[i].frameIndex]);
		}
	}
	//����ֵ
	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunshine);
	//��ָ��λ������ַ�
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

	//��Ⱦ�϶������е�ֲ��
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
	//��ʼ����
	BeginBatchDraw();
	//����
	putimage(-112, 0, &imgBg);
	//������
	putimagePNG(250, 0, &imgbar);
	//ֲ�￨
	drawCards();
	//ֲ��
	drawZhiWu();
	//��Ⱦ����
	drawSunshines();
	//��Ⱦ��ʬ
	drawZM();
	//��Ⱦֲ���ӵ�
	drawBullets();
	

	
	//����˫����
	EndBatchDraw();
}
//�����������ƫ����
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
				//ͨ���Ƿ���ƫ����
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
				int row = (msg.y - 179) / 102;//��
				int col = (msg.x - 144) / 81;//��
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
//����Ĵ���
void createSunshine() {
	static int count = 0;
	static int fre = 10;
	count++;
	if (count >= fre) {
		fre = 100 + rand() % 200;
		count = 0;
		//���������ȡһ������ʹ�õ�
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
	//���տ���������
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
//����ĸ���
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
//��ʬ�Ĵ���
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
//��ʬ�ĸ���
void updateZM() {
	int zmMax = sizeof(zms) / sizeof(zms[0]);
	//���½�ʬ��λ��
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
					exit(0);//���Ż�
				}
			}
		}
	}
	
	//�ı佩ʬͼƬ֡
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
			lines[zms[i].row] = true;//����н�ʬ��С��һ�����룬����linesΪ��
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
//�ӵ��ĸ���
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
	checkBullet2Zm();//�ӵ��Խ�ʬ����ײ���
	checkZm2ZhiWu();//��ʬ��ֲ�����ײ���
}

void updateGame() {
	updateZhiWu();
	createSunshine();//��������
	updateSunshine();//��������
	createZM();//������ʬ
	updateZM();//���½�ʬ
	shoot();//�����ӵ�
	updateBullet();

	collisionCheck();//ʵ���㶹�ӵ��ͽ�ʬ����ײ���
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
//���ֳ�ʼ��
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
	//��ʼѲ��
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
	//ͣ��2��
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
	//�����л�ԭλ
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
		//getDelay���ؼ�������������ε���֮��ļ��
		timer += getDelay();
		if (timer > 40) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			flag = false;
			updateGame();//������Ϸ����
		}
		updateWindows();
	
	}
	
	system("pause");
	return 0;
}