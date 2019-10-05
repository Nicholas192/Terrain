#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <tgmath.h>
#include <math.h>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

//Missing vertex normals
//Globals
float lengX = 100;
float lengZ = 100;

int iter = 200;

float circleSize = 0;
float circleDist = 5;
float disp = 0;
float hJum = 0;
float jX = 2;
float jZ = 2;

//float camPos[] = {-lengX, lengZ, lengZ/2};	//where the camera is
float camPos[] = {-lengX, lengZ/4, -lengZ};	//where the camera is
float camSpeed = 1.0f;
float eye[] = {lengX/2, 0, lengZ/2};

float angleX = 45;
float angleY = 0;
float angleZ = 0;
float rotL0 = 0;
float rotL1 = 0;
float panY = 0;
float panX = 0;
float panZ = 0;

bool tringle = true;
bool fill = true;
bool both = false;
bool lights = true;
bool flat = false;

float theta = 0;
//light
float amb0[] = {1,1,1,1};
float diff0[] = {0.5, 0.5, 0.5, 1};
float spec0[] = {1,1,1,1};
float pos0[] = {lengX, lengZ, lengZ, 0.0};//position
float sd0[] = {-lengX, lengZ, lengZ/2};//direction

float amb1[] = {1,1,1,1};
float diff1[] = {0.6, 0.4, 0.4, 1};
float spec1[] = {0.7,0,0,1};
float pos1[] = {-lengX, lengZ, -lengZ, 0.0};//position
float sd1[] = {-lengX * 2, 0, -lengZ * 2};//direction


//material
float m_amb1[] = {0, 0.05, 0.0, 1};
float m_diff1[] = {0.4, 0.5, 0.4, 1};
float m_spec1[] = {0.4, 0.7, 0.4, 1};
float shiny1 = 0.078125;


//circle friend's globals
float frLoc[] = {0,1.5,0};//friend's location
float frMap[] = {1,1,1};//friend's location in relation to map
int ang = 0;//angle representing direction he is facing
int cnt = 0;
bool jumping = false;
bool landed = false;
bool focusing = false;
bool awaken = false;



//friend head
//float m_amb2[] = {0.012, 0.023, 0.042, 1};
//float m_diff2[] = {0.43, 0.74, 0.91, 1};
//float m_spec2[] = {0.42, 0.70, 0.90, 1};
float m_amb2[] = {0.01, 0.023, 0.08, 1};
float m_diff2[] = {0.20, 0.30, 0.91, 1};
float m_spec2[] = {0.30, 0.40, 0.91, 1};
//float m_amb2[] = {0.23, 0.23, 0.23, 1};
//float m_diff2[] = {0.74, 0.74, 0.74, 1};
//float m_spec2[] = {0.85, 0.85, 0.85, 1};
float shiny2 = 20.8;

//friend eye
float m_amb3[] = {0.05, 0.05, 0.05, 1};
float m_diff3[] = {0.5, 0.5, 0.5, 1};
float m_spec3[] = {0.7, 0.7, 0.7, 1};
float shiny3 = 0.078125;

//friend pupil
float m_amb4[] = {0.02, 0.02, 0.02, 1};
float m_diff4[] = {0.01, 0.01, 0.01, 1};
float m_spec4[] = {0.4, 0.4, 0.4, 1};
float shiny4 = 0.078125;

float maxH = -999999999.9;
float minH = 999999999.9;
std::vector<std::vector<float>> heightMap;
std::vector<std::vector<std::vector<float>>> storedNormal;

int xSize;
int zSize;

void makeFlat(int x, int z){
	for (int i = 0; i < x+2; i++){
		std::vector<float> temp;
		std::vector<std::vector<float>> thum;
		for (int j = 0; j < z+2; j++){
			temp.push_back(0.0);
			//thum.push_back({0,0,0});
			//storedNormal[i][j].push_back(thum);
			std::vector<float> tram;
			for (int k = 0; k < 3; k++){
				tram.push_back(0.0);
			}
			thum.push_back(tram);
		}
		heightMap.push_back(temp);
		storedNormal.push_back(thum);
	}
	xSize = heightMap.size();
	zSize = heightMap[1].size();
}

