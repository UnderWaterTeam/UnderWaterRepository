#include<windows.h>
#include <fstream>
#include<sstream> 
#include<GL/glut.h>
#include<stdio.h>
#include<string>
#include"Kirchhoff.h"
#include "PIC.h"
#include"DynamicFormula.h"
#include<iostream>
using namespace std;
using namespace Eigen;
/*
��ʱ����glutTimerFunc(unsigned int millis, void (*func)(int value), int value);
�ػ��־��glutPosRedisplay();
��ʱ����һ��������ÿ��millis��������func���������Ҵ�һ��value������ȥ
��Ϊһ����ʱ��ֻ������һ�Σ�������Ҫ��ε��ö�ʱ��
*/
//obj��ȡ   ../source/yuanpan.obj
int id = 0;
long imagewidth = 600;
long imageheight = 800;
string name = "H:\\MeshData\\yuanpan20lab.obj";//tuoyuan.obj yuanpan20 bunnyclose  myprop2
PIC m_pic;
void drawScene();
//���ڵĴ�С
GLfloat windowWidth;
GLfloat windowHeight;
Vector3f omega(	5, 0, 0);
Vector3f velocity(0, 0, -1);			
Matrix3f R = Matrix3f::Identity();//����Ϊ��λ�� ��init()�Ĳ��ǵ�λ��
Vector3f y(0,0,0);
Vector3f ts(0,0,0);
Vector3f fs(0,0,0);
MatrixXf K;
float delta_t=0.1;

