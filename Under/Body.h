#pragma once
#include <Eigen/Dense>
#include <Eigen/Cholesky>  
#include <Eigen/LU>  
#include <Eigen/QR>  
#include <Eigen/SVD>  
#include <vector>
#include "PICnew.h"
#include "PIC.h"
using namespace std;
using namespace Eigen;
struct onepointST//������������ϵ
{
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	int id;
	Vector3d midpoint;//�е�
	Vector3d vertex[3];//������������
	Vector3d normal;//����
	double area;
};
class Body {
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	static int idnum;//��������Ƭ��ţ�������ֵid
	Matrix4d g;
	VectorXd epsilon;//��������ϵ��
	VectorXd tsfs;
	//Vector3d f;
	MatrixXd K;
	int faceNum;
	double delta_t ;
	double bodyMass;//computeJ�õ���mass
	double bodyDensity=0.5;//��û�õ� no ���������õ���  1.75
	double fluidDensity= 0.98;
	double volume;
	Vector3d masscenter;//��������ϵ�����ģ���Ϊ����һֱ���˶�
	//VectorXd traction;
	PICnew *m_picnew;
	vector <onepointST> v_onepoint;//������������
	Body(PICnew *m_picnew,Matrix3d R,Vector3d y, double delta_t, Vector3d ve, Vector3d ome);
	void Subexpressions(double &w0, double &w1, double &w2, double &f1, double &f2, double &f3, double &g0, double &g1, double &g2);
	Matrix3d computeJ();
	MatrixXd computeKB();

	float* GetRotAndTransData();
	void computetsfs(VectorXd traction);//ͨ���������ϵ�������������ͺ�������
	Matrix3d so3_ad(Vector3d omega);
	Matrix4d se3_cay(VectorXd tempepsilon);
	Matrix3d so3_cay(Vector3d tempw);
	MatrixXd se3_ad(VectorXd tempepsilon);
	VectorXd Unconstr_Dyn(VectorXd epsilon_now, VectorXd epsilon_last, Matrix4d &gk);
	VectorXd se3_DEP(VectorXd epsilon_now, VectorXd epsilon_last, Matrix4d &gk);
	MatrixXd se3_Ctln(VectorXd tempepsilon);
	VectorXd tsfs2tf(Matrix3d Y);
	void nextTime();
	void convertFromBodyToSpace();
	
};