#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <glut.h>
#include <windows.h>
#define MAX_CHAR 128

float PI=3.14159265358979323846; // 

GLuint theObject;

float XP[3] = { 15, 0, 0 }, XN[3] = { -15, 0, 0 },
YP[3] = { 0, 15, 0 }, YN[3] = { 0, -15, 0 },
ZP[3] = { 0, 0, 15 }, ZN[3] = { 0, 0, -15 };

int at_menu = 1, select_com = 1, End_flag = 0;
int is_AI = 1, Has_Gpoint[6], Save_Gpoint[6][3];
int turn_RB = 1, win_RB = 0; //1:Red  -1:Blue
int ary_RB[4][4][4], if_reset = 0;
int slt_x = 0, slt_y = 0, slt_z = 0, slt_w = 0;
float one_bl = 8, half_bl = 4;
float PX_S[5][5][3], PX_E[5][5][3],
PY_S[5][5][3], PY_E[5][5][3],
PZ_S[5][5][3], PZ_E[5][5][3];

void drawString(const char* str) {
	static int isFirstCall = 1;
	static GLuint lists;
	if (isFirstCall) {
		isFirstCall = 0;
		lists = glGenLists(MAX_CHAR);
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}
	for (; *str != '*'; ++str)
		glCallList(lists + *str);
}

void selectFont(int size, int charset, const char* face) {
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}

void Set_line(){
	
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++){
			PX_S[i][j][0] = -one_bl;
			PX_S[i][j][1] = -one_bl + half_bl*j;
			PX_S[i][j][2] = -one_bl + half_bl*i;
			PX_E[i][j][0] = one_bl;
			PX_E[i][j][1] = -one_bl + half_bl*j;
			PX_E[i][j][2] = -one_bl + half_bl*i;

			PY_S[i][j][0] = -one_bl + half_bl*j;
			PY_S[i][j][1] = -one_bl;
			PY_S[i][j][2] = -one_bl + half_bl*i;
			PY_E[i][j][0] = -one_bl + half_bl*j;
			PY_E[i][j][1] = one_bl;
			PY_E[i][j][2] = -one_bl + half_bl*i;

			PZ_S[i][j][0] = -one_bl + half_bl*j;
			PZ_S[i][j][1] = -one_bl + half_bl*i;
			PZ_S[i][j][2] = one_bl;
			PZ_E[i][j][0] = -one_bl + half_bl*j;
			PZ_E[i][j][1] = -one_bl + half_bl*i;
			PZ_E[i][j][2] = -one_bl;
		}

}
void reset_ary_RB(){
	if_reset = 1;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				ary_RB[i][j][k] = 0;
				
}

bool Check_Win(int whose_turn){
	int ball_x = 0, ball_y = 0, ball_z = 0,
		ball_x_corse = 0, ball_y_corse = 0, ball_z_corse = 0,
		ball_x_Bcorse = 0, ball_y_Bcorse = 0, ball_z_Bcorse = 0,
		ball_3Dcorse1 = 0, ball_3Dcorse2 = 0, ball_3Dcorse3 = 0, ball_3Dcorse4 = 0;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			for (int k = 0; k < 4; k++){
				if (ary_RB[i][j][k] == whose_turn)
					ball_x++; 
				if (ary_RB[k][i][j] == whose_turn)
					ball_y++;
				if (ary_RB[j][k][i] == whose_turn)
					ball_z++;
			}
			if (ball_x == 4 || ball_y == 4 || ball_z == 4)
				return true;
			else
				ball_x = 0, ball_y = 0, ball_z = 0; //recount

			if (ary_RB[i][j][j] == whose_turn)
				ball_x_corse++;
			if (ary_RB[j][i][j] == whose_turn)
				ball_y_corse++;
			if (ary_RB[j][j][i] == whose_turn)
				ball_z_corse++;
			if (ary_RB[i][0 + j][3 - j] == whose_turn)
				ball_x_Bcorse++;
			if (ary_RB[0 + j][i][3 - j] == whose_turn)
				ball_y_Bcorse++;
			if (ary_RB[0 + j][3 - j][i] == whose_turn)
				ball_z_Bcorse++;
		}
		if (ball_x_corse == 4 || ball_y_corse == 4 || ball_z_corse == 4
			|| ball_x_Bcorse == 4 || ball_y_Bcorse == 4 || ball_z_Bcorse == 4)
			return true;
		else
			ball_x_corse = 0, ball_y_corse = 0, ball_z_corse = 0,
			ball_x_Bcorse = 0, ball_y_Bcorse = 0, ball_z_Bcorse = 0; //recount

		if (ary_RB[0 + i][0 + i][0 + i] == whose_turn)
			ball_3Dcorse1++;
		if (ary_RB[0 + i][0 + i][3 - i] == whose_turn)
			ball_3Dcorse2++;
		if (ary_RB[0 + i][3 - i][3 - i] == whose_turn)
			ball_3Dcorse3++;
		if (ary_RB[0 + i][3 - i][0 + i] == whose_turn)
			ball_3Dcorse4++;
	}
	if (ball_3Dcorse1 == 4 || ball_3Dcorse2 == 4 || ball_3Dcorse3 == 4
		|| ball_3Dcorse4 == 4)
		return true;
	else
		ball_3Dcorse1 = 0, ball_3Dcorse2 = 0, ball_3Dcorse3 = 0,
		ball_3Dcorse4 = 0; //recount

	return false;
}
void AI_good_point(int No, int x, int y, int z){
	Has_Gpoint[No] = 1;
	Save_Gpoint[No][0] = x;
	Save_Gpoint[No][1] = y;
	Save_Gpoint[No][2] = z;
}
void AI_put_point(int No){
	ary_RB[Save_Gpoint[No][0]][Save_Gpoint[No][1]][Save_Gpoint[No][2]] = -1;
}

