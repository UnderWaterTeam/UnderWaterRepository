#pragma once
#include <Eigen/Dense>
#include <Eigen/Cholesky>  
#include <Eigen/LU>  
#include <Eigen/QR>  
#include <Eigen/SVD>  
#include <vector>
#include "PIC.h"
using namespace std;
using namespace Eigen;
struct VertexandNormalST//��������Ͷ��㷨��
{
	Vector3d coordinate;
	Vector3d vertexNormal;
};
struct FaceandNormalST//���������淨��
{
	int vertexIndex[3];
	Vector3d faceNormal;
};
class PICnew {
public:
	vector <VertexandNormalST> vertexandnormal;
	vector <FaceandNormalST> faceandnormal;
	PICnew(PIC *m_PIC);
	//PICnew();
};