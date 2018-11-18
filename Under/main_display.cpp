#include<windows.h>
#include<iostream>
#include<sstream> 
#include<GL/glut.h>
#include<stdio.h>
#include"main_display.h"
#include"Quaternion.h"
#include"Kirchhoff.h"
#include "PIC.h"
#include <fstream>
#include<iostream>
using namespace std;
/*
��ʱ����glutTimerFunc(unsigned int millis, void (*func)(int value), int value);
�ػ��־��glutPosRedisplay();
��ʱ����һ��������ÿ��millis��������func���������Ҵ�һ��value������ȥ
��Ϊһ����ʱ��ֻ������һ�Σ�������Ҫ��ε��ö�ʱ��
*/

//obj��ȡ
string name = "H:\\MeshData\\cube.obj";
PIC m_pic;

CMatrix toDaOmega(CVector3D omega);
void drawScene();
//���ڵĴ�С
GLfloat windowWidth;
GLfloat windowHeight;

//��Ҫ��֪��7+1����
double omega[3] = { 0, 0, 1 };
double velocity[3] ={0, -1,0 };
double y[3] = { 0, 0, 0 };
double t[3] = { 0, 0, 0 };
double f[3] = { 0, -9.8f, 0 };
double one[3] = { 1, 1, 1 };//�� �����һ��
CVector3D v_omega(omega);
CVector3D v_velocity(velocity);
CMatrix m_DaOmega=toDaOmega(v_omega);//��
CMatrix m_R;
CPoint3D p_y(0,0,0);//y
CVector3D v_t(0,0,0);//t
CVector3D v_f(0,-0.98f,0);//f

CVector3D v_rand(one);//��λ�� �����һ��
CVector6D K(0,0,0,0,0,0);
/*
//������������
CKirchhoff m_K(m_pic);
CVector6D K = m_K.computeK();
*/

//�м���
CMatrix m_R_ ;
CPoint3D p_y_(0,0,0);
CVector3D v_l(0, 0, 0);
CVector3D v_p(0, 0, 0);
CVector3D v_l_(0, 0, 0);
CVector3D v_p_(0, 0, 0);

//ƫ���� ��ת��
CVector3D temp_deltay(0,0,0);
CQuaternion q(0, 0, 0, 0);

double delta_t = 0.05;
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
			else if (s[1] == 'n'){//vn 0.637005 -0.0421857 0.769705 ������
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
void GLDraw()
{
	GLfloat ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };  // ����ǿ��
	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // ɢ��ǿ��
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // ����ǿ��
													 // ƽ�й�Դ, GL_POSITION���Ե����һ������Ϊ0
	GLfloat direction[] = { -3.0f, -3.4f, -8.8f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

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

		glColor3f(0.0, 1.0, 0.0);     
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
CMatrix toDaOmega(CVector3D omega){//w->��
	CMatrix res;
	res[0][0] = 0;
	res[0][1] = -omega[2];
	res[0][2] = omega[1];
	res[1][0] = omega[2];
	res[1][1] = 0;
	res[1][2] = -omega[0];
	res[2][0] = -omega[1];
	res[2][1] = omega[0];
	res[2][2] = 0;
	return res;
}
void init() {
	ReadPIC();
	//������������
	CKirchhoff m_K(m_pic);
	K = m_K.computeK();
	cout << "K�㣺" << K.getData1().getm_data(0);
	cout << "Kһ��" << K.getData1().getm_data(1);
	cout << "K����" << K.getData1().getm_data(2);

	cout << "K����" << K.getData2().getm_data(0);
	cout << "K�ģ�" << K.getData2().getm_data(1);
	cout << "K�壺" << K.getData2().getm_data(2);
	//glClearColor(0.0, 0.0, 0.0, 1.0);           //���ñ�����ɫ  
	glClearColor(0.75f, 0.75f, 0.75f, 0.0f);
	//��Ȳ��Ե�������� 
	glClearDepth(1.0);                    //������Ȼ���ĳ�ʼֵ 
	glDepthFunc(GL_LEQUAL);           //��Ȳ��Եķ��� 
	glEnable(GL_DEPTH_TEST);          //������Ȳ���
	//����
//���ʷ���������
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };  //���淴�����
	GLfloat mat_shininess[] = { 50.0 };               //�߹�ָ��
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };   //��λ��(1,1,1), ���1-����
	GLfloat Light_Model_Ambient[] = { 0.2, 0.2, 0.2, 1.0 }; //���������

	glClearColor(0.0, 0.0, 0.0, 0.0);  //����ɫ
	glShadeModel(GL_SMOOTH);           //��������ģʽ

	//��������
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	//�ƹ�����
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);   //ɢ�������
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);  //���淴���
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_Model_Ambient);  //���������

	glEnable(GL_LIGHTING);   //����:ʹ�ù�
	glEnable(GL_LIGHT0);     //��0#��
	glEnable(GL_DEPTH_TEST); //����Ȳ���

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
void DrawCylinder(double r, double h, int nSlice)
{
	//glTranslated(0, 0, -h / 2);

#define  PI   3.1415926
	double delta = PI * 2 / nSlice;
	double angle = delta;

	glColor3f(1.0f, 0.0f, 0.0f);

	//top
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 0, 1);
	glVertex3d(0, 0, h);
	glVertex3d(r, 0, h);
	for (int i = 1; i < nSlice; ++i)
	{
		glVertex3d(r * cos(angle), r * sin(angle), h);
		angle += delta;
	}
	glVertex3d(r, 0, h);
	glEnd();

	//bottom
	angle = delta;
	glColor3f(0.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 0, -1);
	glVertex3d(0, 0, 0);
	glVertex3d(r, 0, 0);
	for (int i = 1; i < nSlice; ++i)
	{
		glVertex3d(r * cos(angle), r * sin(angle), 0);
		angle += delta;
	}
	glVertex3d(r, 0, 0);
	glEnd();

	//cylinder
	angle = delta;
	glColor3f(0.0f, 0.0f, 1.0f);

	glBegin(GL_QUAD_STRIP);
	glNormal3f(1, 0, 0);
	glVertex3d(r, 0, h);
	glVertex3d(r, 0, 0);
	for (int i = 1; i < nSlice; ++i)
	{
		double c = cos(angle);
		double s = sin(angle);
		glNormal3d(c, s, 0);

		double x = r * c;
		double y = r * s;
		glVertex3d(x, y, h);
		glVertex3d(x, y, 0);

		angle += delta;
	}
	glNormal3f(1, 0, 0);
	glVertex3d(r, 0, h);
	glVertex3d(r, 0, 0);
	glEnd();
}
void drawScene()           //����
{
	//glRotated()
	//glTranslated(m_translate[0], m_translate[1], m_translate[2]);
	glTranslated(temp_deltay[0], temp_deltay[1], temp_deltay[2]);
	glRotated(q.get_w(), q.get_x(), q.get_y(), q.get_z());
	glColor3f(0.0, 1.0, 0.0);     //��
	GLDraw();
	//DrawCylinder(1,2, 32);
	//glutSolidCube(3);

} //���ڴ�С�����仯ʱ����Ӧ���� 
void reshape(int width, int height) {
	glViewport(0, 0, width, height);      //�����Ӵ���С 
	//�����Ӿ����С 
	glMatrixMode(GL_PROJECTION);
	double ratio = (double)width / height;
	glLoadIdentity();
	gluPerspective(60, ratio, 1, 1000);
	//glOrtho(-5, 5, -5, 5, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(16, 0, -2, 0, 0, -2, 0, 0, 1);
}

