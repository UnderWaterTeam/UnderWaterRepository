#include "DynamicFormula2.h"
#include <cmath>
double DynamicFormula2::mu=1;

//����ÿ�����ϵ�Ӧ��
VectorXd DynamicFormula2::computetraction(vector<Body*> m_body){
	int n = m_body[0]->idnum;//��ȡ����Ƭ��
	MatrixXd coefficient(3*n, 3*n);
	coefficient.setZero();
	VectorXd traction(3 * n);
	MatrixXd H(3*n, 3*n);
	H.setZero();
	VectorXd u(3*n);		
	for (int p = 0;p < m_body.size();p++) {
		for (int q = 0;q < m_body[p]->v_onepoint.size();q++) {
			Matrix3d R = m_body[p]->g.block(0, 0, 3, 3);
			Vector3d y_ = m_body[p]->g.block(0, 3, 3, 1);//������ ���������λ��
			Vector3d y = R*m_body[p]->v_onepoint[q].midpoint+y_;// �õ�y
			int b= m_body[p]->v_onepoint[q].id;//�±����
			Vector3d ny = R*m_body[p]->v_onepoint[q].normal;//�õ�y�ķ���
			//��������Ľ��ٶȺ��ٶ�
			Vector3d w = R*m_body[p]->epsilon.block(0, 0, 3, 1);
			Vector3d v = R*m_body[p]->epsilon.block(3, 0, 3, 1);
			//cout << "�ٶ��ǣ���" << v << endl;
			//Ӧ��������������ٶ�
			u.block(3 * b,0,3,1) = w.cross(y) + v;
			for (int i = 0;i < m_body.size();i++) {
				for (int j = 0;j < m_body[i]->v_onepoint.size();j++) {
					Matrix3d R2 = m_body[i]->g.block(0, 0, 3, 3);
					Vector3d y_2 = m_body[i]->g.block(0, 3, 3, 1);//������ ���������λ��
					Vector3d x = R2*m_body[i]->v_onepoint[j].midpoint+y_2;//�õ�sourcepoint x
					int a = m_body[i]->v_onepoint[j].id;//�±����
					Vector3d nx = R*m_body[i]->v_onepoint[j].normal;//�õ�x�ķ���
					if (x != y) {
						Matrix3d KS=	computeKij(x, nx, y, ny)*m_body[p]->v_onepoint[q].area;
						//cout << "K" << endl << computeKij(x, nx, y, ny) << endl;
						coefficient.block(3*a, 3*b, 3, 3) = KS;
						Matrix3d HS = computeHij(x, nx, y, ny)*m_body[p]->v_onepoint[q].area;
						//cout << "H"<<endl << computeHij(x, nx, y, ny) << endl;
						H.block(3*a, 3*b, 3, 3) = HS;												
					}	
					//else if(x==y){
					//	y += ny * 0.01;
					//	Matrix3d KS = computeKij(x, nx, y, ny)*m_body[p]->v_onepoint[q].area;
					//	//cout << "K" << endl << computeKij(x, nx, y, ny) << endl;
					//	coefficient.block(3 * a, 3 * b, 3, 3) = KS;
					//	Matrix3d HS = computeHij(x, nx, y, ny)*m_body[p]->v_onepoint[q].area;
					//	//cout << "H" << endl << computeHij(x, nx, y, ny) << endl;
					//	H.block(3 * a, 3 * b, 3, 3) = HS;
					//}				
				}
			}
		}
	}
	for (int i = 0;i < n;i++) {
		Matrix3d HSum;
		HSum.setZero();
		Matrix3d coefficientSum;
		coefficientSum.setZero();
		for (int j = 0;j < n&&i!=j;j++) {
			HSum += H.block(3 * i, 3 * j, 3, 3);
			coefficientSum += coefficient.block(3 * i, 3 * j, 3, 3);
		}
		Matrix3d identity;
		identity.setIdentity();
		H.block(3 * i, 3 * i, 3, 3) = identity-HSum;//�Խ����ϵ�����Ԫ��������������
		coefficient.block(3 * i, 3 * i, 3, 3) = identity-coefficientSum;
	}
	//cout <<"coefficient"<<endl<< coefficient << endl;
	//�����Է�����
	VectorXd b = H * u;
	//cout << "H:" << endl << H << endl;
	//cout << "u:" << endl << u << endl;
	//cout << "b:" << endl << b << endl;
	traction = coefficient.fullPivHouseholderQr().solve(b);//sigma=strength NAN!
	//cout << "traction:" << endl << traction << endl;
	return traction;
}

Matrix3d DynamicFormula2::computeKij(Vector3d x, Vector3d nx, Vector3d y, Vector3d ny) {
	Matrix3d res;
	res.setZero();
	double r2 = pow(x(0) - y(0), 2) +
		pow(x(1) - y(1), 2) + pow(x(2) - y(2), 2);
	double r =sqrt(r2);//r��ƽ��
	for (int i = 0;i < 3;i++) {
		for (int j = 0;j < 3;j++) {
			double rknk = 0;
			for (int k = 0;k < 3;k++) {
				//cout << "yk-xk" << y(k) - x(k) << endl;
				//cout << "nx(k)" << nx(k) << endl;
				rknk += (y(k)-x(k) ) / r * nx(k);
			}
			res(i, j) = 3 / (4 * 3.1415926*r2)*(y(i) - x(i) )*( y(j) - x(j))/ r2 *rknk;
			//r*r�ϲ����Ӿ���
			//cout << "i:" << i << "j:" << j <<":"<< res(i, j)<<endl;
		}
	}
	return res;
}
double DynamicFormula2::dirac(int a, int b) {
	if (a == b) {
		return 1;
	}
	else {
		return 0;
	}
}

Matrix3d DynamicFormula2::computeHij(Vector3d x, Vector3d nx, Vector3d y, Vector3d ny) {
	Matrix3d res;
	res.setZero();
	double r2 = pow(x(0) - y(0), 2) +
		pow(x(1) - y(1), 2) + pow(x(2) - y(2), 2);
	double r = sqrt(r2);//r��ƽ��
	Vector3d n = (x-y) / r;//y��x�ĵ�λ����
	double rlnl=0;
	for (int l = 0;l < 3;l++) {
		rlnl += (y(l) - x(l)) * ny(l);
	}
	rlnl /= r;//��߾���
	for (int i = 0;i < 3;i++) {
		for (int j = 0;j < 3 ;j++) {
			for (int k = 0;k < 3;k++) {
				res(i, j) += mu / (4 * 3.1415926)*
				(
					(3 * (dirac(i, j)* (y(k) - x(k)) / (r2 * r2) + dirac(j, k) * (y(i) - x(i)) / (r2 * r2) )
						- 30 * (y(i) - x(i))  * (y(j) - x(j)) * (y(k) - x(k)) / (r2*r2*r2)
					)  *rlnl
						+ 3 * ( n(i)*(y(j) - x(j))  * (y(k) - x(k)) / (r2*r2*r) 
										+ n(k) *(y(i) - x(i)) * (y(j) - x(j) ) / ( r2*r2*r) 
								 )
						+ 2 * dirac(i, k)*n(j)/(r2*r)
				)*nx(k);
				//cout << "H i =" << i << "j=" << j << H(i, j) << endl;
			}
		}
	}
	return res;
}


