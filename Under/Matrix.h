#pragma once
#include "Point3D.h"

class CMatrix
{
public:
	CMatrix(void);
	~CMatrix(void);

	double* operator[](int idx)
	{
		return m_data[idx];
	}

	const double* operator[](int idx) const
	{
		return m_data[idx];
	}

	CMatrix operator % (const CMatrix &matrix) const;//����˷�
	CVector3d operator * (const CVector3d &vector) const;//�����������
	static CMatrix s_GetRotaionMatrix(double angle, const CPoint3D &axis);

private:
	double m_data[3][3];
};

