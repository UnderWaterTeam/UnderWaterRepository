#include<windows.h>
#include <fstream>
#include<sstream> 
#include<GL/glut.h>
#include<stdio.h>
#include<string>
#include"Kirchhoff.h"
#include "PIC.h"
#include"PICnew.h"
#include"Body.h"
#include"DynamicFormula2.h"
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
int modelNum = 2;//ģ������ sanlengzhui  2���Ļ��ĳ�2
string name[2] = { "H:\\MeshData\\sanlengzhui.obj", "H:\\MeshData\\cube.obj"};//ell0  myproplab
//ģ������
vector<PIC*> v_pic;
vector<PICnew*>  v_picnew;
vector<Body*> v_body;
Vector3d y_0(0, 0, 0);
Vector3d y_1(3, 0, 0);
Vector3d y[2] = { y_0,y_1 };
//���ڵĴ�С
GLdouble windowWidth;
GLdouble windowHeight;
Vector3d omega(0, 0, 0);
Vector3d velocity(0, 0, 0);
Matrix3d R = Matrix3d::Identity();//����Ϊ��λ�� ��init()�Ĳ��ǵ�λ��
double m_fluidDensity = 0.98;
double m_bodyDensity = 1.75;
double delta_t = 0.04;

//DynamicFormula m_DF(omega, velocity, R, y, delta_t);
bool mouseLeftDown;
bool mouseRightDown;
double mouseX, mouseY;
double cameraDistance = 0;
double cameraAngleX;
double cameraAngleY;
void ReadPIC()//������obj�ļ���ȡ��v_pic��,v_picת��v_picnew
{
	for (int i = 0;i < modelNum;i++) {
		ifstream ifs(name[i]);//cube bunny Eight
		string s;
		Mian *f;
		Vertex *v;
		FaXiangLiang *vn;
		WenLi  *vt;
		PIC *m_pic = new PIC();
		while (getline(ifs, s))
		{
			if (s.length() < 2)continue;
			if (s[0] == 'v') {
				if (s[1] == 't') {//vt 0.581151 0.979929 ����
					istringstream in(s);
					vt = new WenLi();
					string head;
					in >> head >> vt->TU >> vt->TV;
					m_pic->VT.push_back(*vt);
				}
				else	if (s[1] == 'n') {//vn 0.637005 -0.0421857 0.769705 ������
					istringstream in(s);
					vn = new FaXiangLiang();
					string head;
					in >> head >> vn->NX >> vn->NY >> vn->NZ;
					m_pic->VN.push_back(*vn);
				}
				else {//v -53.0413 158.84 -135.806 ��
					istringstream in(s);
					v = new Vertex();
					string head;
					in >> head >> v->X >> v->Y >> v->Z;
					m_pic->V.push_back(*v);
				}
			}
			else if (s[0] == 'f') {//f 2443//2656 2442//2656 2444//2656 ��
				for (int k = s.size() - 1; k >= 0; k--) {
					if (s[k] == '/')s[k] = ' ';
				}
				istringstream in(s);
				f = new Mian();
				string head;
				in >> head;
				int i = 0;
				while (i < 3)
				{//
					if (m_pic->V.size() != 0)
					{
						in >> f->V[i];
						f->V[i] -= 1;
					}
					if (m_pic->VT.size() != 0)
					{
						in >> f->T[i];
						f->T[i] -= 1;
					}
					if (m_pic->VN.size() != 0)
					{
						in >> f->N[i];
						f->N[i] -= 1;
					}
					i++;
				}
				m_pic->F.push_back(*f);
			}
		}
		v_pic.push_back(m_pic);

	}
	for (int i = 0;i < v_pic.size();i++) {
		PICnew *a_picnew = new PICnew( v_pic[i]);
		v_picnew.push_back(a_picnew);
	}

}
void GLDraw(int k)//k��ʾ�ǵڼ���ģ��
{
	glColor3f(0.0, 1.0, 0.0);     //��
	PIC *m_pic = v_pic[k];
	for (int i = 0; i < m_pic->F.size(); i++)
	{
		glBegin(GL_TRIANGLE_FAN);                            // ����������GL_TRIANGLES;GL_LINE_LOOP;GL_LINES;GL_POINTS
		if (m_pic->VT.size() != 0)glTexCoord2f(m_pic->VT[m_pic->F[i].T[0]].TU, m_pic->VT[m_pic->F[i].T[0]].TV);  //����    
		if (m_pic->VN.size() != 0)glNormal3f(m_pic->VN[m_pic->F[i].N[0]].NX, m_pic->VN[m_pic->F[i].N[0]].NY, m_pic->VN[m_pic->F[i].N[0]].NZ);//������
		glVertex3f(m_pic->V[m_pic->F[i].V[0]].X, m_pic->V[m_pic->F[i].V[0]].Y, m_pic->V[m_pic->F[i].V[0]].Z);        // �϶���

		if (m_pic->VT.size() != 0)glTexCoord2f(m_pic->VT[m_pic->F[i].T[1]].TU, m_pic->VT[m_pic->F[i].T[1]].TV);  //����
		if (m_pic->VN.size() != 0)glNormal3f(m_pic->VN[m_pic->F[i].N[1]].NX, m_pic->VN[m_pic->F[i].N[1]].NY, m_pic->VN[m_pic->F[i].N[1]].NZ);//������
		glVertex3f(m_pic->V[m_pic->F[i].V[1]].X, m_pic->V[m_pic->F[i].V[1]].Y, m_pic->V[m_pic->F[i].V[1]].Z);        // ����

		if (m_pic->VT.size() != 0)glTexCoord2f(m_pic->VT[m_pic->F[i].T[2]].TU, m_pic->VT[m_pic->F[i].T[2]].TV);  //����
		if (m_pic->VN.size() != 0)glNormal3f(m_pic->VN[m_pic->F[i].N[2]].NX, m_pic->VN[m_pic->F[i].N[2]].NY, m_pic->VN[m_pic->F[i].N[2]].NZ);//������
		glVertex3f(m_pic->V[m_pic->F[i].V[2]].X, m_pic->V[m_pic->F[i].V[2]].Y, m_pic->V[m_pic->F[i].V[2]].Z);        // ����
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
void write() {
	glPushMatrix();
	glPopMatrix();
}
void init() {
	/*
	Vector3d temp = R.row(0);
	R.row(0) = R.row(1);
	R.row(1) = temp;
	m_DF.setR(R);*/
	ReadPIC();
	cout << "y0" << y[0] << endl;
	cout << "y1" << y[1] << endl;
	for (int i = 0;i < modelNum;i++) {
		PICnew *picnew = v_picnew[i];
		Body *m_body = new Body(v_picnew[i], R, y[i]);
		v_body.push_back(m_body);
	}
	DynamicFormula2 m_DF2 ;
	VectorXd tractions= m_DF2.computetraction(v_body);//�ڲ���3*1�Ӿ���
	int j = 0;
	for (int i = 0;i < modelNum;i++) {//��ģ������
		int facenum = v_body[i]->faceNum;
		//Vector3d 
		VectorXd abodytraction(facenum * 3);
		abodytraction=tractions.block(3*i,0, facenum*3,1);//һ��������Ƭ�ܵ�������
		v_body[i]->computetsfs(abodytraction);
	}
	//������������
	//CKirchhoff m_K(m_pic, m_bodyDensity, m_fluidDensity);
	//MatrixXd K = m_K.computeK();//��ʼʱ�õ�K����
	//VectorXd m_tsfs = m_K.computetsfs();
	//cout << "tsfs" << m_tsfs << endl;
	//m_DF.tsfs = m_tsfs;

	//m_DF.q = R;
	//m_DF.setK(K);
	//m_DF.lp = m_DF.computelp();//�����ʼ��lp
	glClearColor(0.0, 0.0, 0.0, 0.0);  //����ɫ
	//��Ȳ��Ե�������� 
	glClearDepth(1.0);                    //������Ȼ���ĳ�ʼֵ 
	glDepthFunc(GL_LEQUAL);           //��Ȳ��Եķ��� 
	glEnable(GL_DEPTH_TEST);          //������Ȳ���
	GLfloat direction[] = { 0, -1, 0, 0 }; // ƽ�й�Դ, GL_POSITION���Ե����һ������Ϊ0
	GLfloat ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };  // ����ǿ��
	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // ɢ��ǿ��
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // ����ǿ��
	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	GLfloat direction1[] = { 0, 0, 1, 0 };
	glLightfv(GL_LIGHT1, GL_POSITION, direction1);

	glEnable(GL_LIGHTING);   //����:ʹ�ù�
	glEnable(GL_LIGHT0);     //��0#��
	glEnable(GL_LIGHT1);     //��0#��
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

void drawScene()           //����
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);//4, 0, -2,��ʵ�ʳ����У�Ӧ��������ģ�ͱ任֮�󣬵���OpenGL�У�����˳��ǡ���෴��Ӧ��������ģ�ͱ任֮ǰ
	glTranslatef(0, -cameraDistance, 0);
	//���������
	for (int i = 0;i < modelNum;i++) {
		glPushMatrix();
		float *pData = v_body[i]->GetRotAndTransData();
		//cout << pData[0] << " " << pData[4] << " " << pData[8] << " " << pData[12] << endl;
		//cout << pData[1] << " " << pData[5] << " " << pData[9] << " " << pData[13] << endl;
		//cout << pData[2] << " " << pData[6] << " " << pData[10] << " " << pData[14] << endl;
		//cout << pData[3] << " " << pData[7] << " " << pData[11] <<" " << pData[15] << endl;
		Vector3d tempy = v_body[i]->g.block(0, 3, 3, 1);
		cout << "��" << i << "��cube��λ����" << tempy << endl;
		glTranslatef(tempy(0), tempy(1), tempy(2));//��ʵ���෴
		glMultMatrixf(pData);
		//glRotated(m_DF.theta, m_DF.delta_q.x(), m_DF.delta_q.y(), m_DF.delta_q.z());
		GLDraw(i);
		glPopMatrix();
	}
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
//���ڴ�С�����仯ʱ����Ӧ���� 
void reshape(int width, int height) {
	glViewport(0, 0, width, height);      //�����Ӵ���С 
	//�����Ӿ����С 
	glMatrixMode(GL_PROJECTION);
	double ratio = (double)width / height;
	glLoadIdentity();
	//gluPerspective(60, ratio, 1, 1000);
	int viewsize = 25;

	if (width <= height)
		glOrtho(-viewsize, viewsize, -viewsize * (GLdouble)height / (GLdouble)width, viewsize * (GLdouble)height / (GLdouble)width, 0, 20.0);
	else
		glOrtho(-viewsize * (GLdouble)width / (GLdouble)height, viewsize*(GLdouble)width / (GLdouble)height, -viewsize, viewsize, 0, 20.0);

	//glOrtho(-25, 25, -25, 25, -10, 10);
}

void TimerFunction(int value)
{
	DynamicFormula2 m_DF2;
	VectorXd tractions = m_DF2.computetraction(v_body);//�ڲ���3*1�Ӿ���
	int j = 0;
	for (int i = 0;i < modelNum;i++) {//��ģ������
		int facenum = v_body[i]->faceNum;
		//Vector3d 
		//Vector3d sumf(0, 0, 0);//traction���
		VectorXd abodytraction = tractions.block(3 * i, 0, facenum*3, 1);//һ��������Ƭ�ܵ�������
		v_body[i]->computetsfs(abodytraction);
		v_body[i]->nextTime();
	}
	/*
	glutPostRedisplay ��ǵ�ǰ������Ҫ���»��ơ�ͨ��glutMainLoop��һ��ѭ��ʱ��
	������ʾ�����ص���������ʾ���ڵ�������塣��ε���glutPostRedisplay��
	����һ����ʾ�ص�ֻ������һ��������ʾ�ص�
	*/
	glutPostRedisplay(); //��־���»���
	glutTimerFunc(delta_t * 1000, TimerFunction, 1);
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
		//cameraDistance = 0;
		cameraDistance += (y - mouseY) * 0.02f;
		mouseY = y;
	}
	glutPostRedisplay();
}
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);    //GLUT ��ĳ�ʼ�� 
	//��ʾģʽ��ʼ������ɫ��ʽ����GLUT_RGBA 
	//           �������˫���塪��GLUT_SINGLE ���� GLUT_double //          �Ƿ�ʹ����Ȼ��桪��GLUT_DEPTH  
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);          //������ʼλ�� 
	glutInitWindowSize(800, 500);             //���ڴ�С 
	glutCreateWindow("UnderWaterRidgebody");       //�������ڲ�ָ�����ڵ����� 
	glutMouseFunc(mouseCB);
	glutMotionFunc(mouseMotionCB);

	init();                                 //OpenGL �ĳ�ʼ������ 

	//������Ϣ��Ӧ�Ļص����� 
	glutDisplayFunc(display);               //���ô����ػ�ʱ����Ӧ���� 
	glutReshapeFunc(reshape);              //���ô��ڴ�С�����仯ʱ����Ӧ���� 

	//��ʱ��  ÿdelta_t*1000���봥��һ��
	glutTimerFunc(delta_t * 1000, TimerFunction, 1);

	glutMainLoop();                   //��Ϣѭ������ȡ��Ϣ���ַ���Ϣ����Ӧ��Ϣ 
	return 0;
} 
