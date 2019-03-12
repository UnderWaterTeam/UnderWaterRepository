#pragma once
#include <Eigen/Dense>
#include <Eigen/Cholesky>  
#include <Eigen/LU>  
#include <Eigen/QR>  
#include <Eigen/SVD>  
//#include"Quaternion.h"
#include"Body.h"
#include <iostream>
using namespace std;
using namespace Eigen;
class DynamicFormula2
{
public:
	static VectorXd computetraction(vector<Body*> m_body);//����ÿ�����ϵ�Ӧ��
	static Matrix3d computeKij(Vector3d x,Vector3d nx, Vector3d y,Vector3d ny);
	static Matrix3d computeHij(Vector3d x, Vector3d nx, Vector3d y, Vector3d ny);
	static double dirac(int a, int b);
/*
	MatrixXd K;
	double delta_t;*/
	static double mu;
	//ƽ����ת��Ҫ����������
	//Quaternionf delta_q;
	//double theta;

};