bool AI_select(int whose_turn, int check_point){
	for (int i = 0; i < 6; i++){
		Has_Gpoint[i] = 0, Save_Gpoint[i][0] = -1,
		Save_Gpoint[i][1] = -1, Save_Gpoint[i][2] = -1;
	}
		
	int ball_x = 0, ball_y = 0, ball_z = 0,
		ball_xgp = 0, ball_ygp = 0, ball_zgp = 0,
		ball_x_corse = 0, ball_y_corse = 0, ball_z_corse = 0,
		ball_x_Bcorse = 0, ball_y_Bcorse = 0, ball_z_Bcorse = 0,
		ball_x_corsegp = 0, ball_y_corsegp = 0, ball_z_corsegp = 0,
		ball_x_Bcorsegp = 0, ball_y_Bcorsegp = 0, ball_z_Bcorsegp = 0,
		ball_3Dcorse1 = 0, ball_3Dcorse2 = 0, ball_3Dcorse3 = 0, ball_3Dcorse4 = 0,
		ball_3Dcorse1gp = 0, ball_3Dcorse2gp = 0, ball_3Dcorse3gp = 0, ball_3Dcorse4gp = 0;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			for (int k = 0; k < 4; k++){
				if (ary_RB[i][j][k] == whose_turn)
					ball_x++;
				else if (ary_RB[i][j][k] == 0)
					AI_good_point(0, i, j, k), ball_xgp++;

				if (ary_RB[k][i][j] == whose_turn)
					ball_y++;
				else if (ary_RB[k][i][j] == 0)
					AI_good_point(1, k, i, j), ball_ygp++;

				if (ary_RB[j][k][i] == whose_turn)
					ball_z++;
				else if (ary_RB[j][k][i] == 0)
					AI_good_point(2, j, k, i), ball_zgp++;

			}
			if (ball_x >= check_point && ball_x+ball_xgp==4 && Has_Gpoint[0] == 1){
				AI_put_point(0); return true;}
			else if (ball_y >= check_point && ball_y + ball_ygp == 4 && Has_Gpoint[1] == 1){
				AI_put_point(1); return true;}
			else if (ball_z >= check_point && ball_z + ball_zgp == 4 && Has_Gpoint[2] == 1){
				AI_put_point(2); return true;}
			else
				ball_x = 0, ball_y = 0, ball_z = 0,
				ball_xgp = 0, ball_ygp = 0, ball_zgp = 0; //recount

			if (ary_RB[i][j][j] == whose_turn)
				ball_x_corse++;
			else if (ary_RB[i][j][j] == 0)
				AI_good_point(0, i, j, j), ball_x_corsegp++;

			if (ary_RB[j][i][j] == whose_turn)
				ball_y_corse++;
			else if (ary_RB[j][i][j] == 0)
				AI_good_point(1, j, i, j), ball_y_corsegp++;

			if (ary_RB[j][j][i] == whose_turn)
				ball_z_corse++;
			else if (ary_RB[j][j][i] == 0)
				AI_good_point(2, j, j, i), ball_z_corsegp++;

			if (ary_RB[i][0 + j][3 - j] == whose_turn)
				ball_x_Bcorse++;
			else if (ary_RB[i][0 + j][3 - j] == 0)
				AI_good_point(3, i, 0 + j, 3 - j), ball_x_Bcorsegp++;

			if (ary_RB[0 + j][i][3 - j] == whose_turn)
				ball_y_Bcorse++;
			else if (ary_RB[0 + j][i][3 - j] == 0)
				AI_good_point(4, 0 + j, i, 3 - j), ball_y_Bcorsegp++;

			if (ary_RB[0 + j][3 - j][i] == whose_turn)
				ball_z_Bcorse++;
			else if (ary_RB[0 + j][3 - j][i] == 0)
				AI_good_point(5, 0 + j, 3 - j, i), ball_z_Bcorsegp++;
		}

		if (ball_x_corse >= check_point && ball_x_corse + ball_x_corsegp == 4 && Has_Gpoint[0] == 1){
			AI_put_point(0); return true;
		}
		else if (ball_y_corse >= check_point && ball_y_corse + ball_y_corsegp == 4 && Has_Gpoint[1] == 1){
			AI_put_point(1); return true;
		}
		else if (ball_z_corse >= check_point && ball_z_corse + ball_z_corsegp == 4 && Has_Gpoint[2] == 1){
			AI_put_point(2); return true;
		}
		else if (ball_x_Bcorse >= check_point && ball_x_Bcorse + ball_x_Bcorsegp == 4 && Has_Gpoint[3] == 1){
			AI_put_point(3); return true;
		}
		else if (ball_y_Bcorse >= check_point && ball_y_Bcorse + ball_y_Bcorsegp == 4 && Has_Gpoint[4] == 1){
			AI_put_point(4); return true;
		}
		else if (ball_z_Bcorse >= check_point && ball_z_Bcorse + ball_z_Bcorsegp == 4 && Has_Gpoint[5] == 1){
			AI_put_point(5); return true;
		}
		else
			ball_x_corse = 0, ball_y_corse = 0, ball_z_corse = 0,
			ball_x_Bcorse = 0, ball_y_Bcorse = 0, ball_z_Bcorse = 0, //recount
			ball_x_corsegp = 0, ball_y_corsegp = 0, ball_z_corsegp = 0,
			ball_x_Bcorsegp = 0, ball_y_Bcorsegp = 0, ball_z_Bcorsegp = 0;

		if (ary_RB[0 + i][0 + i][0 + i] == whose_turn)
			ball_3Dcorse1++;
		else if (ary_RB[0 + i][0 + i][0 + i] == 0)
			AI_good_point(0, 0 + i, 0 + i, 0 + i), ball_3Dcorse1gp++;

		if (ary_RB[0 + i][0 + i][3 - i] == whose_turn)
			ball_3Dcorse2++;
		else if (ary_RB[0 + i][0 + i][3 - i] == 0)
			AI_good_point(1, 0 + i, 0 + i, 3 - i), ball_3Dcorse2gp++;

		if (ary_RB[0 + i][3 - i][3 - i] == whose_turn)
			ball_3Dcorse3++;
		else if (ary_RB[0 + i][3 - i][3 - i] == 0)
			AI_good_point(2, 0 + i, 3 - i, 3 - i), ball_3Dcorse3gp++;

		if (ary_RB[0 + i][3 - i][0 + i] == whose_turn)
			ball_3Dcorse4++;
		else if (ary_RB[0 + i][3 - i][0 + i] == 0)
			AI_good_point(3, 0 + i, 3 - i, 0 + i), ball_3Dcorse4gp++;
	}

	if (ball_3Dcorse1 >= check_point && ball_3Dcorse1 + ball_3Dcorse1gp == 4 && Has_Gpoint[0] == 1){
		AI_put_point(0); return true;
	}
	else if (ball_3Dcorse2 >= check_point && ball_3Dcorse2 + ball_3Dcorse2gp == 4 && Has_Gpoint[1] == 1){
		AI_put_point(1); return true;
	}
	else if (ball_3Dcorse3 >= check_point && ball_3Dcorse3 + ball_3Dcorse3gp == 4 && Has_Gpoint[2] == 1){
		AI_put_point(2); return true;
	}
	else if (ball_3Dcorse4 >= check_point && ball_3Dcorse4 + ball_3Dcorse4gp == 4 && Has_Gpoint[3] == 1){
		AI_put_point(3); return true;
	}
	else
		ball_3Dcorse1 = 0, ball_3Dcorse2 = 0, ball_3Dcorse3 = 0,
		ball_3Dcorse4 = 0, ball_3Dcorse1gp = 0, ball_3Dcorse2gp = 0,
		ball_3Dcorse3gp = 0, ball_3Dcorse4gp = 0;
		//recount




	return false;
}