void resetFlat(int x, int z){
	for (int i = 0; i < x+2; i++){
		for (int j = 0; j < z+2; j++){
			heightMap[i][j] = 0.0;
			for (int k = 0; k < 3; k++){
				storedNormal[i][j][k] = 0.0;
			}
		}
	}
	minH = 999999999.9;
	maxH = -999999999.9;
}


void makeHeight(int iterations, int circSize, int height, int dispCopy, int x, int z){
	srand(time(NULL));
	int pt[2] = {x, z};
	//float circleDist;// = rand() % 100;
	//int circSize;// = rand() % 100;
	//int dispCopy;
	//int height;

	int radd;
	
	//int xSize = heightMap.size(F);
	//int zSize = heightMap[1].size();
	//disp = zSize;
	//for each terrain point (tx,tz) do	
	//pd = distance from circle center * 2 / terrainCircleSize
	//if fabs(pd) <= 1.0
	//	height(tx,tz) +=  disp/2 + cos(pd*3.14)*disp/2;
	//int iterations = 3 + (int) rand % 17;
	for (int j = 0; j < iterations; j++){
		if (landed == false){
			pt[0] = (int) rand() % xSize;//(int)heightMap.size();
			pt[1] = (int) rand() % zSize;//(int)heightMap[tx].size();
			height = xSize/10 - (int) rand() % xSize/5;//(int)heightMap.size() / 2;
			radd = (int) (rand() % 20)+1;
			//circleDist = rand() % (int) lengX;
			circSize = 1 + (int) rand() % (int) 20;//xSize/2;
			dispCopy = lengZ/10;// (int) (rand()%12)+1;
		}
		else if (landed == true){
			//pt[0] = (int) rand() % xSize;//(int)heightMap.size();
			//pt[1] = (int) rand() % zSize;//(int)heightMap[tx].size();
			height = hJum;
			radd = (int) (rand() % 20)+1;
			circSize = circleSize;
			dispCopy = disp;
		}
		for (int tx = 1; tx < xSize - 1; tx++){//heightMap.size(); tx++){
			for (int tz = 1; tz < zSize - 1; tz++){//heightMap[tx].size(); tz++){
				int dx = tx - pt[0];
				int dz = tz - pt[1];
				/*if (landed == false){
					dz = tz - pt[1];
					dx = tx - pt[0];
				}
				else{
					dx = pt[0];
					dz = pt[1];
				}*/

				float circleDist = ((sqrtf(pow(dx,2) + pow(dz,2))));
				float pd = circleDist * 2 / circSize;
				if (fabs(pd) <= 1.0){
					heightMap[tx][tz] +=  dispCopy/2 + cos((pd*3.14159265358979323846))*(dispCopy/2);
				}
				//}
			}
		}
	}


	for(int x=0; x< xSize; x++){                       
        for(int z=0; z< zSize; z++){
            if(heightMap[x][z] < minH){
                minH = heightMap[x][z];
            }
			if(heightMap[x][z] > maxH){
                maxH = heightMap[x][z];
            }
            
        }
	}
	//printf("%f", minH);
	//printf("%f", maxH);
}

void saveNormal(){
	float vec1[3];
	float vec2[3];
	float vec3[3];

	int nxSize = storedNormal.size();
	int nzSize = storedNormal[1].size();
	//int xSize = heightMap.size();
	//int zSize = heightMap[1].size();

	for(int x = 0; x < nxSize-1; x++){
		for(int z = 0; z < nzSize-1; z++){
			vec1[0] = x + 1;
            vec1[1] = heightMap[x + 1][z] - heightMap[x][z];
			vec1[2] = z;
			vec2[0] = x + 1;
			vec2[1] = heightMap[x + 1][1 + z] - heightMap[x][z];
			vec2[2] = z + 1;

			//cross product
			vec3[0] = (vec1[1] * vec2[2]) - (vec2[1] * vec1[2]);
			vec3[1] = (vec1[0] * vec2[2]) - (vec2[0] * vec1[2]);
			vec3[2] = (vec1[0] * vec2[1]) - (vec2[0] * vec1[1]);

			//float vLength = sqrtf(pow(vec3[0], 2) + pow(vec3[1], 2) + pow(vec3[2], 2) ); 
			float vLength = sqrtf((vec3[0] * vec3[0]) + (vec3[1] * vec3[1])+ (vec3[2] * vec3[2]) ) + 1; 

			float normal[3];
			storedNormal[x][z][0] = vec3[0] / vLength;
			storedNormal[x][z][1] = vec3[1] / vLength;
			storedNormal[x][z][2] = vec3[2] / vLength;
			normal[0] = vec3[0] / vLength;
			normal[1] = vec3[1] / vLength;
			normal[2] = vec3[2] / vLength;
			
			//printf("%f", storedNormal[x][z][0]);
			//std::vector<float> thum;//(normal[0], normal[1], normal[2]);
			//thum.push_back(normal[0]);
			//thum.push_back(normal[1]);
			//thum.push_back(normal[2]);
			//storedNormal[x][z].push_back(thum);
			//storedNormal[x][z].push_back(normal[0]);
			//storedNormal[x][z].push_back(normal[1]);
			//storedNormal[x][z].push_back(normal[2]);
		}
	}
}


