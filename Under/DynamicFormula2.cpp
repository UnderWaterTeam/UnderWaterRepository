#include "DynamicFormula2.h"
#include <cmath>
double DynamicFormula2::mu=0.001;//


//����ÿ�����ϵ�Ӧ��
VectorXd DynamicFormula2::computetraction(vector<Body*> m_body){
	int n = m_body[0]->idnum;//��ȡ����Ƭ��,idnum�Ǿ�̬����
	MatrixXd coefficient(3*n, 3*n);
	coefficient.setZero();
	VectorXd traction(3 * n);
	MatrixXd H(3*n, 3*n);
	H.setZero();
	VectorXd u(3*n);		
	Matrix3d identity;
	identity.setIdentity();
	for (int p = 0;p < m_body.size();p++) {
		for (int q = 0;q < m_body[p]->v_onepoint.size();q++) {//field point
			Matrix3d R = m_body[p]->g.block(0, 0, 3, 3);
			Vector3d y_ = m_body[p]->g.block(0, 3, 3, 1);//���������������λ��
			Vector3d y = R*m_body[p]->v_onepoint[q].midpoint+y_;//��������ϵ��field���λ��
			//cout << "����y" << y << endl; ����Ҫy������е���
			// �õ�y�� q���е�����������ϵ��λ��
			int b= m_body[p]->v_onepoint[q].id;//�±����
			Vector3d ny = R*m_body[p]->v_onepoint[q].normal;//�õ�y�ķ���
			//��������Ľ��ٶȺ��ٶ�
			Vector3d w = R*m_body[p]->epsilon.block(0, 0, 3, 1);
			Vector3d v = R*m_body[p]->epsilon.block(3, 0, 3, 1);
			//cout << "�ٶ��ǣ���" << v << endl;
			//Ӧ��������������ٶ�
			Vector3d tempw = m_body[p]->epsilon.block(0, 0, 3, 1);
			Vector3d tempv = m_body[p]->epsilon.block(3, 0, 3, 1);
			Vector3d tempy = m_body[p]->v_onepoint[q].midpoint;
			Matrix3d Y(3, 3);
			Y(0, 0) = 0;
			Y(0, 1) = -tempy(2);
			Y(0, 2) = tempy(1);
			Y(1, 0) = tempy(2);
			Y(1, 1) = 0;
			Y(1, 2) = -tempy(0);
			Y(2, 0) = -tempy(1);
			Y(2, 1) = tempy(0);
			Y(2, 2) = 0;
			u.block(3 * b,0,3,1) = 
				R*(tempw.cross(tempy) 
				+ tempv);
			for (int i = 0;i < m_body.size();i++) {//source point
				for (int j = 0;j < m_body[i]->v_onepoint.size();j++) {
					Matrix3d R2 = m_body[i]->g.block(0, 0, 3, 3);
					Vector3d y_2 = m_body[i]->g.block(0, 3, 3, 1);//������ ���������λ��
					Vector3d x = R2*m_body[i]->v_onepoint[j].midpoint+y_2;
					//cout << "����x" << x << endl;
					//�õ�sourcepoint x
					int a = m_body[i]->v_onepoint[j].id;//�±����
					Vector3d nx = R*m_body[i]->v_onepoint[j].normal;//�õ�x�ķ���
					if (p==i&&q==j) {//�Խ���
						//cout << "����Խ���" << endl;
						//��������
						Vector3d p0 = m_body[p]->v_onepoint[q].vertex[0];
						Vector3d p1 = m_body[p]->v_onepoint[q].vertex[1];
						Vector3d p2 = m_body[p]->v_onepoint[q].vertex[2];
						//����������е�
						Vector3d p01 = (p0 + p1) / 2;
						Vector3d p02 = (p0 + p2) / 2;
						Vector3d p12 = (p1 + p2) / 2;
						//S0
						Vector3d mid0 = (p0 + p01 + p02) / 3;
						double S0 = trianglearea(p0, p01, p02);
						//S1
						Vector3d mid1 = (p1 + p01 + p12) / 3;
						double S1 = trianglearea(p1, p01, p12);
						//S2
						Vector3d mid2 = (p2 + p02 + p12) / 3;
						double S2 = trianglearea(p2, p02, p12);
						//�м������μ�������
						//�����е���е�
						Vector3d p0102 = (p01 + p02) / 2;
						Vector3d p0112 = (p01 + p12) / 2;
						Vector3d p0212 = (p02 + p12) / 2;
						//S01
						Vector3d mid01 = (p01 + p0102 + p0112) / 3;
						double S01 = trianglearea(p01, p0102, p0112);
						//S02
						Vector3d mid02 = (p02 + p0102 + p0212) / 3;
						double S02 = trianglearea(p02, p0102, p0212);
						//S12
						Vector3d mid12 = (p12 + p0112 + p0212) / 3;
						double S12 = trianglearea(p12, p0112, p0212);

						//�м������μ�������
						//�����е���е�
						Vector3d p01020112 = (p0102 + p0112) / 2;
						Vector3d p01020212 = (p0112 + p0212) / 2;
						Vector3d p01120212 = (p0112 + p0212) / 2;
						//S0102
						Vector3d mid0102 = (p0102 + p01020112 + p01020212) / 3;
						double S0102 = trianglearea(p0102, p01020112, p01020212);
						//S0112
						Vector3d mid0112 = (p0112 + p01020112 + p01120212) / 3;
						double S0112 = trianglearea(p0112, p01020112, p01120212);
						//S0212
						Vector3d mid0212 = (p0212 + p01020212 + p01120212) / 3;
						double S0212 = trianglearea(p0212, p01020212, p01120212);

						Matrix3d KS = computeKij(x, nx, mid0, ny)*S0
							+ computeKij(x, nx, mid1, ny)*S1
							+ computeKij(x, nx, mid2, ny)*S2
							+ computeKij(x, nx, mid01, ny)*S01
							+ computeKij(x, nx, mid02, ny)*S02
							+ computeKij(x, nx, mid12, ny)*S12
							+ computeKij(x, nx, mid0102, ny)*S0102
							+ computeKij(x, nx, mid0112, ny)*S0112
							+ computeKij(x, nx, mid0212, ny)*S0212;
						//cout << "K" << endl << computeKij(x, nx, y, ny) << endl;						
						coefficient.block(3 * a, 3 * b, 3, 3) = KS - 0.5*identity;
						Matrix3d HS = computeHij(x, nx, mid0, ny)*S0
							+ computeHij(x, nx, mid1, ny)*S1
							+ computeHij(x, nx, mid2, ny)*S2
							+ computeHij(x, nx, mid01, ny)*S01
							+ computeHij(x, nx, mid02, ny)*S02
							+ computeHij(x, nx, mid12, ny)*S12
							+ computeKij(x, nx, mid0102, ny)*S0102
							+ computeKij(x, nx, mid0112, ny)*S0112
							+ computeKij(x, nx, mid0212, ny)*S0212;
						//cout << "H" << endl << computeHij(x, nx, y, ny) << endl;
						H.block(3 * a, 3 * b, 3, 3) = HS;
					}	
					else{//�ǶԽ���
						//cout << "����ǶԽ���" << endl;
						//��������
						Vector3d p0 = m_body[p]->v_onepoint[q].vertex[0];
						Vector3d p1 = m_body[p]->v_onepoint[q].vertex[1];
						Vector3d p2 = m_body[p]->v_onepoint[q].vertex[2];
						//����������е�
						Vector3d p01 = (p0 + p1) / 2;
						Vector3d p02 = (p0 + p2) / 2;
						Vector3d p12 = (p1 + p2) / 2;
						//S0
						Vector3d mid0 = (p0 + p01 + p02) / 3;
						double S0 = trianglearea(p0, p01, p02);
						//S1
						Vector3d mid1 = (p1 + p01 + p12) / 3;
						double S1 = trianglearea(p1, p01, p12);
						//S2
						Vector3d mid2 = (p2 + p02 + p12) / 3;
						double S2 = trianglearea(p2, p02, p12);
						//S3
						Vector3d mid3 = (p01 + p02 + p12) / 3;
						double S3 = trianglearea(p01, p02, p12);
						Matrix3d KS = computeKij(x, nx, mid0, ny)*S0
							+ computeKij(x, nx, mid1, ny)*S1
							+ computeKij(x, nx, mid2, ny)*S2
							+ computeKij(x, nx, mid3, ny)*S3;
						//	Matrix3d KS=	computeKij(x, nx, y, ny)*m_body[p]->v_onepoint[q].area;
							//cout << "K" << endl << computeKij(x, nx, y, ny) << endl;						
						coefficient.block(3 * a, 3 * b, 3, 3) = KS;
						Matrix3d HS = computeHij(x, nx, mid0, ny)*S0
							+ computeHij(x, nx, mid1, ny)*S1
							+ computeHij(x, nx, mid2, ny)*S2
							+ computeHij(x, nx, mid3, ny)*S3;
						//cout<<"��������Բ��ԣ��ĸ����"<<S0+S1+S2+S3<<"area��"<< m_body[p]->v_onepoint[q].area<<endl;
					//	Matrix3d HS = computeHij(x, nx, y, ny)*m_body[p]->v_onepoint[q].area;
						//cout << "H"<<endl << computeHij(x, nx, y, ny) << endl;
						H.block(3 * a, 3 * b, 3, 3) = HS;
					}				
				}
			}
		}
	}
	//for (int i = 0;i < n;i++) {
	//	Matrix3d HSum;
	//	HSum.setZero();
	//	Matrix3d coefficientSum;
	//	coefficientSum.setZero();
	//	for (int j = 0;j < n&&i!=j;j++) {
	//		HSum += H.block(3 * i, 3 * j, 3, 3);
	//		coefficientSum += coefficient.block(3 * i, 3 * j, 3, 3);
	//	}
	//	Matrix3d identity;
	//	identity.setIdentity();
	//	H.block(3 * i, 3 * i, 3, 3) = identity-HSum+0.5*identity;//�Խ����ϵ�����Ԫ��������������
	//	coefficient.block(3 * i, 3 * i, 3, 3) =1.5* identity-coefficientSum;
	//}
	//cout <<"coefficient"<<endl<< coefficient << endl;
	//�����Է�����
	VectorXd b = H * u;
	//cout << "H:" << endl << H << endl;
	//cout << "u:" << endl << u << endl;
	//cout << "b:" << endl << b << endl;
	traction = coefficient.fullPivHouseholderQr().solve(b);//sigma=strength NAN!
	int k = 0;//taction����
	//�����������ϵ���
	for (int p = 0;p < m_body.size();p++) {
		for (int q = 0;q < m_body[p]->v_onepoint.size();q++) {
			//�������е�����������
			double S = m_body[p]->v_onepoint[q].area;
			traction(k) *= S;
			traction(k+1) *= S;
			traction(k+2) *= S;
			k += 3;
		}
	}
	//cout << "�˻�ȥ�Ľ��b:" << endl << coefficient*traction << endl;
	//cout <<"traction:"<< traction << endl;
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
			res(i, j) =( 3 / (4 * 3.1415926*r2))*( (y(i) - x(i) )*( y(j) - x(j)) )/ r2 *rknk;
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
	//Vector3d n = (y-x) / r;//y��x�ĵ�λ����
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
						+ 3 * ( ny(i)*(y(j) - x(j))  * (y(k) - x(k)) / (r2*r2*r) 
										+ ny(k) *(y(i) - x(i)) * (y(j) - x(j) ) / ( r2*r2*r) 
								 )
						+ 2 * dirac(i, k)*ny(j)/(r2*r)
				)*nx(k);
				//cout << "H i =" << i << "j=" << j << H(i, j) << endl;
			}
		}
	}
	return res;
}