DynamicFormula m_DF(omega,velocity,R,y,ts,fs,K,delta_t);
bool mouseLeftDown;
bool mouseRightDown;
float mouseX, mouseY;
float cameraDistance;
float cameraAngleX;
float cameraAngleY;
void ReadPIC()
{
	ifstream ifs(name);//cube bunny Eight
	string s;
	Mian *f;
	Vertex *v;
	FaXiangLiang *vn;
	WenLi  *vt;
	while (getline(ifs, s))
	{
		if (s.length()<2)continue;
		if (s[0] == 'v'){
			if (s[1] == 't'){//vt 0.581151 0.979929 ����
				istringstream in(s);
				vt = new WenLi();
				string head;
				in >> head >> vt->TU >> vt->TV;
				m_pic.VT.push_back(*vt);
			}
			else	if (s[1] == 'n'){//vn 0.637005 -0.0421857 0.769705 ������
				istringstream in(s);
				vn = new FaXiangLiang();
				string head;
				in >> head >> vn->NX >> vn->NY >> vn->NZ;
				m_pic.VN.push_back(*vn);
			}
			else{//v -53.0413 158.84 -135.806 ��
				istringstream in(s);
				v = new Vertex();
				string head;
				in >> head >> v->X >> v->Y >> v->Z;
				m_pic.V.push_back(*v);
			}
		}
		else if (s[0] == 'f'){//f 2443//2656 2442//2656 2444//2656 ��
			for (int k = s.size() - 1; k >= 0; k--){
				if (s[k] == '/')s[k] = ' ';
			}
			istringstream in(s);
			f = new Mian();
			string head;
			in >> head;
			int i = 0;
			while (i<3)
			{//
				if (m_pic.V.size() != 0)
				{
					in >> f->V[i];
					f->V[i] -= 1;
				}
				if (m_pic.VT.size() != 0)
				{
					in >> f->T[i];
					f->T[i] -= 1;
				}
				if (m_pic.VN.size() != 0)
				{
					in >> f->N[i];
					f->N[i] -= 1;
				}
				i++;
			}
			m_pic.F.push_back(*f);
		}
	}
}
void ScreenShot(const char* imagefilepath)
{
	GLint pView[4];
	glGetIntegerv(GL_VIEWPORT, pView);//�õ���ͼ����,pView[2]Ϊ��width,pView[3]Ϊ�߼�height

	GLsizei numComponet = 3;
	GLsizei bufferSize = pView[2] * pView[3] * sizeof(GLfloat)*numComponet;

	GLfloat* _data = new GLfloat[bufferSize];
	unsigned char*  data = new unsigned char[bufferSize];
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);//����4�ֽڶ���
	glReadBuffer(GL_FRONT);
	glReadPixels(pView[0], pView[1], pView[2], pView[3], GL_BGR_EXT, GL_FLOAT, _data);//����GL_RGB�Ķ�ȡ��ʽ������GL_BGR����GL_BGR_Ext
	glReadBuffer(GL_BACK);
	for (int i = 0; i < bufferSize; i++)
	{
		data[i] = _data[i] * 255;
	}
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	infoHeader.biSize = 40;
	infoHeader.biWidth = imagewidth;
	infoHeader.biHeight = imageheight;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = 24;
	infoHeader.biCompression = BI_RGB;
	infoHeader.biSizeImage = pView[2] * pView[3] * 3;
	infoHeader.biXPelsPerMeter = 0;
	infoHeader.biYPelsPerMeter = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;
	fileHeader.bfType = 0x4D42;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = 54;
	fileHeader.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pView[2] * pView[3] * 3);
	FILE *fd;
	if (fopen_s(&fd, imagefilepath, "wb+") != 0)//filepathΪ���������ļ�������
	{
		cout << "bmpͼƬ�������"<< imagefilepath;
		exit(0);
	}
	else
	{
		fwrite(&fileHeader, 1, sizeof(BITMAPFILEHEADER), fd);
		fwrite(&infoHeader, 1, sizeof(BITMAPINFOHEADER), fd);
		fwrite(data, 1, pView[2] * pView[3] * 3, fd);
		fclose(fd);
	}
	delete[] data;
	delete[] _data;
}
void GLDraw()
{													
	glColor3f(0.0, 1.0, 0.0);     //��
	for (int i = 0; i<m_pic.F.size(); i++)
	{
		glBegin(GL_TRIANGLE_FAN);                            // ����������GL_TRIANGLES;GL_LINE_LOOP;GL_LINES;GL_POINTS
		if (m_pic.VT.size() != 0)glTexCoord2f(m_pic.VT[m_pic.F[i].T[0]].TU, m_pic.VT[m_pic.F[i].T[0]].TV);  //����    
		if (m_pic.VN.size() != 0)glNormal3f(m_pic.VN[m_pic.F[i].N[0]].NX, m_pic.VN[m_pic.F[i].N[0]].NY, m_pic.VN[m_pic.F[i].N[0]].NZ);//������
		glVertex3f(m_pic.V[m_pic.F[i].V[0]].X , m_pic.V[m_pic.F[i].V[0]].Y , m_pic.V[m_pic.F[i].V[0]].Z );        // �϶���

		if (m_pic.VT.size() != 0)glTexCoord2f(m_pic.VT[m_pic.F[i].T[1]].TU, m_pic.VT[m_pic.F[i].T[1]].TV);  //����
		if (m_pic.VN.size() != 0)glNormal3f(m_pic.VN[m_pic.F[i].N[1]].NX, m_pic.VN[m_pic.F[i].N[1]].NY, m_pic.VN[m_pic.F[i].N[1]].NZ);//������
		glVertex3f(m_pic.V[m_pic.F[i].V[1]].X , m_pic.V[m_pic.F[i].V[1]].Y , m_pic.V[m_pic.F[i].V[1]].Z );        // ����

		if (m_pic.VT.size() != 0)glTexCoord2f(m_pic.VT[m_pic.F[i].T[2]].TU, m_pic.VT[m_pic.F[i].T[2]].TV);  //����
		if (m_pic.VN.size() != 0)glNormal3f(m_pic.VN[m_pic.F[i].N[2]].NX, m_pic.VN[m_pic.F[i].N[2]].NY, m_pic.VN[m_pic.F[i].N[2]].NZ);//������
		glVertex3f(m_pic.V[m_pic.F[i].V[2]].X , m_pic.V[m_pic.F[i].V[2]].Y , m_pic.V[m_pic.F[i].V[2]].Z );        // ����
		glEnd();
		// �����λ��ƽ���    
		/*if(m_pic.VN.size()!=0){
		glBegin(GL_LINES);                            // ����������
		glVertex3f(m_pic.V[m_pic.F[i].V[0]].X/YU,m_pic.V[m_pic.F[i].V[0]].Y/YU, m_pic.V[m_pic.F[i].V[0]].Z/YU);        // �϶���
		glVertex3f(m_pic.V[m_pic.F[i].V[0]].X/YU+m_pic.VN[m_pic.F[i].N[0]].NX
		,m_pic.V[m_pic.F[i].V[0]].Y/YU+m_pic.VN[m_pic.F[i].N[0]].NY
		, m_pic.V[m_pic.F[i].V[0]].Z/YU+m_pic.VN[m_pic.F[i].N[0]].NZ);                    // ����
		glEnd();                                // �����λ��ƽ���
		}*/
	}
}
void init() {
	/*
	Vector3f temp = R.row(0);
	R.row(0) = R.row(1);
	R.row(1) = temp;
	m_DF.setR(R);*/
	ReadPIC();
	//������������
	CKirchhoff m_K(m_pic);
	K = m_K.computeK();//��ʼʱ�õ�K����

	m_DF.q = R;
	m_DF.setK(K);
	m_DF.lp=	m_DF.computelp();//�����ʼ��lp

	glClearColor(0.0, 0.0, 0.0, 0.0);  //����ɫ
	//��Ȳ��Ե�������� 
	glClearDepth(1.0);                    //������Ȼ���ĳ�ʼֵ 
	glDepthFunc(GL_LEQUAL);           //��Ȳ��Եķ��� 
	glEnable(GL_DEPTH_TEST);          //������Ȳ���
	GLfloat direction[] = { 3.0, -3.4f, 8.8f, 0.0f }; // ƽ�й�Դ, GL_POSITION���Ե����һ������Ϊ0
	GLfloat ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };  // ����ǿ��
	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // ɢ��ǿ��
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // ����ǿ��
	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glEnable(GL_LIGHTING);   //����:ʹ�ù�
	glEnable(GL_LIGHT0);     //��0#��
	glShadeModel(GL_SMOOTH);//�ж�����ڲ�����Ĺ��ղ���˫���Բ�ֵ�ķ����õ�
	//���ö���β���
	GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat mat_diffuse[] = { 0.1, 0.5, 0.8, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat shininess = 50.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	//ʹ��GL_COLOR_MATERIAL����������glColorMaterial()����ָ��glColor*()������Ӱ�쵽�Ĳ�������
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);

}
//�����ػ�ʱ����Ӧ���� 
void display() {
	//��ǰ�����õı���ɫ�����ֵ�����ɫ�������Ȼ��� 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//��ʼ��ģ����ͼ���� 
	drawScene();            //����ͼԪ�Ļ��� 
	glFlush();              //���ƽ�����Flush ��ǰ��Ⱦ��ˮ�� 
	glutSwapBuffers();       //����ǰ�󻺴棨ֻ����˫�����ģʽ�� 
}