void drawTerrain(bool f, bool l){
	

	if (f == true){
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	} else if (f == false) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	for (int i = 0; i < xSize-1; i++){
		for (int j = 0; j < zSize-1; j++){
			if (tringle == true){
				glBegin(GL_TRIANGLE_STRIP);
			}
			if (tringle == false){
				glBegin(GL_QUAD_STRIP);
			}
			
			float heightCol;
			/*if (i == xSize - 3){
				if (j == zSize - 3){
					heightCol = (heightMap[i+1][j+1] - minH)/(maxH-minH);
				}else{
					heightCol = (heightMap[i+1][j] - minH)/(maxH-minH);
				}
			}else{
				if (j == zSize - 3){
					heightCol = (heightMap[i][j+1] - minH)/(maxH-minH);
				}else{
					heightCol = (heightMap[i][j] - minH)/(maxH-minH);
				}
			}*/
			heightCol = (heightMap[i][j] - minH)/(maxH-minH);
			glColor3f(heightCol, 1.0 - heightCol, 0);
			if (heightCol >= 0.9){
				glColor3f(1, 1, 1);
			}
			else{
				glColor3f(heightCol + 0.1, 1.0 - heightCol, 0);
			}
			if (l == true){ 
				//glEnable(GL_BLEND);
				/*float m_amb[] = { 0, 0, 0, 1};
				float m_diff[] = { 0, 0, 0, 1};
				float m_spec[] = { 0, 0, 0, 1};
				float shiny = 57.8;*/
				/*float m_amb[] = { (heightCol), (1 - heightCol), 0};
				float m_diff[] = { (heightCol), (1 - heightCol), 0};
				float m_spec[] = { (heightCol), (1 - heightCol), 0};
				float shiny = 57.8;*/

				
			

				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  m_amb1); 
        		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  m_diff1); 
    	    	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  m_spec1); 
	        	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  shiny1);
			}
			//
			glNormal3f(storedNormal[i][j][0], storedNormal[i][j][1], storedNormal[i][j][2] );
			glVertex3i(i - lengX / 2, heightMap[i][j], j - lengZ / 2);
			glNormal3f(storedNormal[i][j+1][0], storedNormal[i][j+1][1], storedNormal[i][j+1][2] );
			glVertex3i(i - lengX/2, heightMap[i][j+1], j+1 -lengZ/2);
			glNormal3f(storedNormal[i+1][j][0], storedNormal[i+1][j][1], storedNormal[i+1][j][2] );
			glVertex3i(i+1 - lengX/2, heightMap[i+1][j], j - lengZ/2);
			glNormal3f(storedNormal[i+1][j+1][0], storedNormal[i+1][j+1][1], storedNormal[i+1][j+1][2] );
			glVertex3i(i+1 -lengX/2, heightMap[i+1][j+1], j+1 -lengZ/2);
			glEnd();
		}
	}
}