void Draw_LineBox (void) {
	Set_line();
	glLineWidth (2.0);
	glBegin (GL_LINES);
	//glColor3f (1,0,0); // X axis is red.
	//glColor3f (0,1,0); // Y axis is green.
	//glColor3f (0,0,1); // z axis is blue.

	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++){
			glColor3f(0, 0, 1);
			glVertex3fv(PX_S[i][j]);
			glVertex3fv(PX_E[i][j]);
			glColor3f(0, 1, 0);
			glVertex3fv(PY_S[i][j]);
			glVertex3fv(PY_E[i][j]);
			//glColor3f(4, 0, 0);
			//glVertex3fv(PZ_S[i][j]);
			//glVertex3fv(PZ_E[i][j]);
		}

	glEnd();
}

void Draw_Axes(void) {
	glLineWidth(6.0);
	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3fv(XP);
	glVertex3fv(XN);
	glColor3f(0, 1, 0);
	glVertex3fv(YP);
	glVertex3fv(YN);
	glColor3f(4, 0, 0);
	glVertex3fv(ZP);
	glVertex3fv(ZN);

	glEnd();
}

/* Create display list with Torus and initialize state*/
static void init(void) {
	// Display List
	theObject = glGenLists (1); //creates a display list
	glNewList(theObject, GL_COMPILE); // Define the display list
	glEndList();  // Define the display list (End)
}
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window 
	
	selectFont(50, ANSI_CHARSET, "Comic Sans MS");

	if (at_menu == 1){
		glColor3f(0, 1, 1);
		glRasterPos3f(-2, 3, 0);
		drawString("Tic-Tac-Toe 3D*");
		glRasterPos3f(0, 0, 0);
		drawString("One player*");
		glRasterPos3f(0, -2, 0);
		drawString("Two players*");
		if (select_com == 1){
			glRasterPos3f(-2, 0, 0);
			drawString("->*");
		}
		else{
			glRasterPos3f(-2, -2, 0);
			drawString("->*");
		}
	}
	else{

		glColor3f(0, 0, 1);
		glRasterPos3f(15, 0, 0);
		drawString("X*");

		glColor3f(0, 1, 0);
		glRasterPos3f(0, 15, 0);
		drawString("Y*");

		glColor3f(8, 1, 1);
		glRasterPos3f(0, 0, 15);
		drawString("Z*");

		Draw_Axes();
		Draw_LineBox();

		if (is_AI == 1 && turn_RB == -1){//------AI turn
			if (AI_select(1, 3) == false)
				if (AI_select(-1, 3) == false)
					if (AI_select(1, 2) == false)
						if (AI_select(-1, 2) == false) //AI move
							AI_select(1, 1);
			turn_RB = 1;
		}

		if (win_RB != 2){
			if (Check_Win(-1 * turn_RB))
				if (-1 * turn_RB == 1)
					win_RB = 1;
				else
					win_RB = -1;
		}
	
		glColor3f(100, 100, 100);
		glRasterPos3f(0, 0, 0);
		if (win_RB == 1){
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window 
			drawString("Red player Win!!*");
			End_flag = 1;
			win_RB = 2; //stop check
		}
		else if (win_RB == -1){
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window 
			drawString("Blue player Win!!*");
			End_flag = 1;
			win_RB = 2; //stop check
		}
			
		//glutSwapBuffers();

		if (if_reset == 0)
			reset_ary_RB();

		if (End_flag != 1){
			
				//-----------------redraw Red and Blue
				for (int i = 0; i < 4; i++)
					for (int j = 0; j < 4; j++)
						for (int k = 0; k < 4; k++){

					glPushMatrix();
					if (ary_RB[i][j][k] == 1){
						glColor3f(2.3f, 0.2f, 0.1f); // Object Color   Red
						glTranslatef(6 - 4 * i, 6 - 4 * j, 6 - 4 * k); //  Object Position
						glutSolidSphere(2, 20, 20); // Plot Solid Objects
					}
					else if (ary_RB[i][j][k] == -1){
						glColor3f(0.9f, 0.4f, 0.7f); // Object Color   Blue
						glTranslatef(6 - 4 * i, 6 - 4 * j, 6 - 4 * k); //  Object Position
						glutSolidSphere(2, 20, 20); // Plot Solid Objects
					}
					glPopMatrix();
						}
						
				if (End_flag == 0){
					//---------------------------------selection
					if (slt_w == 0){
						for (int i = 0; i < 4; i++){
							glPushMatrix();//---x
							glColor3f(0.5f, 1.0f, 0.9f); // selection Color   
							glTranslatef(6 - slt_x * 4, 9, 6 - i * 4); //  Object Position
							glutSolidOctahedron();
							glPopMatrix();

							glPushMatrix();
							glTranslatef(6 - slt_x * 4, -9, 6 - i * 4); //  Object Position
							glutSolidOctahedron();
							glPopMatrix();
						}
					}

					if (slt_w == 1 || slt_w == 2){
						float slt_y_line_S[3] = { 6 - slt_x * 4, 6 - slt_y * 4, 9 },
							slt_y_line_E[3] = { 6 - slt_x * 4, 6 - slt_y * 4, -9 };
						glLineWidth(10.0);
						glBegin(GL_LINES);
						glColor3f(1, 1, 1);
						glVertex3fv(slt_y_line_S);
						glVertex3fv(slt_y_line_E);
						glEnd();

						glPushMatrix();//---y
						glColor3f(0.5f, 1.0f, 0.9f); // selection Color 
						glTranslatef(6 - slt_x * 4, 6 - slt_y * 4, 9); //  Object Position
						glutSolidCube(2);
						glPopMatrix();

						glPushMatrix();//---y
						glTranslatef(6 - slt_x * 4, 6 - slt_y * 4, -9); //  Object Position
						glutSolidCube(2);
						glPopMatrix();

						for (int i = 0; i < 12; i++){
							glPushMatrix();//---y
							glTranslatef(6 - slt_x * 4, 6 - slt_y * 4, -9 + i*1.5); //  Object Position
							glutSolidCube(1.5);
							glPopMatrix();
						}
					}

					if (slt_w == 2){
						glPushMatrix();
						glColor3f(0.5f, 1.0f, 0.9f); // selection Color  
						glTranslatef(6 - slt_x * 4, 6 - slt_y * 4, 6 - slt_z * 4); //  Object Position
						glutSolidSphere(2.1, 20, 20);
						//glutSolidCube(4); // Plot Solid Objects
						//glutWireTorus(0.25, 0.75, 50, 50);
						glPopMatrix();

						//glPushMatrix();//---z 
						//glColor3f(0.5f, 1.0f, 0.9f); // selection Color 
						//glTranslatef(9, 6 - slt_y * 4, 6 - slt_z * 4); //  Object Position
						//glutSolidOctahedron();
						//glPopMatrix();

						//glPushMatrix();//---z 
						//glTranslatef(-9, 6 - slt_y * 4, 6 - slt_z * 4); //  Object Position
						//glutSolidOctahedron();
						//glPopMatrix();
					}
				}
		}
		//Draw_Axes (); //ori place
	}

	glFlush();
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat) w/(GLfloat) h, 1.0, 80.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 30, 0, 0, 0, 0, 1, 0);
}
/* Rotate about x-axis when "x" typed; rotate about y-axis when "y" typed; "i" returns torus to original view */
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'x':
	case 'X':
		if (at_menu != 1){
			glRotatef(-15., 1.0, 0.0, 0.0);
			glutPostRedisplay();
		}
		break;
	case 'y':
	case 'Y':
		if (at_menu != 1){
			glRotatef(15., 0.0, 1.0, 0.0);
			glutPostRedisplay();
		}
		break;
	case 'z':
	case 'Z':
		if (at_menu != 1){
			glRotatef(15, 0.0, 0.0, 1.0);
			glutPostRedisplay();
		}
		break;
	case 'i':
	case 'I':
		if (at_menu != 1){
			glLoadIdentity();
			gluLookAt(0, 0, 20, 0, 0, 0, 0, 1, 0);
			glutPostRedisplay();
		}
		break;
	case 'u':
	case 'U':
		if (at_menu != 1){
			glLoadIdentity();
			gluLookAt(0, 0, 30, 0, 0, 0, 0, 1, 0);
			glutPostRedisplay();
		}
		break;
	case 32: //----------change xyz
		if (at_menu != 1){
			if (slt_w == 0)
				slt_x = (slt_x + 1) % 4;
			else if (slt_w == 1)
				slt_y = (slt_y + 1) % 4;
			else
				slt_z = (slt_z + 1) % 4;
		}
		else
			select_com = (select_com + 1) % 2;
		
		glutPostRedisplay();
		break;
	case 13: //enter
		if (End_flag == 2){
			End_flag = 0;
			at_menu = 3;
		}
		else if (End_flag == 1){
			End_flag = 2;
		}
		if (at_menu == 0){
			if (slt_w == 2){ //------finish select
				if (ary_RB[slt_x][slt_y][slt_z] == 0){
					ary_RB[slt_x][slt_y][slt_z] = turn_RB;
		
					if (Check_Win(turn_RB))
						is_AI = 0;
					
					turn_RB *= -1;
				}
				else
					slt_w = (slt_w - 1) % 3;
			}
			slt_w = (slt_w + 1) % 3;
		}
		else if (at_menu == 1){
			at_menu = 0;
			if (select_com == 1)
				is_AI = 1;
			else
				is_AI = 0;
		}
		else if (at_menu == 3){
			glLoadIdentity();
			gluLookAt(0, 0, 30, 0, 0, 0, 0, 1, 0);
			at_menu = 1, select_com = 1, End_flag = 0;
			is_AI = 1, turn_RB = 1, win_RB = 0;
			reset_ary_RB();
			slt_x = 0, slt_y = 0, slt_z = 0, slt_w = 0;
		}
		glutPostRedisplay();
		break;
	case 8: //back to last step
		if (at_menu != 1){
			slt_w--;
			if (slt_w < 0) slt_w = 0;
			glutPostRedisplay();
		}
		break;
	case 27:
		exit(0);
		break;
	}
}
int main(int argc, char **argv) {
	glutInitWindowSize(950, 700);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Tic-Tac-Toe 3D");
	//glutCreateWindow(argv[0]);
	init();
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	
	
	// LIGHTING and MATERIAL Properties
	//GLfloat light_position[] = { 1.0, 1.0, 1.0, 1.0 }; // light position (x,y,z,w)
	GLfloat light_position[] = { 10.0, 0.0, 0.0, 0.0 }; // light position (x,y,z,w)
	GLfloat light[] = {0.5, 0.5, 0.0 }; // Light Source Color
	GLfloat lmodel_ambient[] = { 0.1, 0.5, 0.7, 1.0 }; // Ambient Light Color

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 }; //
	GLfloat mat_shininess[] = { 50.0 }; //
	
	glShadeModel (GL_SMOOTH);

	// Light
	glEnable(GL_LIGHTING); // 2) enable Phong lighting calculation
	glEnable(GL_LIGHT0); // 2) up to eight point sources (radiating in all directions)
	glEnable(GL_DEPTH_TEST); // 2) Depth Perception
	
	// Position light
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); // 2a) set light position

	glEnable(GL_COLOR_MATERIAL); // 3) enable color driven materials
	//
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light ); // 4) diffuse
	glLightfv(GL_LIGHT0, GL_SPECULAR, light ); // 4) specular
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient); // 4) ambient
	//
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); // 5) set material properties 
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); // 5) set material properties
	
	glClearColor (0.0, 0.0, 0.0, 0.0);
	
	
	glutMainLoop();
	return 0;
}

