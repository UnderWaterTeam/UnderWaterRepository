#include"DynamicFormula.h"
DynamicFormula::DynamicFormula(Vector3f omega, Vector3f velocity, Matrix3f R,
	Vector3f y, Vector3f ts, Vector3f fs, MatrixXf K, float delta_t)
{
	this->w = omega;
	this->v = velocity;
	this->R = R; //R��γ�ʼ��
	this->y = y;
	this->ts = ts;
	this->fs = fs;
	this->K = K;
	this->delta_t = delta_t;
}

DynamicFormula::~DynamicFormula()
{
}
Matrix3f DynamicFormula::toDaOmegaOrY(Vector3f omega) {
	Matrix3f res;
	res(0, 0) = 0;
	res(0, 1) = -omega(2);
	res(0, 2) = omega(1);
	res(1, 0) = omega(2);
	res(1, 1) = 0;
	res(1, 2) = -omega(0);
	res(2, 0) = -omega(1);
	res(2, 1) = omega(0);
	res(2, 2) = 0;
	return res;
}
VectorXf DynamicFormula::tsfs2tf(Matrix3f R, Matrix3f Y){
	Matrix3f Rt = R.transpose();
	Matrix3f zero=Matrix3f::Zero();
	//zero.setZero(3, 3);
	Matrix3f negRtY =  zero- Rt * Y;
	MatrixXf trans(6, 6);//����ֿ鸳ֵ
	trans.block(0, 0, 3, 3) = Rt.block(0, 0, 3, 3);
	trans.block(0, 3, 3, 3) = negRtY.block(0, 0, 3, 3);
	trans.block(3, 0, 3, 3) = zero.block(0, 0, 3, 3);
	trans.block(3, 3, 3, 3) = Rt.block(0, 0, 3, 3);
	VectorXf tsfs(6);
	tsfs.block(0, 0, 3, 1) = ts;
	tsfs.block(3, 0, 3, 1) = fs;
	/*
	tsfs(0) = ts(0);
	tsfs(1) = ts(1);
	tsfs(2) = ts(2);
	tsfs(3) = fs(0);
	tsfs(4) = fs(1);
	tsfs(5) = fs(2);*/	
	return trans * tsfs;
}
/*
Matrix3f DynamicFormula::computeR_() {
	Matrix3f daOmega = toDaOmegaOrY(w);
	return R * daOmega;
}*/
Vector3f DynamicFormula::computey_() {
	return R * v;
}
VectorXf DynamicFormula::computelp() {
	VectorXf wv(6);
	wv.block(0, 0, 3, 1) = w;
	wv.block(3, 0, 3, 1) = v;
	return K * wv;
}
VectorXf DynamicFormula::computelp_() {
	Matrix3f Y = toDaOmegaOrY(y);
	VectorXf tf= tsfs2tf(R, Y);
	Vector3f l = vec62Vec31(lp);
	Vector3f p = vec62Vec32(lp);
	Vector3f a = l.cross(w) + p.cross(v);
	Vector3f b = p.cross(w);
	VectorXf ab(6);
	ab.block(0, 0, 3, 1) = a;
	ab.block(3, 0, 3, 1) = b;
	return ab + tf;
}
void DynamicFormula::computeNextR() {
	/*
	cout << "q_old:" << q.coeffs() << endl;
	Quaternionf q_w(0, w(0)*delta_t, w(1)*delta_t, w(2)*delta_t);
	float length = sqrt(w(0)*w(0) + w(1)*w(1) + w(2)*w(2));
	q_w.normalize();
	Quaternionf delta_q(delta_t*length, q_w.x(), q_w.y(), q_w.z());
	q = delta_q * q;
	q.normalize();
	R = q.toRotationMatrix();
	cout << "R:" << R << endl;*/
	cout << "q_old:" << q.coeffs() << endl;
	Quaternionf q_w(0, w(0)*delta_t, w(1)*delta_t, w(2)*delta_t);
	q_w *= q;
	q_w.w() = q_w.w()*0.5;
	q_w.x() = q_w.x()*0.5;
	q_w.y() = q_w.y()*0.5;
	q_w.z() = q_w.z()*0.5;
	Quaternionf delta_q=q_w;//��������delta_q
	delta_q.normalize();
	temp_rotate = delta_q.toRotationMatrix();//delta_qת��Ϊ���� display����ջ��������ˣ�ת4*4��
	cout <<"temp_rotate="<< temp_rotate << endl;
	q = delta_q * q ;//���㾭����ת���orientation 
	q.normalize();
	R = q.toRotationMatrix();
	cout << "R:" << R << endl;
	/*
	Quaternionf q_w(0, w(0)*delta_t, w(1)*delta_t, w(2)*delta_t);
	Quaternionf delta_q(q_w.w()+ 1, q_w.x()/2, q_w.y()/2, q_w.z()/2);
	temp_rotate = delta_q.toRotationMatrix();
	q = delta_q * q;
	q.normalize();
	R = q.toRotationMatrix();
	cout << "R:" << R << endl;*/
}
float* DynamicFormula::GetRotationData() {
	float data[16];
	data[0] = temp_rotate(0,0);
	data[1] = temp_rotate(1,0);
	data[2] = temp_rotate(2,0);
	data[3] = y(0);

	data[4] = temp_rotate(0,1);
	data[5] = temp_rotate(1,1);
	data[6] = temp_rotate(2,1);
	data[7] = y(1);

	data[8] = temp_rotate(0,2);
	data[9] = temp_rotate(1,2);
	data[10] = temp_rotate(2,2);
	data[11] = y(2);

	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 1;

	return data;
}
void DynamicFormula::computeNexty( Vector3f y_) {
	temp_deltay = delta_t * y_;
	y= y + temp_deltay;
	cout << "(" << y(0) << "," << y(1) << "," << y(2) << ")" << endl;
}
VectorXf DynamicFormula::computeNextlp() {
	return lp + delta_t * lp_;
}
void DynamicFormula::computeNextwv(VectorXf lp) {
	MatrixXf Kneg = K.inverse();
	VectorXf res=Kneg * lp;
	w = res.block(0, 0, 3, 1);
	v = res.block(3, 0, 3, 1);
}
Vector3f DynamicFormula::vec62Vec31(VectorXf wv) {//Ҳ��������wv
	return wv.block(0, 0, 3, 1);
}
Vector3f DynamicFormula::vec62Vec32(VectorXf wv) {
	return wv.block(3, 0, 3, 1);
}

void DynamicFormula::nextTime() {
	Vector3f y_ = computey_();
	lp = computelp();//ֻҪ�����ʼ��lp,����ÿ�μ���lp
	computeNextR();
	computeNexty(y_);
	cout << "y:" << y << endl;
	lp_ = computelp_();
	lp=computeNextlp();
	cout << "lp:" << lp << endl;
	computeNextwv(lp);
	cout << "w:"<<w(0)<<" " << w(1) << " " << w(2) << endl;
	cout << "v:"<<v(0) << " " << v(1) << " " << v(2) << endl;
}
void DynamicFormula::set_tsfs(Vector3f ts,Vector3f fs) {
	this->ts = ts;
	this->fs = fs;
}