void drawFriend(){
	//draw the head
	if (lights == true){
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  m_amb2); 
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  m_diff2); 
    	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  m_spec2); 
	    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  shiny2);
	}
	
	
	glPushMatrix();
    glRotatef(ang, 0, 1, 0);
	glPushMatrix();
	glColor3f(0, 0, 1);
	glScalef(1.5,1.5,1.5);
	glutSolidSphere(1, 100, 100);
	glPopMatrix();

	//left eyeball
	glPushMatrix();

	if (lights == true){
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  m_amb3); 
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  m_diff3); 
    	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  m_spec3); 
	    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  shiny3);
	}

	glTranslatef(0.8, 0.8, 1.2);
	glScalef(0.4,0.4,0.4);
	glColor3f(1.0,1.0,1.0);
	glutSolidSphere(1, 100, 100);

	//left pupil
	glPushMatrix();

	if (lights == true){
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  m_amb4); 
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  m_diff4); 
    	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  m_spec4); 
	    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  shiny4);
	}

	glTranslatef(0, 0.6, 0.75);
	glScalef(0.2,0.2,0.2);
	glColor3f(0,0,0);
	glutSolidSphere(1, 100, 100);
	glPopMatrix();
	glPopMatrix();
	//how the TA wants you to do it
	
	//right eyeball
	glPushMatrix();
	if (lights == true){
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  m_amb3); 
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  m_diff3); 
    	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  m_spec3); 
	    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  shiny3);
	}
	glTranslatef(-0.8, 0.8, 1.2);
	glScalef(0.4,0.4,0.4);
	glColor3f(1.0,1.0,1.0);
	glutSolidSphere(1, 100, 100);
	//right pupil

	if (lights == true){
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  m_amb4); 
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  m_diff4); 
    	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  m_spec4); 
	    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  shiny4);
	}

	glTranslatef(0, 0.6, 0.75);
	glScalef(0.2,0.2,0.2);
	glColor3f(0,0,0);
	glutSolidSphere(1, 100, 100);
	glPopMatrix();

	glPopMatrix();
}

//OpenGL functions
void keyboard(unsigned char key, int xIn, int yIn)
{
	int mod = glutGetModifiers();
	switch (key)
	{
		case 'q':
		case 27:	//27 is the esc key
			exit(0);
			break;


		case 't':
			tringle = true;
			break;

		case 'y':
			tringle = false;
			break;

		case 'w':
			if (fill == true && both == false){
				fill = false;
			}
			else if (fill == false && both == false){
				both = true;
			}
			else if (fill == false && both == true){
				fill = true;
				both = false;
			}
			break;

		case 'l':
			lights = !lights;
			//glLoadIdentity();
			break;

		case 'r':
			srand(time(NULL));
			resetFlat(lengX, lengZ);
			makeHeight(iter, 1 + (int) rand() % (int) 20, xSize/10 - (int) rand() % xSize/5, lengZ/10, (int) rand() % xSize, (int) rand() % zSize);
			saveNormal();
			glutPostRedisplay();
			break;

		case 'p':
			flat = !flat;
			break;

		case 'j':
			//if (panY < maxH * 0.8){panY += 1;}
			panY += 1;
			//eye[1]++;
			break;

		case 'm':
			if (panY > -maxH * 0.8){panY -= 1;}
			panY -= 1;
			//eye[1]--;
			break;

		case 'h':
			//if (panX < lengX/2){panX += 1;}
			panX += 1;
			break;

		case 'n':
//			if (panX > -lengX/2){panX -= 1;}
			panX -= 1;
			break;

		case 'g':
			//if (panZ < lengZ/2){panZ += 1;}
			panZ += 1;
			break;

		case 'b':
			//if (panZ > -lengZ/2){panZ -= 1;}
			panZ -= 1;
			break;

		case 'u':
			rotL0 -= 1;
			break;

		case 'i':
			rotL0 += 1;
			break;

		case 'k':
			rotL1 -= 1;
			break;

		case 'o':
			rotL1 += 1;
			break;

		case 91:
			if (angleZ > -15){angleZ -= 1;}
			break;

		case 93:
			if (angleZ < 15){angleZ += 1;}
			break;

		case 49:
			circleSize++;
			printf("The circle size for every mountain Orb Friend will make is set to: %f", circleSize);
			printf("\n");
			break;

		case 50:
			circleSize--;
			printf("The circle size for every mountain Orb Friend will make is set to: %f", circleSize);
			printf("\n");
			break;

		/*case 51:
			circleDist++;
			printf("The circle dist for every mountain Orb Friend will make is set to: %f", circleSize);
			printf("\n");
			break;

		case 52:
			circleDist--;
			printf("The circle dist for every mountain Orb Friend will make is set to: %f", circleSize);
			printf("\n");
			break;*/

		case 53:
			disp++;
			printf("The disp for every mountain Orb Friend will make is set to: %f", circleSize);
			printf("\n");
			break;

		case 54:
			disp--;
			printf("The disp for every mountain Orb Friend will make is set to: %f", circleSize);
			printf("\n");
			break;

		case 48:
			disp = 0; circleSize = 0;
			//printf("The disp for every mountain Orb Friend will make is set to: %f", circleSize);
			//printf("\n");
			break;

		case 'x':
			if (frLoc[2] > -lengZ/2){
				frLoc[2] -= 1; ang = 180;}
			break;

		case 's':
			if (frLoc[2] < lengZ/2){
				frLoc[2] += 1; ang = 0;}
			break;
		
		case 'c':
			if (frLoc[0] > -lengX/2){
				frLoc[0] -= 1; ang = 270;}
			break;

		case 'z':
			if (frLoc[0] < lengX/2){
				frLoc[0] += 1; ang = 90;}
			break;

		case 'v':
			angleX = 0; angleY = 0; angleZ = 0;
			panX = 0; panY = 0; panZ = 0;
			break;

		case 32:
			if(jumping == false && landed == false){jumping = true;}
			break;
		
		case 'd':
			if (focusing == false && awaken == false){focusing = true;}
			break;

		case 'f':
			resetFlat(lengX, lengZ);
			break;
	}
}