void drawScene()           //����
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(2, 0, 0, 0, 0, 0, 0, 0, 1);//4, 0, -2,
	glPopMatrix();
	if (mouseRightDown ) {
		glTranslatef(0, 0, -cameraDistance*0.1);
	}	
	float *pData = m_DF.GetRotationData();
	cout << pData[0] << " " << pData[4] << " " << pData[8] << " " << pData[12] << endl;
	cout << pData[1] << " " << pData[5] << " " << pData[9] << " " << pData[13] << endl;
	cout << pData[2] << " " << pData[6] << " " << pData[10] << " " << pData[14] << endl;
	cout << pData[3] << " " << pData[7] << " " << pData[11] <<" " << pData[15] << endl;
	glMultMatrixf(pData);
	//glTranslated(m_DF.temp_deltay(0), m_DF.temp_deltay(1), m_DF.temp_deltay(2));
	//glRotated(m_DF.theta, m_DF.delta_q.x(), m_DF.delta_q.y(), m_DF.delta_q.z());
	GLDraw();
}
//���ڴ�С�����仯ʱ����Ӧ���� 
void reshape(int width, int height) {
	glViewport(0, 0, width, height);      //�����Ӵ���С 
	//�����Ӿ����С 
	glMatrixMode(GL_PROJECTION);
	float ratio = (float)width / height;
	glLoadIdentity();
	//gluPerspective(60, ratio, 1, 1000);
	int viewsize=25;
	
	if (width <= height)
		glOrtho(-viewsize, viewsize, -viewsize * (GLfloat)height / (GLfloat)width, viewsize * (GLfloat)height / (GLfloat)width, -10.0, 10.0);
	else
		glOrtho(-viewsize *(GLfloat)width / (GLfloat)height, viewsize*(GLfloat)width / (GLfloat)height	, -viewsize, viewsize, -10.0, 10.0);

	//glOrtho(-25, 25, -25, 25, -10, 10);
}

