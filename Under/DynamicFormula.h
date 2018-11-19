#pragma once
#include <Eigen/Dense>
#include <Eigen/Cholesky>  
#include <Eigen/LU>  
#include <Eigen/QR>  
#include <Eigen/SVD>  
#include"Quaternion.h"
using namespace Eigen;
class DynamicFormula
{
public:
	DynamicFormula(Vector3d omega, Vector3d velocity, Matrix3d R,
		Vector3d y, Vector3d ts, Vector3d fs,MatrixXd K,double delta_t);
	~DynamicFormula();
	void setK(MatrixXd newK) {
		K = newK;
	}
	Matrix3d toDaOmegaOrY(Vector3d omega);
	VectorXd tsfs2tf(Matrix3d R, Matrix3d Y, Vector3d ts, Vector3d fs);
	Matrix3d computeR_();
	Matrix3d computey_();
	VectorXd computelp();

	VectorXd computelp_(VectorXd lp);
	void computeNextR(Matrix3d R_);
	void computeNexty(Vector3d y_);
	VectorXd computeNextlp(VectorXd lp, VectorXd lp_);
	void computeNextwv(VectorXd lp);
	Vector3d vec62Vec31(VectorXd ab);
	Vector3d vec62Vec32(VectorXd ab);
	void nextTime();
	void set_tsfs(Vector3d ts,Vector3d fs);

//private:
	//��ʼʱ�̵� 6+1����
	Vector3d w;
	Vector3d v;
	Matrix3d R; //R��ʼ��һ����������
	Vector3d y;
	Vector3d ts;
	Vector3d fs;

	MatrixXd K;
	double delta_t;
	//ƽ����ת��Ҫ����������
	Quaterniond q;
	Vector3d temp_deltay;

};

DynamicFormula::DynamicFormula(Vector3d omega, Vector3d velocity,Matrix3d R,
	Vector3d y, Vector3d ts, Vector3d fs,MatrixXd K,double delta_t)
{
	this->w= omega;
	this->v= velocity;
	this->R	=R; //R��γ�ʼ��
	this->y=y;
	this->ts=ts;
	this->fs = fs;
	this->K = K;
	this->delta_t = delta_t;
}

DynamicFormula::~DynamicFormula()
{
}