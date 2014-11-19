#include <algorithm>
#include "quaternion.h"
#include "mat.h"

namespace ml{
	
const quaternion quaternion::One = ml::quaternion(0.0f, 0.0f, 0.0f, 1.0f);

quaternion::quaternion(){
	
}

quaternion::quaternion(float f)
	: x(f),
	y(f),
	z(f),
	w(f){
}

quaternion::quaternion(const vec4 &v)
	: x(v.x),
	y(v.y),
	z(v.z),
	w(v.w){
}

quaternion::quaternion(float x_, float y_, float z_, float w_)
	: x(x_),
	y(y_),
	z(z_),
	w(w_){
}

quaternion::quaternion(const vec3 &v, float w_)
	: x(v.x),
	y(v.y),
	z(v.z),
	w(w_){
}

quaternion::quaternion(const mat3 &rot){
	float tr = trace(rot);
	if (tr > 0.0f)
	{
		float s = sqrt(tr + 1.0f);
		w = s * 0.5f;
		float t = 0.5f / s;
		x = (rot.m21 - rot.m12) * t;
		y = (rot.m02 - rot.m20) * t;
		z = (rot.m10 - rot.m01) * t;
	}
	else
	{
		// diagonal is negative
		int i = 0;
		if (rot.m11 > rot.m00) i = 1;
		if (rot.m22 > rot[i][i]) i = 2;

		static const int NEXT[3] = {1, 2, 0};
		int j = NEXT[i];
		int k = NEXT[j];
		float s = sqrt((rot[i][i] - (rot[j][j] + rot[k][k])) + 1.0f);
		elem[i] = s * 0.5f;
		float t;
		if (s != 0.0) t = 0.5f / s;
		else t = s;
		w = (rot[k][j] - rot[j][k]) * t;
		elem[j] = (rot[j][i] + rot[i][j]) * t;
		elem[k] = (rot[k][i] + rot[i][k]) * t;
	}
}

quaternion quaternion::createRotation(float angle, const vec3 &rotationAxis){
	quaternion res;
	float rad = DEG_TO_RAD(angle/2.0f);
	res.v = normalize(rotationAxis) * std::sin(rad);
	res.s = std::cos(rad);
	return res;	
}

quaternion quaternion::lerp(const quaternion &q1, const quaternion &q2, float alpha){
	return normalize((1.0f-alpha)*q1+alpha*q2);
}

quaternion quaternion::slerp(const quaternion &q1, const quaternion &q2, float alpha){
	float dot = ml::dot(q1.vector, q2.vector);

	static const double DOT_THRESHOLD = 0.995;
	if(dot > DOT_THRESHOLD){
		return lerp(q1, q2, alpha);
	}

	dot = std::min(1.0f, std::max(-1.0f, dot)); //clamp to valid range
	float theta = std::acos(dot);
	float sin = std::sin(theta);
	return  std::sin((1.0f-alpha)*theta)/sin * q1 + std::sin(alpha*theta)/sin * q2;

}



quaternion operator+(const quaternion &q1, const quaternion &q2){
	quaternion res;
	for(int i = 0 ; i < 4 ; i++){
		res.elem[i] = q1.elem[i] + q2.elem[i];
	}
	return res;
}

quaternion operator-(const quaternion &q1, const quaternion &q2){
	quaternion res;
	for(int i = 0; i < 4; i++){
		res.elem[i] = q1.elem[i] - q2.elem[i];
	}
	return res;
}

quaternion operator*(const quaternion &q1, const quaternion &q2){
	quaternion res;
	res.v = q1.s*q2.v + q2.s * q1.v + cross(q1.v,q2.v);
	res.s = q1.s*q2.s - dot(q1.v, q2.v);
	return res;
}

quaternion operator/(const quaternion &q1, const quaternion &q2){
	quaternion res;
	for(int i = 0; i < 4 ; i++){
		res.elem[i] = q1.elem[i] / q2.elem[i];
	}
	return res;
}

quaternion& operator+=(quaternion &q1, const quaternion &q2){
	for(int i = 0 ; i < 4 ; i++){
		q1.elem[i] += q2.elem[i];
	}
	return q1;
}
quaternion& operator-=(quaternion &q1, const quaternion &q2){
	for(int i = 0 ; i < 4 ; i++){
		q1.elem[i] -= q2.elem[i];
	}
	return q1;
}
quaternion& operator*=(quaternion &q1, const quaternion &q2){
	q1 = q1 * q2;
	return q1;
}
quaternion& operator/=(quaternion &q1, const quaternion &q2){
	for(int i = 0 ; i < 4; i++){
		q1.elem[i] /= q2.elem[i];
	}
	return q1;
}



//------------------------quaternion/Scalar Operations ------------------------
quaternion operator+(const quaternion &q, float f){
	quaternion res;
	for(int i = 0; i < 4 ; i++){
		res.elem[i] = q.elem[i] + f;
	}
	return res;
}

quaternion operator-(const quaternion &q, float f){
	quaternion res;
	for(int i = 0; i < 4; i++){
		res.elem[i] = q.elem[i] - f;
	}
	return res;
}

quaternion operator*(const quaternion &q, float f){
	quaternion res;
	for(int i = 0; i < 4; i++){
		res.elem[i] = q.elem[i] * f;
	}
	return res;
}

quaternion operator/(const quaternion &q, float f){
	quaternion res; //catch divide by zero ?
	for(int i = 0; i < 4; i++){
		res.elem[i] = q.elem[i] / f;
	}
	return res;
}

quaternion operator+(float f, const quaternion &q){
	quaternion res;
	for(int i = 0; i < 4; i++){
		res.elem[i] = f + q.elem[i];
	}
	return res;
}

quaternion operator-(float f, const quaternion &q){
	quaternion res;
	for(int i = 0; i < 4; i++){
		res.elem[i] = f - q.elem[i];
	}
	return res;
}

quaternion operator*(float f, const quaternion &q){
	quaternion res;
	for(int i = 0 ; i < 4; i++){
		res.elem[i] = f * q.elem[i];
	}
	return res;
}

quaternion operator/(float f, const quaternion &q){
	quaternion res;
	for(int i = 0; i < 4; i++){
		res.elem[i] = f / q.elem[i];
	}
	return res;
}

quaternion& operator+=(quaternion &q, float f){
	for(int i = 0; i < 4 ; i++){
		q.elem[i] += f;
	}
	return q;
}
quaternion& operator-=(quaternion &q, float f){
	for(int i = 0; i < 4 ; i++){
		q.elem[i] -= f;
	}
	return q;
}
quaternion& operator*=(quaternion &q, float f){
	for(int i = 0; i < 4 ; i++){
		q.elem[i] *= f;
	}
	return q;
}
quaternion& operator/=(quaternion &q, float f){
	for(int i = 0; i < 4 ; i++){
		q.elem[i] /= f;
	}
	return q;
}

std::ostream& operator<<(std::ostream &os, const quaternion &q){
	os<<"["<<q.x<<" "<<q.y<<" "<<q.z<<" "<<q.w<<"]";
	return os;
}

//------------------------quaternion Functions------------------------

quaternion conjugate(const quaternion &q){
	return quaternion(-q.x, -q.y, -q.z, q.w);
}
//rot has to be unit length
vec3 rotate(const vec3& v, quaternion rot){
	quaternion tmp(v, 0.0f);
	quaternion res = rot * tmp * conjugate(rot);
	return res.v;
}
//rot has to be unit length
vec3 rotate(const vec4& v, quaternion rot){
	quaternion tmp(v);
	quaternion res = rot * tmp * conjugate(rot);
	return res.v;
}
float squaredLength(const quaternion &q){
	return q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
}

quaternion invert(const quaternion &q){
	return conjugate(q)/squaredLength(q);
}


float length(const quaternion &q){
	return std::sqrtf(squaredLength(q));
}

quaternion normalize(const quaternion &q){
	return q/length(q);
}

}