void TimerFunction(int value)
{
	m_DF.nextTime();
	//����tsfs ����һʱ�̵��������
	/*
	glutPostRedisplay ��ǵ�ǰ������Ҫ���»��ơ�ͨ��glutMainLoop��һ��ѭ��ʱ��
	������ʾ�����ص���������ʾ���ڵ�������塣��ε���glutPostRedisplay��
	����һ����ʾ�ص�ֻ������һ��������ʾ�ص�
	*/
	id++;
	char b[4];
	for (int i = 00;i < 4;i++)
	{
		b[i] = (char)id;
		id = id >> 8;
	}
	glutPostRedisplay(); //��־���»���
	string s = "H:\\MeshData\\images\\0001.bmp";
//	s = s + b[0] + b[1] + b[2] + b[3];
//	s+=".bmp";
	const char *imagefilepath = s.data();
	//ScreenShot(imagefilepath);
	glutTimerFunc(delta_t*1000, TimerFunction, 1);
}
void mouseCB(int button, int state, int x, int y)
{
	mouseX = x;
	mouseY = y;
	if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseRightDown = true;
		}
		else if (state == GLUT_UP)
			mouseRightDown = false;
	}
}

void mouseMotionCB(int x, int y)
{
	if (mouseRightDown)
	{
		cameraDistance = 0;
		cameraDistance += (y - mouseY) * 0.2f;
		mouseY = y;
	}
	glutPostRedisplay();
}
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);    //GLUT ��ĳ�ʼ�� 
	//��ʾģʽ��ʼ������ɫ��ʽ����GLUT_RGBA 
	//           �������˫���塪��GLUT_SINGLE ���� GLUT_float //          �Ƿ�ʹ����Ȼ��桪��GLUT_DEPTH  
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE| GLUT_DEPTH);
	glutInitWindowPosition(100, 100);          //������ʼλ�� 
	glutInitWindowSize(800, 500);             //���ڴ�С 
	glutCreateWindow("UnderWaterRidgebody");       //�������ڲ�ָ�����ڵ����� 
	glutMouseFunc(mouseCB);
	glutMotionFunc(mouseMotionCB);

	init();                                 //OpenGL �ĳ�ʼ������ 

	//������Ϣ��Ӧ�Ļص����� 
	glutDisplayFunc(display);               //���ô����ػ�ʱ����Ӧ���� 
	glutReshapeFunc(reshape);              //���ô��ڴ�С�����仯ʱ����Ӧ���� 

	//��ʱ��  ÿ500���봥��һ��
	glutTimerFunc(delta_t*1000, TimerFunction, 1);

	glutMainLoop();                   //��Ϣѭ������ȡ��Ϣ���ַ���Ϣ����Ӧ��Ϣ 
	return 0;
} //