void init(void)
{
	/*glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 1000);*/
}

/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */

void display(void)
{
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, lengX * lengZ);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	

	if (fill == true){
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	} else if (fill == false) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	gluLookAt(camPos[0], camPos[1], camPos[2], eye[0], eye[1], eye[2], 0,1,0);

	
	
	saveNormal();
	
	glPushMatrix();

	glTranslatef(panX, panY, panZ);
	glPushMatrix();
	
	if (jumping == true){
		theta += 1;
		cnt = 16 * sin(theta/14);
	}
	
	//glTranslatef(lengX /2, 0, lengZ/4);
	glPushMatrix();
	glRotatef(angleX, 0, 1, 0);
	glRotatef(angleY, 1, 0, 0);
	glRotatef(angleZ, 0, 0, 1);
	//glTranslatef(-lengX/2, 0, -lengZ/2);
		
	frMap[0] = frLoc[0] + lengX/2;
	frMap[2] = frLoc[2] + lengZ/2;
	frMap[1] =  heightMap[frMap[0]][frMap[2]] + frLoc[1];
		if (both == false){
			drawTerrain(fill, lights);
			glPushMatrix();
			if (cnt == 0 && jumping == true){
				jumping = false;
				landed = true;
			}
			glTranslatef(frLoc[0], frMap[1] + cnt, frLoc[2]);
			if (landed == true){
				makeHeight(1, circleSize, hJum, disp, frMap[0], frMap[2]);
				landed = false;
				theta = 0;
			}
			drawFriend();
			glPopMatrix();
		}
		else if (both == true){
			drawTerrain(true, lights);
			glPushMatrix();
			if (cnt == 0 && jumping == true){
				jumping = false;
				landed = true;
			}
			glTranslatef(frLoc[0], frMap[1] + cnt, frLoc[2]);
			if (landed == true){
				makeHeight(1, circleSize, hJum, disp, frLoc[0], frLoc[2]);
				landed = false;
				theta = 0;
			}
			drawFriend();
			glPopMatrix();
			glTranslatef(0, maxH + lengX/3, 0);
			drawTerrain(false, lights);
		}

		

		if (lights == true)
		{
			glEnable(GL_LIGHTING); 
			if (flat == false){
	    		glShadeModel(GL_SMOOTH);
			}
			else {
				glShadeModel(GL_FLAT);
			}

		

			glPushMatrix();
			//glTranslatef(-lengX,lengZ/2,-lengZ);
			glRotatef(rotL0,0,1,0);
			float lightpos0[4] = {pos0[0] + lengX,pos0[1] - lengZ/2,pos0[2] + lengZ,1};
    		glEnable(GL_LIGHT0); 
			glLightfv(GL_LIGHT0, GL_POSITION, lightpos0);
    		glLightfv(GL_LIGHT0, GL_AMBIENT, amb0); 
    		glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0); 
	    	glLightfv(GL_LIGHT0, GL_SPECULAR, spec0); 
			glPopMatrix();
			//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, sd0); 
			//glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 90.0); 
			//glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 5.0);

			glPushMatrix();
			//glTranslatef(-lengX,lengZ/2,-lengZ);
			glRotatef(rotL1,0,1,0);
			float lightpos1[4] = {pos1[0] - lengX, pos1[1] + lengZ/2, pos1[2] - lengZ,1};
			//glTranslatef(-lengX, lengZ, -lengZ);
    		glEnable(GL_LIGHT1); 
			glLightfv(GL_LIGHT1, GL_POSITION, lightpos1);
	    	glLightfv(GL_LIGHT1, GL_AMBIENT, amb1); 
    		glLightfv(GL_LIGHT1, GL_DIFFUSE, diff1); 
    		glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);
			glPopMatrix(); 

			//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, sd1); 
			//glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 90.0); 
			//glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 5.0);
	}
	else if (lights == false){
		glDisable(GL_LIGHTING);
	}
	glPopMatrix();
	
	glPopMatrix();
	glPopMatrix();

	
	//switch our buffers for a smooth animation
	glutSwapBuffers();
}

