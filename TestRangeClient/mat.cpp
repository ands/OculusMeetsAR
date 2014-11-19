#include "mat.h"

namespace ml{

const mat2 mat2::Zero = mat2(0.0f);
const mat2 mat2::One = mat2(1.0f);

const mat2x3 mat2x3::Zero = mat2x3(0.0f);
const mat2x3 mat2x3::One = mat2x3(1.0f);

const mat3 mat3::Zero = mat3(0.0f);
const mat3 mat3::One = mat3(1.0f);

const mat3x4 mat3x4::Zero = mat3x4(0.0f);
const mat3x4 mat3x4::One = mat3x4(1.0f);

const mat4 mat4::Zero = mat4(0.0f);
const mat4 mat4::One = mat4(1.0f);


float trace(const mat2 &m){
	return m.m00 + m.m11;
}

float trace(const mat2x3 &m){
	return m.m00 + m.m11 + 1.0f;
}

float trace(const mat3 &m){
	return m.m00 + m.m11 + m.m22;
}

float trace(const mat3x4 &m){
	return m.m00 + m.m11 + m.m22 + 1.0f;
}

float trace(const mat4 &m){
	return m.m00 + m.m11 + m.m22 + m.m33;
}

mat2 transpose(const mat2 &m){
	return mat2(m.m00, m.m10, m.m01, m.m11);
}

mat3 transpose(const mat2x3 &m){
	return mat3(m.m00, m.m10, 0.0f,
				m.m01, m.m11, 0.0f,
				m.m02, m.m12, 1.0f);
}

mat3 transpose(const mat3 &m){
	return mat3(m.m00, m.m10, m.m20,
				m.m01, m.m11, m.m21,
				m.m02, m.m12, m.m22);
}

mat4 transpose(const mat3x4 &m){
	return mat4(m.m00, m.m10, m.m20, 0.0f,
				m.m01, m.m11, m.m21, 0.0f,
				m.m02, m.m12, m.m22, 0.0f,
				m.m03, m.m13, m.m23, 1.0f);
}

mat4 transpose(const mat4 &m){
	return mat4(m.m00, m.m10, m.m20, m.m30,
				m.m01, m.m11, m.m21, m.m31,
				m.m02, m.m12, m.m22, m.m32,
				m.m03, m.m13, m.m23, m.m33);
}

float det(const mat2 &m){
	return m.m00 * m.m11 - m.m01 * m.m10;
}

float det(const mat2x3 &m){
	return m.m00 * m.m11 - m.m01 * m.m10;
}

float det(const mat3 &m){
	return dot(m.row[0], cross(m.row[1], m.row[2]));
}

float det(const mat3x4 &m){
	return -1;
}

float det(const mat4 &m){
	return m.m00*m.m11*m.m22*m.m33 - m.m00*m.m11*m.m23*m.m32 + m.m00*m.m12*m.m23*m.m31 - m.m00*m.m12*m.m21*m.m33
		+ m.m00*m.m13*m.m21*m.m32 - m.m00*m.m13*m.m22*m.m31 - m.m01*m.m12*m.m23*m.m30 + m.m01*m.m12*m.m20*m.m33
		- m.m01*m.m13*m.m20*m.m32 + m.m01*m.m13*m.m22*m.m30 - m.m01*m.m10*m.m22*m.m33 + m.m01*m.m10*m.m23*m.m32
		+ m.m02*m.m13*m.m20*m.m31 - m.m02*m.m13*m.m21*m.m30 + m.m02*m.m10*m.m21*m.m33 - m.m02*m.m10*m.m23*m.m31
		+ m.m02*m.m11*m.m23*m.m30 - m.m02*m.m11*m.m20*m.m33 - m.m03*m.m10*m.m21*m.m32 + m.m03*m.m10*m.m22*m.m31
		- m.m03*m.m11*m.m22*m.m30 + m.m03*m.m11*m.m20*m.m32 - m.m03*m.m12*m.m20*m.m31 + m.m03*m.m12*m.m21*m.m30;
}

mat2 matrixCompMult(const mat2 &m1, const mat2 &m2){
	return mat2(m1.m00 * m2.m00, m1.m01 * m2.m01,
				m1.m10 * m2.m10, m1.m11 * m2.m11);
}

mat2x3 matrixCompMult(const mat2x3 &m1, const mat2x3 &m2){
	return mat2x3(m1.m00 * m2.m00, m1.m01 * m2.m01, m1.m02 * m2.m02,
				  m1.m10 * m2.m10, m1.m11 * m2.m11, m1.m12 * m2.m12);
}

mat3 matrixCompMult(const mat3 &m1, const mat3 &m2){
	return mat3(m1.m00 * m2.m00, m1.m01 * m2.m01, m1.m02 * m2.m02,
				m1.m10 * m2.m10, m1.m11 * m2.m11, m1.m12 * m2.m12,
				m1.m20 * m2.m20, m1.m21 * m2.m21, m1.m22 * m2.m22);
}

mat3x4 matrixCompMult(const mat3x4 &m1, const mat3x4 &m2){
	return mat3x4(m1.m00 * m2.m00, m1.m01 * m2.m01, m1.m02 * m2.m02, m1.m03 * m2.m03,
				  m1.m10 * m2.m10, m1.m11 * m2.m11, m1.m12 * m2.m12, m1.m13 * m2.m13,
				  m1.m20 * m2.m20, m1.m21 * m2.m21, m1.m22 * m2.m22, m1.m23 * m2.m23);
}

mat4 matrixCompMult(const mat4 &m1, const mat4 &m2){
	return mat4(m1.m00 * m2.m00, m1.m01 * m2.m01, m1.m02 * m2.m02, m1.m03 * m2.m03,
				m1.m10 * m2.m10, m1.m11 * m2.m11, m1.m12 * m2.m12, m1.m13 * m2.m13,
				m1.m20 * m2.m20, m1.m21 * m2.m21, m1.m22 * m2.m22, m1.m23 * m2.m23,
				m1.m30 * m2.m30, m1.m31 * m2.m31, m1.m32 * m2.m32, m1.m33 * m2.m33);
}

std::ostream& operator<<(std::ostream& os, const mat2 &m){
	os<<"["<<m.row[0]<<"\n"<<m.row[1]<<"]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const mat2x3 &m){
	os<<"["<<m.row[0]<<"\n"<<m.row[1]<<"]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const mat3 &m){
	os<<"["<<m.row[0]<<"\n"<<m.row[1]<<"\n"<<m.row[2]<<"]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const mat3x4 &m){
	os<<"["<<m.row[0]<<"\n"<<m.row[1]<<"\n"<<m.row[2]<<"]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const mat4 &m){
	os<<"["<<m.row[0]<<"\n"<<m.row[1]<<"\n"<<m.row[2]<<"\n"<<m.row[3]<<"]";
	return os;
}

} //namespace ml