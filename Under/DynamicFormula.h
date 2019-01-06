#pragma once
#include <Eigen/Dense>
#include <Eigen/Cholesky>  
#include <Eigen/LU>  
#include <Eigen/QR>  
#include <Eigen/SVD>  
//#include"Quaternion.h"
#include <iostream>
using namespace std;
using namespace Eigen;
class DynamicFormula
{
public:
	DynamicFormula(Vector3f omega, Vector3f velocity, Matrix3f R,
		Vector3f y, float delta_t);
	~DynamicFormula();
	void setK(MatrixXf newK) {
		K = newK;
	}
	void setR(Matrix3f newR) {
		R = newR;
	}
	VectorXf tsfs2tf(Matrix3f Y);
	//Matrix3f computeR_();
	Vector3f computey_();
	VectorXf computelp();
	VectorXf computelp_();
	Matrix3f computeNextR();
	Vector3f computeNexty(Vector3f y_);
	VectorXf computeNextlp();
	VectorXf computeNextwv();
	Vector3f vec62Vec31(VectorXf ab);
	Vector3f vec62Vec32(VectorXf ab);
	void nextTime();
	void set_tsfs(Vector3f ts,Vector3f fs);
	float* GetRotAndTransData();
	Matrix3f s_GetRotaionMatrix(float angle, const Vector3f &axis);
	Matrix3f so3_ad(Vector3f omega);
	Matrix4f se3_cay(VectorXf tempepsilon);
	Matrix3f so3_cay(Vector3f tempw);
	MatrixXf se3_ad(VectorXf tempepsilon);
	VectorXf Unconstr_Dyn(VectorXf epsilon_now, VectorXf epsilon_last, Matrix4f gk);
	VectorXf se3_DEP(VectorXf epsilon_now, VectorXf epsilon_last, Matrix4f gk);
	MatrixXf se3_Ctln(VectorXf tempepsilon);
//private:
	//��ʼʱ�̵� 6+1����
	Vector3f w;
	Vector3f v;
	Matrix3f R;//R��ʼ��һ����������
	Quaternionf q;//Rת����Ԫ��
	Vector3f y;
	VectorXf tsfs;
	VectorXf lp;
	VectorXf lp_;
	Matrix4f g;
	VectorXf epsilon;

	MatrixXf K;
	float delta_t;
	//ƽ����ת��Ҫ����������
	Quaternionf delta_q;
	float theta;

};