void special(int key, int x, int y){
	switch (key){
	case GLUT_KEY_UP:
		printf("Up Arrow was pressed!\n");
		if (angleY > -15){angleY -= 1;}
		break;

	case GLUT_KEY_DOWN:
		printf("Down Arrow was pressed!\n");
		if (angleY < 15){angleY += 1;}
		break;

	case GLUT_KEY_RIGHT:
		printf("Right Arrow was pressed!\n");
		angleX += 1;
		break;

	case GLUT_KEY_LEFT:
		printf("Left Arrow was pressed!\n");
		angleX -= 1;
		break;
	}

	

}

void mouse(int btn, int state, int x, int y){
	printf("mouseFunc coords: %i,%i\n", x, y);

	if (btn == GLUT_LEFT_BUTTON){
//		printf("Left button\n");

		if (state == GLUT_UP){
//			printf("Released\n");
		}

		if (state == GLUT_DOWN){
//			printf("Pressed\n");
		}
	}
}

void motion(int x, int y){
//	printf("mouseMotion coords: %i,%i\n", x, y);
}

void passive(int x, int y){
//	printf("mousePassive coords: %i,%i\n", x, y);
}

void reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0, w, 0, h);
	gluPerspective(45, (float)((w+0.0f)/h), 1, 100);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void FPS(int val){
	glutPostRedisplay();
	glutTimerFunc(17, FPS, 0); // 1sec = 1000, 60fps = 1000/60 = ~17
}

void callBackInit(){
	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passive);
	glutReshapeFunc(reshape);
	glutTimerFunc(0, FPS, 0);
}


void menuProc(int value){
	if (value == 1)
		printf("First item was clicked\n");
	if (value == 2)
		printf("Second item was clicked\n");
	if (value == 3)
		printf("gahhh item was clicked\n");
	if (value == 4)
		printf("gahhhhhh item was clicked\n");
	if (value == 5)
		printf("????? item was clicked\n");
	if (value == 6)
		printf("!!!!! item was clicked\n");
}

//could use a seperate menu processor for submenu!
void menuProc2(int value){

}

