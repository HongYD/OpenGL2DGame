#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<GL/glut.h>
#include<vector>
#include<iostream>
#include<time.h>
using namespace std;
int WIDTH = 800;
int HEIGHT = 900;
#define STEP_WIDTH 150
#define STEP_HEIGHT 40
#define PLAYER_WIDTH 40
#define PLAYER_HEIGHT 40
#define MOVE_SPEED 20
void SetupViewTransform();
void KeyboardFunc(unsigned char key, int x, int y);
void SetupViewVolume();
void Render();
void Reshape(int w, int h);
int Rand_x();
void TimeFunc(int id);
void ComputeStep();
void DrawStep();
void Compute_Player();//对人物进行计算，绘制
void Draw_Player();//对人物进行计算，绘制
void Draw_Border();//绘制边框
//void InitLife();//绘制人物生命值
void Collide();
void Update_Player();
void Update_Step();
void Draw_Life();
void InitOpenGL();
unsigned char *ReadBmpFile(char *fname, int *w, int *h);
using namespace std;


struct Point
{
	int x;
	int y;
};
struct Step
{
	vector<Point>Pts;
};
struct Object//游戏人物结构体
{
	vector<Point>Pts2;
};
//阶梯种类
vector<Step>StepList_Normal;
vector<Step>StepList_Left;
vector<Step>StepList_Right;
vector<Step>StepList_Spine;
vector<Step>StepList_Spring;
vector<Object>Player;//游戏人物容器，用于在游戏人物死亡时产生新人物
vector<Object>Life_State;
GLuint TexId[10];
void InitLife();
bool draw_player;
bool Collider_Bool=0;//适用于所有台阶的碰撞bool类型，如果人物与任意一种台阶相撞，立即改变人物速度，与台阶同速上升，产生静止效果。
int Collider_Normal_Spine=0;//冲撞次数记录，用于记录人物与少血加血台阶的冲撞次数，决定血量的绘制方式。
bool Collider_Left=0;
bool Collider_Right=0;
bool Collider_Spring;
int Collide_index_Normal=0;//冲撞次数记录，用于记录人物与加血台阶的冲撞编号，如果不相符，说明冲撞次数。
int Collide_index_Spine =10 ;//冲撞次数记录，用于记录人物与减血台阶的冲撞编号，如果不相符，说明冲撞的次数。
bool Collide_Left;//检测与左行台阶的碰撞。
bool Collide_Right;//检测与右行台阶的碰撞。
int a = 10;
int main(int argc, char **argv)
{
		glutInit(&argc, argv);
		glutInitWindowSize(WIDTH, HEIGHT);
		glutInitWindowPosition(0, 0);
		glutInitDisplayMode(GL_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutCreateWindow("Man Down");
		InitOpenGL();
		InitLife();
		glutDisplayFunc(Render);
		glutReshapeFunc(Reshape);
	    glutTimerFunc(100, TimeFunc, 0);
		glutKeyboardFunc(KeyboardFunc);
	    glutMainLoop();
	    return 0;
}
void ComputeStep()
{
    Step tempstep;
    Point pt1, pt2, pt3, pt4;
    pt1.x = Rand_x();
    pt1.y = 0;
    pt2.x = pt1.x + STEP_WIDTH;
    pt2.y = 0;
    pt3.x = pt2.x;
    pt3.y = pt2.y + STEP_HEIGHT;
    pt4.x = pt1.x;
    pt4.y = pt1.y + STEP_HEIGHT;
    tempstep.Pts.push_back(pt1);
    tempstep.Pts.push_back(pt2);
    tempstep.Pts.push_back(pt3);
    tempstep.Pts.push_back(pt4);
	int ix = rand() % 40;//调整阶梯密度。
    if (ix == 0)
    {
    	StepList_Normal.push_back(tempstep);
    }
    if (ix == 1)
    {
    	StepList_Left.push_back(tempstep);
		
    }
    if (ix == 2)
    {
    	StepList_Right.push_back(tempstep);
		
    }
    if (ix == 3)
    {
    	StepList_Spine.push_back(tempstep);
	
    }
    if (ix == 4)
    {
    	StepList_Spring.push_back(tempstep);
    }
}
int Rand_x()
{
	int i = (int)(rand() %(WIDTH-150-200+1)+200);
	return i;
}
void TimeFunc(int id)
{
	if (id == 0)
	{
		Collide();
		Update_Player();
		Update_Step();
		//Collide();
		glutPostRedisplay();
		glutTimerFunc(100, TimeFunc, 0);
	}
}
void SetupViewTransform()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, 0.0);
}
void SetupViewVolume()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, WIDTH, 0, HEIGHT, -10,10);
}
void Render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SetupViewTransform();
	SetupViewVolume();
	glMatrixMode(GL_MODELVIEW);
	Collide();
	Draw_Border();
	Compute_Player();
	Draw_Player();
	ComputeStep();
	DrawStep();
	Draw_Life();
	glutSwapBuffers();
	
}
void DrawStep()
{
		for (unsigned int i = 0; i < StepList_Normal.size(); i++)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, TexId[5]);
			glBegin(GL_POLYGON);
			glTexCoord2d(0, 0);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Normal[i].Pts[0].x, StepList_Normal[i].Pts[0].y, 0);

			glTexCoord2d(1, 0);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Normal[i].Pts[1].x, StepList_Normal[i].Pts[1].y, 0);

			glTexCoord2d(1,1);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Normal[i].Pts[2].x, StepList_Normal[i].Pts[2].y, 0);

			glTexCoord2d(0, 1);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Normal[i].Pts[3].x, StepList_Normal[i].Pts[3].y, 0);

			glEnd();
			printf("%d,%d\n",StepList_Normal[0].Pts[3].x,StepList_Normal[0].Pts[3].y);
		}
	
		for (unsigned int i = 0; i < StepList_Spring.size(); i++)
		{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, TexId[6]);
		glBegin(GL_POLYGON);
		glTexCoord2d(0, 0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(StepList_Spring[i].Pts[0].x, StepList_Spring[i].Pts[0].y, 0);

		glTexCoord2d(1, 0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(StepList_Spring[i].Pts[1].x, StepList_Spring[i].Pts[1].y, 0);

		glTexCoord2d(1, 1);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(StepList_Spring[i].Pts[2].x, StepList_Spring[i].Pts[2].y, 0);

		glTexCoord2d(0, 1);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(StepList_Spring[i].Pts[3].x, StepList_Spring[i].Pts[3].y, 0);

		glEnd();
		glPopMatrix();

		}
	
	
		for (unsigned int i = 0; i < StepList_Left.size(); i++)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, TexId[3]);
			glBegin(GL_POLYGON);
			glTexCoord2d(0, 0);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Left[i].Pts[0].x, StepList_Left[i].Pts[0].y, 0);

			glTexCoord2d(1, 0);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Left[i].Pts[1].x, StepList_Left[i].Pts[1].y, 0);

			glTexCoord2d(1, 1);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Left[i].Pts[2].x, StepList_Left[i].Pts[2].y, 0);

			glTexCoord2d(0, 1);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Left[i].Pts[3].x, StepList_Left[i].Pts[3].y, 0);

			glEnd();
			glPopMatrix();
		}
	
	//if(Draw_Right==1)
		for (unsigned int i = 0; i < StepList_Right.size(); i++)
		{

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, TexId[2]);
			glBegin(GL_POLYGON);
			glTexCoord2d(0, 0);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Right[i].Pts[0].x, StepList_Right[i].Pts[0].y, 0);

			glTexCoord2d(1, 0);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Right[i].Pts[1].x, StepList_Right[i].Pts[1].y, 0);

			glTexCoord2d(1, 1);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Right[i].Pts[2].x, StepList_Right[i].Pts[2].y, 0);

			glTexCoord2d(0, 1);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Right[i].Pts[3].x, StepList_Right[i].Pts[3].y, 0);

			glEnd();
			glPopMatrix();
		}
		//for (int i = 0; i < StepList_Right.size(); i++)
		//{
		//	for (int j = 0; j < StepList_Right[i].Pts.size(); j++)
		//	{
		//		StepList_Right[i].Pts[j].y += 10;
		//	}
		//}
	
	//if(Draw_Spine==1)
		for (unsigned int i = 0; i < StepList_Spine.size(); i++)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, TexId[1]);
			glBegin(GL_POLYGON);
			glTexCoord2d(0, 0);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Spine[i].Pts[0].x, StepList_Spine[i].Pts[0].y, 0);

			glTexCoord2d(1, 0);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Spine[i].Pts[1].x, StepList_Spine[i].Pts[1].y, 0);

			glTexCoord2d(1, 1); 
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Spine[i].Pts[2].x, StepList_Spine[i].Pts[2].y, 0);

			glTexCoord2d(0, 1);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(StepList_Spine[i].Pts[3].x, StepList_Spine[i].Pts[3].y, 0);

			glEnd();
			glPopMatrix();
		}
}
void Compute_Player()
{
	if (Player.size() == 0)
	{
		Object tempplayer;
		Point pts1, pts2, pts3, pts4;
		pts1.x = 400;
		pts1.y = 800;
		pts2.x = 400;
		pts2.y = pts1.y-PLAYER_HEIGHT;
		pts3.x =pts1.x+PLAYER_WIDTH;
		pts3.y =pts1.y-PLAYER_HEIGHT;
		pts4.x = pts1.x + PLAYER_WIDTH;
		pts4.y = pts1.y ;
		tempplayer.Pts2.push_back(pts1);
		tempplayer.Pts2.push_back(pts2);
		tempplayer.Pts2.push_back(pts3);
		tempplayer.Pts2.push_back(pts4);
		Player.push_back(tempplayer);
		draw_player = 1;
	}
}
void Draw_Player()
{
	//glBegin(GL_POLYGON);
	//glColor3f(0.3, 0.4, 0.5);
	//glVertex3i(400, 800, 0);
	//glVertex3i(400, 730, 0);
	//glVertex3i(470, 730, 0);
	//glVertex3i(470, 800, 0);
	//glEnd();
	if (draw_player == 1 && Player.size() == 1)
	{
		
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, TexId[0]);
		glTranslatef(0, 0, 0);
		glBegin(GL_POLYGON);
		glTexCoord2i(0,1);
		glNormal3i(0, 0, 1);
		glVertex3i(Player.back().Pts2[0].x, Player.back().Pts2[0].y, 0);
		glTexCoord2i(0,0);
		glNormal3i(0, 0, 1);
		glVertex3i(Player.back().Pts2[1].x, Player.back().Pts2[1].y, 0);
		glTexCoord2i(1,0);
		glNormal3i(0, 0, 1);
		glVertex3i(Player.back().Pts2[2].x, Player.back().Pts2[2].y, 0);
		glTexCoord2i(1,1);
		glNormal3i(0, 0, 1);
		glVertex3i(Player.back().Pts2[3].x, Player.back().Pts2[3].y, 0);
		glEnd();
		//printf("%d,%d\n", Player.back().Pts2[1].x, Player.back().Pts2[1].y);
	}
}
void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	WIDTH = w;
	HEIGHT = h;
}
void KeyboardFunc(unsigned char key, int x, int y)
{
	if (key == 'a')
	{
		if (Player.back().Pts2[0].x>200 )
		for (unsigned int k = 0; k < Player.back().Pts2.size(); k++)
		{
			
				Player.back().Pts2[k].x-= MOVE_SPEED;
			
		}
	}
	if (key == 'd')
	{
		if (Player.back().Pts2[2].x < WIDTH )
		for (unsigned int k = 0; k < Player.back().Pts2.size(); k++)
		{
			
				Player.back().Pts2[k].x += MOVE_SPEED;
			
		}
	}	
}
void Draw_Border()
{
	//glColor3f(0.3, 0.3, 0.3);
	//left
	glPushMatrix();
	glTranslatef(0, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TexId[7]);

	glBegin(GL_POLYGON);
	glTexCoord2i(0, 0);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3i(200, 0, 0);

	glTexCoord2i(1, 0);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3i(210, 0, 0);

	glTexCoord2i(1, 1);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3i(210, HEIGHT, 0);

	glTexCoord2i(0, 1);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3i(200, HEIGHT, 0);
	glEnd();


	//top
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TexId[4]);
	glBegin(GL_POLYGON);
	{
		glTexCoord2d(0, 1);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(210, HEIGHT, 0);

		glTexCoord2d(0, 0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(210, HEIGHT - 20, 0);

		glTexCoord2d(1, 0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(210 + WIDTH, HEIGHT - 20, 0);

		glTexCoord2d(1, 1);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(210 + WIDTH, HEIGHT, 0);
	}
	glEnd();

	//right
	glBindTexture(GL_TEXTURE_2D, TexId[7]);
	glBegin(GL_POLYGON);

	glTexCoord2d(0, 1);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3i(WIDTH, HEIGHT, 0);


	glBegin(GL_POLYGON);
	glTexCoord2d(1, 1);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3i(WIDTH-10, HEIGHT, 0);


	glBegin(GL_POLYGON);
	glTexCoord2d(1, 0);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3i(WIDTH - 10, 0, 0);


	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3i(WIDTH, 0, 0);
	glEnd();


	//button
	glBegin(GL_POLYGON);
	glVertex3i(210 + WIDTH, 10, 0);
	glVertex3i(210, 10, 0);
	glVertex3i(200,0, 0);
	glVertex3i(200 + WIDTH, 0, 0);
	glEnd();
}

void Collide()
{
	bool id=false;//控制碰撞次数。
	for (unsigned int i = 0;i < StepList_Normal.size(); i++)
	{
		if ((Player.back().Pts2[2].x>StepList_Normal[i].Pts[0].x)&&(Player.back().Pts2[1].x < StepList_Normal[i].Pts[1].x) &&(Player.back().Pts2[1].y == StepList_Normal[i].Pts[2].y))
		{
			int temp_index;
			Collider_Bool = 1;
	     	id = true;
			temp_index = i;
			if (Collider_Normal_Spine == 1&&temp_index!=Collide_index_Normal)
			{
				Collider_Normal_Spine = 0;
			}
			if (Collider_Normal_Spine == 2&& temp_index != Collide_index_Normal)
			{
				Collider_Normal_Spine = 1;
			}
			Collide_index_Normal = temp_index;
		}
		
	
	}
	for (unsigned int i = 0; i < StepList_Spring.size(); i++)
	{
		if (Player.back().Pts2[2].x>StepList_Spring[i].Pts[0].x&&Player.back().Pts2[1].x < StepList_Spring[i].Pts[1].x&&Player.back().Pts2[1].y == StepList_Spring[i].Pts[2].y)
		{
			Collider_Bool = 1;
			id = true;

		}

		
	}
	for (unsigned int i = 0; i < StepList_Spine.size(); i++)
	{
		if (Player.back().Pts2[2].x>StepList_Spine[i].Pts[0].x&&Player.back().Pts2[1].x < StepList_Spine[i].Pts[1].x&&Player.back().Pts2[1].y == StepList_Spine[i].Pts[2].y)
		{
			int temp_idex;
			Collider_Bool = 1;
			temp_idex = i;
			id = true;
			if (Collider_Normal_Spine == 0&&temp_idex!=Collide_index_Spine)
			{
				Collider_Normal_Spine = 1;
				Collide_index_Spine = temp_idex;
			}
			if (Collider_Normal_Spine == 1&&temp_idex != Collide_index_Spine)
			{
				Collider_Normal_Spine = 2;
				Collide_index_Spine = temp_idex;
			}
			if (Collider_Normal_Spine == 2 && temp_idex != Collide_index_Spine)
			{
				Collider_Normal_Spine = 3;
				Collide_index_Spine = temp_idex;
			}
		}
	}
	for (unsigned int i = 0; i < StepList_Right.size(); i++)
	{
		if (Player.back().Pts2[2].x>StepList_Right[i].Pts[0].x&&Player.back().Pts2[1].x < StepList_Right[i].Pts[1].x&&Player.back().Pts2[1].y == StepList_Right[i].Pts[2].y)
		{
			Collider_Bool = 1;
			Collider_Right = 1;
			id = true;
		}
		else 
			Collider_Right = 0;
	}
	for (unsigned int i = 0; i < StepList_Left.size(); i++)
	{
		if (Player.back().Pts2[2].x>StepList_Left[i].Pts[0].x&&Player.back().Pts2[1].x < StepList_Left[i].Pts[1].x&&Player.back().Pts2[1].y == StepList_Left[i].Pts[2].y)
		{
			Collider_Bool = 1;
			Collider_Left = 1;
			id = true;
		}
		else
			Collider_Left = 0;
	}
	if(id == false)
		Collider_Bool = 0;
}
void Update_Player()
{
	if (Collider_Bool == 0)
	{
		for (unsigned int k = 0; k < Player.back().Pts2.size(); k++)
		{

			Player.back().Pts2[k].y -= 10;

		}
	}
	
	

	if (Collider_Bool == 1)
	{
		for (unsigned int k = 0; k < Player.back().Pts2.size(); k++)
		{

			Player.back().Pts2[k].y += 10;
			if (Collider_Left == 1 && Collider_Bool == 1)
			{
				for (int i = 0; i < Player.back().Pts2.size(); i++)
				{
					Player.back().Pts2[i].x -= 10;
				}
			}
			if (Collider_Right == 1 && Collider_Bool == 1)
			{
				for (int i = 0; i < Player.back().Pts2.size(); i++)
				{
					Player.back().Pts2[i].x += 10;
				}
			}

		}
	}
	if (Player.back().Pts2[1].y < 0)
	{
		exit(1);
		
	}
	if (Player.back().Pts2[0].y > HEIGHT)
	{

		exit(1);
	}
	//if (Collider_Left == 1&& Collider_Bool == 1)
	//{
	//	for (int i = 0; i < Player.back().Pts2.size(); i++)
	//	{
	//		Player.back().Pts2[i].x -= 10;
	//	}
	//}
	//if (Collider_Right == 1&& Collider_Bool == 1)
	//{
	//	for (int i = 0; i < Player.back().Pts2.size(); i++)
	//	{
	//		Player.back().Pts2[i].x += 10;
	//	}
	//}
}
void  Update_Step()
{
	for (unsigned int i = 0; i < StepList_Normal.size(); i++)
	{
		for (unsigned int j = 0; j < StepList_Normal[i].Pts.size(); j++)
		{
			StepList_Normal[i].Pts[j].y += 10;
		}
	}
	for (unsigned int i = 0; i < StepList_Left.size(); i++)
	{
		for (unsigned int j = 0; j < StepList_Left[i].Pts.size(); j++)
		{
			StepList_Left[i].Pts[j].y += 10;
		}
	}
	for (unsigned int i = 0; i < StepList_Right.size(); i++)
	{
		for (unsigned int j = 0; j < StepList_Right[i].Pts.size(); j++)
		{
			StepList_Right[i].Pts[j].y += 10;
		}
	}
	for (unsigned int i = 0; i < StepList_Spring.size(); i++)
	{
		for (unsigned int j = 0; j < StepList_Spring[i].Pts.size(); j++)
		{
			StepList_Spring[i].Pts[j].y += 10;
		}
	}
	for (unsigned int i = 0; i < StepList_Spine.size(); i++)
	{
		for (unsigned int j = 0; j < StepList_Spine[i].Pts.size(); j++)
		{
			StepList_Spine[i].Pts[j].y += 10;
		}
	}
}
void InitLife()
{//0
	Point ptr1, ptr2, ptr3, ptr4;
	Object tempobj;
	ptr1.x = 20;
	ptr1.y = HEIGHT - 10;
	ptr2.x = 20;
	ptr2.y = HEIGHT - 50;
	ptr3.x = 140;
	ptr3.y = HEIGHT - 50;
	ptr4.x = 140;
	ptr4.y = HEIGHT - 10;
	tempobj.Pts2.push_back(ptr1);
	tempobj.Pts2.push_back(ptr2);
	tempobj.Pts2.push_back(ptr3);
	tempobj.Pts2.push_back(ptr4);
	Life_State.push_back(tempobj);
	tempobj.Pts2.clear();
	//1
	ptr1.x = 20;
	ptr1.y = HEIGHT - 10;
	ptr2.x = 20;
	ptr2.y = HEIGHT - 50;
	ptr3.x = 100;
	ptr3.y = HEIGHT - 50;
	ptr4.x = 100;
	ptr4.y = HEIGHT - 10;
	tempobj.Pts2.push_back(ptr1);
	tempobj.Pts2.push_back(ptr2);
	tempobj.Pts2.push_back(ptr3);
	tempobj.Pts2.push_back(ptr4);
	Life_State.push_back(tempobj);
	tempobj.Pts2.clear();
	//2
	ptr1.x = 20;
	ptr1.y = HEIGHT - 10;
	ptr2.x = 20;
	ptr2.y = HEIGHT - 50;
	ptr3.x = 60;
	ptr3.y = HEIGHT - 50;
	ptr4.x = 60;
	ptr4.y = HEIGHT - 10;
	tempobj.Pts2.push_back(ptr1);
	tempobj.Pts2.push_back(ptr2);
	tempobj.Pts2.push_back(ptr3);
	tempobj.Pts2.push_back(ptr4);
	Life_State.push_back(tempobj);
	tempobj.Pts2.clear();
	//3
	ptr1.x = 20;
	ptr1.y = HEIGHT - 10;
	ptr2.x = 20;
	ptr2.y = HEIGHT - 50;
	ptr3.x = 20;
	ptr3.y = HEIGHT - 50;
	ptr4.x = 20;
	ptr4.y = HEIGHT - 10;
	tempobj.Pts2.push_back(ptr1);
	tempobj.Pts2.push_back(ptr2);
	tempobj.Pts2.push_back(ptr3);
	tempobj.Pts2.push_back(ptr4);
	Life_State.push_back(tempobj);
	tempobj.Pts2.clear();
	
}
void Draw_Life()
{
	if (Collider_Normal_Spine == 0)
	{
		
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, TexId[8]);
		glTranslatef(0.0, 0.0, 0.0);
		glPushMatrix();
		glBegin(GL_POLYGON);
		{

			glTexCoord2d(0.0,1.0);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(Life_State[0].Pts2[0].x, Life_State[0].Pts2[0].y, 0);


			glTexCoord2d(0.0, 0.0);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(Life_State[0].Pts2[1].x, Life_State[0].Pts2[1].y, 0);


			glTexCoord2d(1.0, 0.0);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(Life_State[0].Pts2[2].x, Life_State[0].Pts2[2].y, 0);


			glTexCoord2d(1.0, 1.0);
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3i(Life_State[0].Pts2[3].x, Life_State[0].Pts2[3].y, 0);
		}
		glEnd();
		glPushMatrix();
	}
	if (Collider_Normal_Spine == 1)
	{

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, TexId[8]);
		glTranslatef(0.0, 0.0, 0.0);
		glPushMatrix();
		glBegin(GL_POLYGON);


		glTexCoord2d(0.0, 1.0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(Life_State[1].Pts2[0].x, Life_State[1].Pts2[0].y, 0);


		glTexCoord2d(0.0, 0.0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(Life_State[1].Pts2[1].x, Life_State[1].Pts2[1].y, 0);


		glTexCoord2d(1.0, 0.0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(Life_State[1].Pts2[2].x, Life_State[1].Pts2[2].y, 0);


		glTexCoord2d(1.0, 1.0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(Life_State[1].Pts2[3].x, Life_State[1].Pts2[3].y, 0);
		glEnd();
		glPushMatrix();
	}
	if (Collider_Normal_Spine == 2)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, TexId[8]);
		glTranslatef(0.0, 0.0, 0.0);
		glPushMatrix();
		glBegin(GL_POLYGON);
		glTexCoord2d(0.0, 1.0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(Life_State[2].Pts2[0].x, Life_State[2].Pts2[0].y, 0);

		glTexCoord2d(0.0, 0.0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(Life_State[2].Pts2[1].x, Life_State[2].Pts2[1].y, 0);

		glTexCoord2d(1.0, 0.0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(Life_State[2].Pts2[2].x, Life_State[2].Pts2[2].y, 0);

		glTexCoord2d(1.0, 1.0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3i(Life_State[2].Pts2[3].x, Life_State[2].Pts2[3].y, 0);
		glEnd();
		glPushMatrix();
	}
	if (Collider_Normal_Spine == 3)
	{

		glColor3f(1.0, 0.0, 0.0);
		glTranslatef(0.0, 0.0, 0.0);
		glPushMatrix();
		glBegin(GL_POLYGON);
		for (unsigned int i = 0; i < Life_State[3].Pts2.size(); i++)
		{
			glVertex3i(Life_State[3].Pts2[i].x, Life_State[3].Pts2[i].y, 0);
		}
		glEnd();
		glPushMatrix();
		Sleep(1000);
		exit(1);
	}
}
void InitOpenGL()
{
	glEnable(GL_DEPTH_TEST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glGenTextures(10, TexId);
	int w, h;
	unsigned char *pImage;
	pImage = ReadBmpFile("UI/pic1.bmp", &w, &h);
	glBindTexture(GL_TEXTURE_2D, TexId[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	delete[]pImage;

	pImage = ReadBmpFile("UI/pic2.bmp", &w, &h);
	glBindTexture(GL_TEXTURE_2D, TexId[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	delete[] pImage;
	
	pImage = ReadBmpFile("UI/pic3.bmp", &w, &h);
	glBindTexture(GL_TEXTURE_2D, TexId[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	delete[] pImage;
	
	pImage = ReadBmpFile("UI/pic4.bmp", &w, &h);
	glBindTexture(GL_TEXTURE_2D, TexId[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	delete[] pImage;
	
	pImage = ReadBmpFile("UI/pic5.bmp", &w, &h);
	glBindTexture(GL_TEXTURE_2D, TexId[4]);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	delete[] pImage;
	
	
	pImage = ReadBmpFile("UI/pic6.bmp", &w, &h);
	glBindTexture(GL_TEXTURE_2D, TexId[5]);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	delete[] pImage;

	pImage = ReadBmpFile("UI/pic7.bmp", &w, &h);
	glBindTexture(GL_TEXTURE_2D, TexId[6]);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	delete[] pImage;

	pImage = ReadBmpFile("UI/pic8.bmp", &w, &h);
	glBindTexture(GL_TEXTURE_2D, TexId[7]);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	delete[] pImage;


	pImage = ReadBmpFile("UI/pic9.bmp", &w, &h);
	glBindTexture(GL_TEXTURE_2D, TexId[8]);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	delete[] pImage;

	pImage = ReadBmpFile("UI/pic10.bmp", &w, &h);
	glBindTexture(GL_TEXTURE_2D, TexId[9]);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	delete[] pImage;

}
unsigned char *ReadBmpFile(char *fname, int *w, int *h)
{
	FILE *fp = fopen(fname, "rb");
	unsigned char *pData; // The pointer to the memory zone in which we will load the texture

						  // windows.h gives us these types to work with the Bitmap files
	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;
	RGBTRIPLE rgb;

	// Read the fileheader
	fread(&fileheader, sizeof(fileheader), 1, fp);
	fseek(fp, sizeof(fileheader), SEEK_SET); // Jump the fileheader
	fread(&infoheader, sizeof(infoheader), 1, fp); // and read the infoheader

	*w = infoheader.biWidth;
	*h = infoheader.biHeight;
	int size = infoheader.biWidth * infoheader.biHeight;

	// Now we need to allocate the memory for our image (width * height * color deep)
	pData = new byte[size * 4];
	// And fill it with zeros

	memset(pData, 0, size * 4);

	// 侨伎 历厘
	for (int i = 0, j = 0; i < size; ++i)
	{
		fread(&rgb, sizeof(rgb), 1, fp); // load RGB value
		pData[j + 0] = rgb.rgbtRed;
		pData[j + 1] = rgb.rgbtGreen;
		pData[j + 2] = rgb.rgbtBlue;
		pData[j + 3] = 255; // Alpha value
		j += 4;
	}
	fclose(fp);

	return pData;
}
