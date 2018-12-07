#pragma once
#include "Point3D.h"

class CMatrix
{
public:
	CMatrix(void);
	~CMatrix(void);

	float* operator[](int idx)
	{
		return m_data[idx];
	}

	const float* operator[](int idx) const
	{
		return m_data[idx];
	}

	CMatrix operator % (const CMatrix &matrix) const;//����˷�
	CVector3f operator * (const CVector3f &vector) const;//�����������
	static CMatrix s_GetRotaionMatrix(float angle, const CPoint3D &axis);

private:
	float m_data[3][3];
};