void createOurMenu(){
	//int subMenu_id = glutCreateMenu(menuProc2);
	int subMenu_id = glutCreateMenu(menuProc);
	glutAddMenuEntry("gahh", 3);
	glutAddMenuEntry("gahhhhhh", 4);
	glutAddMenuEntry("????", 5);
	glutAddMenuEntry("!!!!!", 6);

	int main_id = glutCreateMenu(menuProc);
	glutAddMenuEntry("First item", 1);
	glutAddMenuEntry("Second item", 2);
	glutAddSubMenu("pink flamingo", subMenu_id);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/* main function - program entry point */
int main(int argc, char** argv)
{


	printf("Welcome to Orb Friend's Terrain Game!!!!!11!!!1!!!1!!\n");
	printf("Enter the x and z grid, and then enter the number of circles to draw initially\n");
	printf("Use the arrow keys and the '[' and ']' keys to rotate the terrian\n");
	printf("Use the keys 'J', 'M', 'H', 'N', 'G' and 'B' to pan the camera to your hearts content (with a limit to prevent you from getting lost)(bonus feature)\n");
	printf("Use the 'V' key to reset the camera position and the rotation of the terrain\n");
	printf("use the 'T' key to enter triangle strip mode, and 'Y' to enter quads stip mode \n");
	printf("Press the 'W' key to toggle the fill modes. Press it once to go from filled to wire frame, again to display both on ontop of the other (the distance between them varies depending on the height of the terrain, so panning may be necessary to see it)\n");
	printf("Press the 'W' key a third time to go back to fill mode\n");
	printf("Press the 'L' key to turn the lights on or off. Notice that when the lights are off, colors represent the height of the terrain.\n");
	printf("Press the 'R' key to reset the terrian with a random landscape, and use the 'F' key to make it completely flat.\n");
	printf("Press the 'P' key to toggle between flat and gouraud shading\n");
	printf("Press the 'U' and 'I' to rotate the white light around the terrain\n");
	printf("Press the 'K' and 'O' to rotate the red light around the terrain\n");
	printf("Press the 'U' and 'I' to rotate the white light around the terrain\n");
	printf("Press the 'S' 'X' 'Z' and 'C' keys to move Orb Freind around the terrain. 'ZOOM!'\n");
	printf("Press the Space Bar to make Orb Friend Jump! 'Wheee!'\n");
	printf("Orb Friend is magical and can change the landscape by jumping on it should he choose to!\n");
	printf("Press the 5 and 6 keys to edit the height of the terrain generated (5 is up, 6 is down)\n");
	printf("Press the 1 and 2 keys to adjust the width of the terrain generated (1 for wider, 2 for thinner)\n");
	printf("Press the 0 key to reset both the height and width of the terrain to 0, their default values\n");
	printf("Note that both the hieght and the width have to be non-zero in order for the terrain to change when Orb Friend lands\n");
	printf("'I hope you enjoy the game, and I would like nothing more than to be your friend!' - Orb Friend\n");
	int tx = 0;
	int tz = 0;
	int ti = 0;
	while (tx < 50 || tx > 300 || /*tz < 50 || tz > 300 ||*/ ti < 150 || ti > 500){
		std::cout << "enter the x diemension of the grid (50 <= tx <= 300)";
		std::cin >> tx;
		/*std::cout << "enter the z diemension of the grid (50 <= tz <= 300)";
		std::cin >> tz;*/
		std::cout << "enter the grid's number of circles (150 <= i <= 500)";
		std::cin >> ti;
	}
	lengX = tx;
	lengZ = tx;
	iter = ti;
	/* First order of buisness, create shape */
	makeFlat(lengX, lengZ);
	//makeHeight(circleDist, circleSize);
	//saveNormal(lengX, lengZ);
	//makeHeight(iter);
	makeHeight(iter, 1 + (int) rand() % (int) 20, xSize/10 - (int) rand() % xSize/5, lengZ/10, (int) rand() % xSize, (int) rand() % zSize);
	//now that the terrain is done we need to create the normal vectors
	//makeNormal();
	//makeFlat(lengX, lengZ);
	glutInit(&argc, argv);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Orb Friend's Terrain Game!!!!!11!!!1!!!1!!");
    
	printf("%i", xSize);
	printf("%i", heightMap.size());
    /* LIGHTING */
    //glEnable(GL_LIGHTING); 
    //glEnable(GL_LIGHT0); 
    //glShadeModel(GL_SMOOTH);
    
	//make the front face culling
	glFrontFace(GL_CCW);
	glEnable(GL_BACK);
	glEnable(GL_CULL_FACE);

	//enable Z buffer test, otherwise things appear in the order they're drawn
	glEnable(GL_DEPTH_TEST);

	//setup the initial view
	// change to projection matrix mode, set the extents of our viewing volume
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
       
	//set clear colour to white
	glClearColor(0, 0, 0, 0);

    
	//register glut callbacks for keyboard and display function
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutSpecialFunc(special);

	callBackInit();

	init();

	//printf("%f", maxH);
	//printf("%f", minH);

	createOurMenu();
	glutMainLoop();				//starts the event glutMainLoop
	return(0);					//return may not be necessary on all compilers
}