//����ת��Ϊ��Ԫ������Ԫ������ʱ��󣬱�ؾ���
CMatrix toMat(CMatrix R_){
	CMatrix res;
	CQuaternion q;
	q.FromRotationMatrix(R_);
	q.setomega(q.get_w()*delta_t);
	res=q.ToMatrix();
	return res;
}

void TimerFunction(int value)
{
	//�м���
	m_R_ = m_R%m_DaOmega;
	p_y_ = m_R *v_velocity;
	v_l = K.getData1() *v_omega;
	v_p = K.getData2() *v_velocity;
	v_l_ = v_l*v_omega += v_p*v_velocity += v_t;
	v_p_ = v_p*v_omega += v_f;

	CMatrix RotateMat = toMat(m_R_);//û����

	//�����һʱ�̵�7����
	CMatrix temp_deltaR = toMat(m_R_);
	m_R = m_R%temp_deltaR;
	temp_deltay.setData(p_y_[0] * delta_t, p_y_[1] * delta_t, p_y_[2] * delta_t);
	p_y += temp_deltay;
	cout << "λ����";
	CVector3D temp_deltaL(v_l_[0] * delta_t, v_l_[1] * delta_t, v_l_[2] * delta_t);
	v_l += temp_deltaL;
	CVector3D temp_deltap(v_p_[0] * delta_t, v_p_[1] * delta_t, v_p_[2] * delta_t);
	v_p += temp_deltap;
	v_omega = v_l /= K.getData1();
	v_velocity = v_p /= K.getData2();

	q.FromRotationMatrix(m_R_);
	q.setomega(q.get_w()*delta_t);
	/*
	//��������仯λ�÷���
	glTranslated(temp_deltay[0], temp_deltay[1], temp_deltay[2]);
	CQuaternion q;
	q.FromRotationMatrix(m_R_);
	q.setomega(q.get_w()*delta_t);

	glRotated(q.get_w(), q.get_x(), q.get_y(), q.get_z());*/
	/*
	glutPostRedisplay ��ǵ�ǰ������Ҫ���»��ơ�ͨ��glutMainLoop��һ��ѭ��ʱ��
	������ʾ�����ص���������ʾ���ڵ�������塣��ε���glutPostRedisplay��
	����һ����ʾ�ص�ֻ������һ��������ʾ�ص�
	*/
	glutPostRedisplay(); //��־���»���
	glutTimerFunc(1000, TimerFunction, 1);
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);    //GLUT ��ĳ�ʼ�� 
	//��ʾģʽ��ʼ������ɫ��ʽ����GLUT_RGBA 
	//           �������˫���塪��GLUT_SINGLE ���� GLUT_DOUBLE //          �Ƿ�ʹ����Ȼ��桪��GLUT_DEPTH  
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);          //������ʼλ�� 
	glutInitWindowSize(500, 500);             //���ڴ�С 
	glutCreateWindow("UnderWaterRidgebody");       //�������ڲ�ָ�����ڵ����� 

	init();                                 //OpenGL �ĳ�ʼ������ 

	//������Ϣ��Ӧ�Ļص����� 
	glutDisplayFunc(display);               //���ô����ػ�ʱ����Ӧ���� 
	glutReshapeFunc(reshape);              //���ô��ڴ�С�����仯ʱ����Ӧ���� 

	//��ʱ��  ÿ1000���봥��һ��
	glutTimerFunc(1000, TimerFunction, 1);

	glutMainLoop();                   //��Ϣѭ������ȡ��Ϣ���ַ���Ϣ����Ӧ��Ϣ 
	return 0;
} //
