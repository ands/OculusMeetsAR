#ifndef ML_MAT_H
#define ML_MAT_H

#include "vec.h"
#include "quaternion.h"

namespace ml{

struct mat2;
struct mat2x3;
struct mat3;
struct mat3x4;
struct mat4;
struct quaternion;
mat4 operator*(const mat4 &m1, const mat4 &m2);

float trace(const mat2 &m);
float trace(const mat2x3 &m);
float trace(const mat3 &m);
float trace(const mat3x4 &m);
float trace(const mat4 &m);

mat2 transpose(const mat2 &m);
mat3 transpose(const mat2x3 &m);
mat3 transpose(const mat3 &m);
mat4 transpose(const mat3x4 &m);
mat4 transpose(const mat4 &m);

float det(const mat2 &m);
float det(const mat2x3 &m);
float det(const mat3 &m);
float det(const mat3x4 &m);
float det(const mat4 &m);

mat2 matrixCompMult(const mat2 &m1, const mat2 &m2);
mat2x3 matrixCompMult(const mat2x3 &m1, const mat2x3 &m2);
mat3 matrixCompMult(const mat3 &m1, const mat3 &m2);
mat3x4 matrixCompMult(const mat3x4 &m1, const mat3x4 &m2);
mat4 matrixCompMult(const mat4 &m1, const mat4 &m2);

std::ostream& operator<<(std::ostream& os, const mat2 &m);
std::ostream& operator<<(std::ostream& os, const mat2x3 &m);
std::ostream& operator<<(std::ostream& os, const mat3 &m);
std::ostream& operator<<(std::ostream& os, const mat3x4 &m);
std::ostream& operator<<(std::ostream& os, const mat4 &m);

struct mat2{
	enum {size = 4, rows = 2, cols = 2};
	union{
		struct{
			float m00, m01;
			float m10, m11;
		};
		struct{
			vec2 row[rows];
		};
		float elemMat[rows][cols];
		float elem[size];
	};

	mat2() {}
	explicit mat2(float value) : m00(value), m01(0.0f), m10(0.0f), m11(value) {}
	explicit mat2(float _m00, float _m11) : m00(_m00), m01(0.0f), m10(0.0f), m11(_m11) {}
	explicit mat2(float _m00, float _m01, float _m10, float _m11) 
		: m00(_m00), m01(_m01), m10(_m10), m11(_m11) {}
	explicit mat2(const vec2 &r1, const vec2 &r2) : m00(r1.x), m01(r1.y), m10(r2.x), m11(r2.y) {}

	vec2& operator[](size_t index){
		assert(index >= 0 && index < rows);
		return row[index];
	}

	const vec2& operator[](size_t index)const{
		assert(index >= 0 && index < rows);
		return row[index];
	}

	static const mat2 Zero;
	static const mat2 One;
};

/************************************************************************************
This Matrix represents a 3x3 matrix with an implicit 3rd line of [0, 0, 1]
for 2d affine transformations
************************************************************************************/
struct mat2x3{
	enum{size = 6, rows = 2, cols = 3};
	union{
		struct{
			float m00, m01, m02;
			float m10, m11, m12;
		};
		struct{
			vec3 row[rows];
		};
		float elemMat[rows][cols];
		float elem[size];
	};

	mat2x3(){}
	explicit mat2x3(float val) : m00(val), m01(0.0f), m02(0.0f), m10(0.0f), m11(val), m12(0.0f) {}
	explicit mat2x3(float _m00, float _m11) : m00(_m00), m01(0.0f), m02(0.0f), m10(0.0f), m11(_m11), m12(0.0f) {}
	explicit mat2x3(float _m00, float _m01, float _m02,
					float _m10, float _m11, float _m12) 
					: m00(_m00), m01(_m01), m02(_m02),
					m10(_m10), m11(_m11), m12(_m12) {}
	explicit mat2x3(const vec3 &r1, const vec3 &r2)
					: m00(r1.x), m01(r1.y), m02(r1.z),
					m10(r2.x), m11(r2.y), m12(r2.z) {}

	static mat2x3 createRotation(float angle){
		float rad = DEG_TO_RAD(angle);
		float cos = std::cos(rad);
		float sin = std::sin(rad);
		return mat2x3(cos , -sin, 0.0f,
					sin , cos , 0.0f);
	}

	static mat2x3 createScale(const vec2 &s){
		return mat2x3(s.x , 0.0f, 0.0f,
					0.0f, s.y , 0.0f);
	}

	vec3& operator[](size_t index){
		assert(index >= 0 && index < rows);
		return row[index];
	}

	const vec3& operator[](size_t index) const{
		assert(index >= 0 && index < rows);
		return row[index];
	}

	static const mat2x3 Zero;
	static const mat2x3 One;
};

struct mat3{
	enum {size = 9, rows = 3, cols = 3};
	union{
		struct{
			float m00, m01, m02;
			float m10, m11, m12;
			float m20, m21, m22;
		};
		struct{
			vec3 row[rows];
		};
		float elemMat[rows][cols];
		float elem[size];
	};

	mat3(){}
	explicit mat3(float value) : m00(value), m01(0.0f), m02(0.0f),
								m10(0.0f), m11(value), m12(0.0f),
								m20(0.0f), m21(0.0f), m22(value) {}
	explicit mat3(float _m00, float _m11, float _m22) : m00(_m00), m01(0.0f), m02(0.0f),
														m10(0.0f), m11(_m11), m12(0.0f),
														m20(0.0f), m21(0.0f), m22(_m22) {}
	explicit mat3(float _m00, float _m01, float _m02,
				  float _m10, float _m11, float _m12,
				  float _m20, float _m21, float _m22) 
				  : m00(_m00), m01(_m01), m02(_m02),
					m10(_m10), m11(_m11), m12(_m12),
					m20(_m20), m21(_m21), m22(_m22) {}
	explicit mat3(const vec3 &r1, const vec3 &r2, const vec3 &r3) : m00(r1.x), m01(r1.y), m02(r1.z),
																	m10(r2.x), m11(r2.y), m12(r2.z),
																	m20(r3.x), m21(r3.y), m22(r3.z) {}

	
	
	static mat3 createRotationX(float angle){
		float rad = DEG_TO_RAD(angle);
		float cos = std::cos(rad);
		float sin = std::sin(rad);
		return mat3(1.0f, 0.0f, 0.0f,
					0.0f, cos , -sin,
					0.0f, sin , cos);	
	}

	static mat3 createRotationY(float angle){
		float rad = DEG_TO_RAD(angle);
		float cos = std::cos(rad);
		float sin = std::sin(rad);
		return mat3(cos , 0.0f, sin ,
					0.0f, 1.0f, 0.0f,
					-sin, 0.0f, cos);	
	}

	static mat3 createRotationZ(float angle){
		float rad = DEG_TO_RAD(angle);
		float cos = std::cos(rad);
		float sin = std::sin(rad);
		return mat3(cos , -sin, 0.0f,
					sin , cos , 0.0f,
					0.0f, 0.0f, 1.0f);
	}

	static mat3 createRotation(float angle, const vec3 &rotAxis){
		float rad = DEG_TO_RAD(angle);
		float cos = std::cos(rad);
		float sin = std::sin(rad);
		float mcos = 1.0f - cos;
		return mat3(cos + rotAxis.x * rotAxis.x * mcos, rotAxis.x * rotAxis.y * mcos - rotAxis.z * sin, rotAxis.x * rotAxis.z * mcos + rotAxis.y * sin,
			rotAxis.x * rotAxis.y * mcos + rotAxis.z * sin, cos + rotAxis.y * rotAxis.y * mcos, rotAxis.y * rotAxis.z * mcos - rotAxis.x * sin,
			rotAxis.z * rotAxis.x * mcos - rotAxis.y * sin, rotAxis.z * rotAxis.y * mcos + rotAxis.x * sin, cos + rotAxis.z * rotAxis.z * mcos);
	}

	static mat3 createScale(const vec3 &s){
		return mat3(s.x , 0.0f, 0.0f,
					0.0f, s.y , 0.0f,
					0.0f, 0.0f, s.z);
	}

	vec3& operator[](size_t index){
		assert(index >= 0 && index < rows);
		return row[index];
	}

	const vec3& operator[](size_t index) const{
		assert(index >= 0 && index < rows);
		return row[index];
	}


	static const mat3 Zero;
	static const mat3 One;
};

/************************************************************************************
This Matrix represents a 4x4 matrix with an implicit 4th line of [0, 0, 0, 1]
for 3d affine transformations COLUMN MAJOR!
************************************************************************************/
struct mat3x4{
	enum{size = 12, rows = 3, cols = 4};
	union{
		struct{
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
		};
		struct{
			vec4 row[rows];
		};
		float elemMat[rows][cols];
		float elem[size];
	};
	
	mat3x4(){}
	explicit mat3x4(float val) 
		: m00(val), m01(0.0f), m02(0.0f), m03(0.0f),
		m10(0.0f), m11(val), m12(0.0f), m13(0.0f),
		m20(0.0f), m21(0.0f), m22(val), m23(0.0f){}
	explicit mat3x4(float _m00, float _m11, float _m22)
		: m00(_m00), m01(0.0f), m02(0.0f), m03(0.0f),
		m10(0.0f), m11(_m11), m12(0.0f), m13(0.0f),
		m20(0.0f), m21(0.0f), m22(_m22), m23(0.0f){}
	explicit mat3x4(float _m00, float _m01, float _m02, float _m03,
					float _m10, float _m11, float _m12, float _m13,
					float _m20, float _m21, float _m22, float _m23) 
					: m00(_m00), m01(_m01), m02(_m02), m03(_m03),
					m10(_m10), m11(_m11), m12(_m12), m13(_m13),
					m20(_m20), m21(_m21), m22(_m22), m23(_m23){}
	explicit mat3x4(const vec4 &r1, const vec4 &r2, const vec4 &r3)
					: m00(r1.x), m01(r1.y), m02(r1.z), m03(r1.w),
					m10(r2.x), m11(r2.y), m12(r2.z) , m13(r2.w),
					m20(r3.x), m21(r3.y), m22(r3.z), m23(r3.w){}


	static mat3x4 createRotationX(float angle){
		float rad = DEG_TO_RAD(angle);
		float cos = std::cos(rad);
		float sin = std::sin(rad);
		return mat3x4(1.0f, 0.0f, 0.0f, 0.0f,
					  0.0f, cos , -sin, 0.0f,
					  0.0f, sin , cos , 0.0f);	
	}

	static mat3x4 createRotationY(float angle){
		float rad = DEG_TO_RAD(angle);
		float cos = std::cos(rad);
		float sin = std::sin(rad);
		return mat3x4(cos, 0.0f, sin, 0.0f,
					  0.0f, 1.0f , 0.0f, 0.0f,
					  -sin, 0.0f , cos , 0.0f);	
	}

	static mat3x4 createRotationZ(float angle){
		float rad = DEG_TO_RAD(angle);
		float cos = std::cos(rad);
		float sin = std::sin(rad);
		return mat3x4(cos, -sin, 0.0f, 0.0f,
					  sin, cos, 0.0f, 0.0f,
					  0.0f, 0.0f , 1.0f , 0.0f);
	}

	static mat3x4 createRotation(float angle, const vec3 &rotAxis){
		float rad = DEG_TO_RAD(angle);
		float cos = std::cos(rad);
		float sin = std::sin(rad);
		float mcos = 1.0f - cos;
		return mat3x4(cos + rotAxis.x * rotAxis.x * mcos, rotAxis.x * rotAxis.y * mcos - rotAxis.z * sin, rotAxis.x * rotAxis.z * mcos + rotAxis.y * sin, 0.0f,
			rotAxis.x * rotAxis.y * mcos + rotAxis.z * sin, cos + rotAxis.y * rotAxis.y * mcos, rotAxis.y * rotAxis.z * mcos - rotAxis.x * sin, 0.0f,
			rotAxis.z * rotAxis.x * mcos - rotAxis.y * sin, rotAxis.z * rotAxis.y * mcos + rotAxis.x * sin, cos + rotAxis.z * rotAxis.z * mcos, 0.0f);
	}

	static mat3x4 createTranslation(const vec3 &t){
		return mat3x4(1.0f, 0.0f, 0.0f, t.x,
					  0.0f, 1.0f, 0.0f, t.y,
					  0.0f, 0.0f, 1.0f, t.z);
	}

	static mat3x4 createScale(const vec3 &s){
		return mat3x4(s.x , 0.0f, 0.0f, 0.0f,
					0.0f, s.y , 0.0f, 0.0f,
					0.0f, 0.0f, s.z , 0.0f);
	}

	static mat3x4 createViewMatrix(const vec3 &cameraPosition, const vec3 &cameraFocus, const vec3 &up){
		vec3 zaxis = normalize(cameraFocus - cameraPosition);
		vec3 xaxis = normalize(cross(up, zaxis));
		vec3 yaxis = cross(zaxis, xaxis);

		return mat3x4(xaxis.x, xaxis.y, xaxis.z, -dot(xaxis, cameraPosition),
					yaxis.x, yaxis.y, yaxis.z, -dot(yaxis, cameraPosition),
					zaxis.x, zaxis.y, zaxis.z, -dot(zaxis, cameraPosition));
	}

	static mat3x4 createOrthographicProjection(const vec3 &llf, const vec3 &urb){
		return mat3x4(2.0f/(urb.y-llf.y), 0.0f, 0.0f, -(urb.y+llf.y)/(urb.y-llf.y),
					0.0f, 2.0f/(urb.x-llf.x), 0.0f, -(urb.x+llf.x)/(urb.x-llf.x),
					0.0f, 0.0f, -2.0f/(urb.z-llf.z), (urb.z+llf.z)/(urb.z-llf.z));
	}

	vec4& operator[](size_t index){
		assert(index >= 0 && index < rows);
		return row[index];
	}

	const vec4& operator[](size_t index) const{
		assert(index >= 0 && index < rows);
		return row[index];
	}

	static const mat3x4 Zero;
	static const mat3x4 One;
};

struct mat4{
	enum {size = 16, rows = 4, cols = 4};
	union{
		struct{
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};
		struct{
			vec4 row[rows];
		};
		float elemMat[rows][cols];
		float elem[size];
	};

	mat4(){}
	explicit mat4(float value) : m00(value), m01(0.0f), m02(0.0f), m03(0.0f),
								m10(0.0f), m11(value), m12(0.0f), m13(0.0f),
								m20(0.0f), m21(0.0f), m22(value), m23(0.0f),
								m30(0.0f), m31(0.0f), m32(0.0f), m33(value) {}
	explicit mat4(float _m00, float _m11, float _m22, float _m33) 
								: m00(_m00), m01(0.0f), m02(0.0f), m03(0.0f),
								m10(0.0f), m11(_m11), m12(0.0f), m13(0.0f),
								m20(0.0f), m21(0.0f), m22(_m22), m23(0.0f),
								m30(0.0f), m31(0.0f), m32(0.0f), m33(_m33) {}
	explicit mat4(float _m00, float _m01, float _m02, float _m03,
				  float _m10, float _m11, float _m12, float _m13,
				  float _m20, float _m21, float _m22, float _m23,
				  float _m30, float _m31, float _m32, float _m33) 
					: m00(_m00), m01(_m01), m02(_m02), m03(_m03),
					m10(_m10), m11(_m11), m12(_m12), m13(_m13),
					m20(_m20), m21(_m21), m22(_m22), m23(_m23),
					m30(_m30), m31(_m31), m32(_m32), m33(_m33) {}
	explicit mat4(const vec4 &r1, const vec4 &r2, const vec4 &r3, const vec4 &r4) 
								: m00(r1.x), m01(r1.y), m02(r1.z), m03(r1.w),
								m10(r2.x), m11(r2.y), m12(r2.z), m13(r2.w),
								m20(r3.x), m21(r3.y), m22(r3.z), m23(r3.w),
								m30(r4.x), m31(r4.y), m32(r4.z), m33(r4.w) {}

	static mat4 createRotationX(float angle){
		float rad = DEG_TO_RAD(angle);
		float cos = std::cos(rad);
		float sin = std::sin(rad);
		return mat4(1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, cos , -sin, 0.0f,
					0.0f, sin , cos , 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);	
	}

	static mat4 createRotationY(float angle){
		float rad = DEG_TO_RAD(angle);
		float cos = std::cos(rad);
		float sin = std::sin(rad);
		return mat4(cos, 0.0f, sin, 0.0f,
					0.0f, 1.0f , 0.0f, 0.0f,
					-sin, 0.0f , cos , 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);	
	}

	static mat4 createRotationZ(float angle){
		float rad = DEG_TO_RAD(angle);
		float cos = std::cos(rad);
		float sin = std::sin(rad);
		return mat4(cos, -sin, 0.0f, 0.0f,
					sin, cos, 0.0f, 0.0f,
					0.0f, 0.0f , 1.0f , 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
	}

	static mat4 createRotation(float angle, const vec3 &rotAxis){
		float rad = DEG_TO_RAD(angle);
		float cos = std::cos(rad);
		float sin = std::sin(rad);
		float mcos = 1.0f - cos;
		return mat4(cos + rotAxis.x * rotAxis.x * mcos, rotAxis.x * rotAxis.y * mcos - rotAxis.z * sin, rotAxis.x * rotAxis.z * mcos + rotAxis.y * sin, 0.0f,
			rotAxis.x * rotAxis.y * mcos + rotAxis.z * sin, cos + rotAxis.y * rotAxis.y * mcos, rotAxis.y * rotAxis.z * mcos - rotAxis.x * sin, 0.0f,
			rotAxis.z * rotAxis.x * mcos - rotAxis.y * sin, rotAxis.z * rotAxis.y * mcos + rotAxis.x * sin, cos + rotAxis.z * rotAxis.z * mcos, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	static mat4 createRotation(const quaternion &quat){
		mat4 mat = ml::mat4(1.0f);

		mat[0][0] = 1 - 2 * quat.y * quat.y - 2 * quat.z * quat.z;
		mat[0][1] = 2 * quat.x *quat.y - 2 * quat.z * quat.w;
		mat[0][2] = 2 * quat.x * quat.z + 2 * quat.y * quat.w;

		mat[1][0] = 2 * quat.x * quat.y + 2 * quat.z * quat.w;
		mat[1][1] = 1 - 2 * quat.x * quat.x - 2 * quat.z * quat.z;
		mat[1][2] = 2 * quat.y * quat.z - 2 * quat.x * quat.w;

		mat[2][0] = 2 * quat.x * quat.z - 2 * quat.y * quat.w;
		mat[2][1] = 2 * quat.y * quat.z + 2 * quat.x * quat.w;
		mat[2][2] = 1 - 2 * quat.x * quat.x - 2 * quat.y * quat.y;

		return mat;
	}

	static void decompose(ml::vec3 &pos, quaternion &rot, ml::vec3 scale, const mat4 &transform){
		pos = ml::vec3(transform.m03, transform.m13, transform.m23);
		
		ml::vec3 column0(transform.m00, transform.m10, transform.m20);
		ml::vec3 column1(transform.m01, transform.m11, transform.m21);
		ml::vec3 column2(transform.m02, transform.m12, transform.m22);

		scale = ml::vec3(length(column0), length(column1), length(column2));
		if(det(transform) < 0){
			scale *= -1.0f;
		}

		if(scale.x != 0.0f){
			column0 /= scale.x;
		}
		if(scale.y != 0.0f){
			column1 /= scale.y;
		}
		if(scale.z != 0.0f){
			column2 /= scale.z;
		}

		mat3 rotation(column0.x, column1.x, column2.x,
					  column0.y, column1.y, column2.y,
					  column0.z, column1.z, column2.z);

		rot = ml::quaternion(rotation);
	}

	static mat4 createTranslation(const vec3 &t){
		return mat4(
			1.0f, 0.0f, 0.0f, t.x,
			0.0f, 1.0f, 0.0f, t.y,
			0.0f, 0.0f, 1.0f, t.z,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	static mat4 createScale(const vec3 &s){
		return mat4(s.x , 0.0f, 0.0f, 0.0f,
					0.0f, s.y , 0.0f, 0.0f,
					0.0f, 0.0f, s.z , 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
	}

	static mat4 createAffineTransform(const vec3 &translation, const quaternion &rotation, const vec3 &scale){
		return createTranslation(translation) * createRotation(rotation) * createScale(scale);
	}

	static mat4 createViewMatrix(const vec3 &cameraPosition, const vec3 &cameraFocus, const vec3 &up){
		vec3 zaxis = normalize(cameraFocus - cameraPosition);
		vec3 xaxis = normalize(cross(up, zaxis));
		vec3 yaxis = cross(zaxis, xaxis);

		return mat4(xaxis.x, xaxis.y, xaxis.z, -dot(xaxis, cameraPosition),
					yaxis.x, yaxis.y, yaxis.z, -dot(yaxis, cameraPosition),
					zaxis.x, zaxis.y, zaxis.z, -dot(zaxis, cameraPosition),
					0.0f, 0.0f, 0.0f, 1.0f);
	}

	static mat4 createOrthographicProjection(const vec3 &llf, const vec3 &urb){
		return mat4(2.0f/(urb.y-llf.y), 0.0f, 0.0f, -(urb.y+llf.y)/(urb.y-llf.y),
					0.0f, 2.0f/(urb.x-llf.x), 0.0f, -(urb.x+llf.x)/(urb.x-llf.x),
					0.0f, 0.0f, -2.0f/(urb.z-llf.z), (urb.z+llf.z)/(urb.z-llf.z),
					0.0f, 0.0f, 0.0f, 1.0f);
	}

	static mat4 createPerspectiveProjection(float p_ratio, float p_fovy, float p_near, float p_far){
		float top = -p_near * (std::tanf(DEG_TO_RAD(p_fovy)/2.0f));
		float bottom = -top;
		float left = bottom * p_ratio;
		float right = top * p_ratio;
		return mat4(-2 * p_near / (right - left), 0.0f, (right + left) / (right - left), 0.0f,  
					0.0f, -2 * p_near / (top - bottom), (top + bottom) / (top - bottom), 0.0f,  
					0.0f, 0.0f,	(p_near + p_far) / (p_near - p_far), -2 * p_near * p_far / (p_near - p_far), 
					0.0f, 0.0f, -1.0f, 0.0f ); 
	}

	vec4& operator[](size_t index){
		assert(index >= 0 && index < rows);
		return row[index];
	}

	const vec4& operator[](size_t index)const{
		assert(index >= 0 && index < rows);
		return row[index];
	}

	static const mat4 Zero;
	static const mat4 One;
};



inline mat2 operator+(const mat2 &m1, const mat2 &m2){
	return mat2(m1.m00 + m2.m00, m1.m01 + m2.m01,
				m1.m10 + m2.m10, m1.m11 + m2.m11);
}

inline mat3 operator+(const mat3 &m1, const mat3 &m2){
	return mat3(m1.m00 + m2.m00, m1.m01 + m2.m01, m1.m02 + m2.m02,
				m1.m10 + m2.m10, m1.m11 + m2.m11, m1.m12 + m2.m12,
				m1.m20 + m2.m20, m1.m21 + m2.m21, m1.m22 + m2.m22);
}

inline mat4 operator+(const mat4 &m1, const mat4 &m2){
	return mat4(m1.m00 + m2.m00, m1.m01 + m2.m01, m1.m02 + m2.m02, m1.m03 + m2.m03,
				m1.m10 + m2.m10, m1.m11 + m2.m11, m1.m12 + m2.m12, m1.m13 + m2.m13,
				m1.m20 + m2.m20, m1.m21 + m2.m21, m1.m22 + m2.m22, m1.m23 + m2.m23,
				m1.m30 + m2.m30, m1.m31 + m2.m31, m1.m32 + m2.m32, m1.m33 + m2.m33);
}

inline mat2 operator-(const mat2 &m1, const mat2 &m2){
	return mat2(m1.m00 - m2.m00, m1.m01 - m2.m01,
				m1.m10 - m2.m10, m1.m11 - m2.m11);
}

inline mat3 operator-(const mat3 &m1, const mat3 &m2){
	return mat3(m1.m00 - m2.m00, m1.m01 - m2.m01, m1.m02 - m2.m02,
				m1.m10 - m2.m10, m1.m11 - m2.m11, m1.m12 - m2.m12,
				m1.m20 - m2.m20, m1.m21 - m2.m21, m1.m22 - m2.m22);
}

inline mat4 operator-(const mat4 &m1, const mat4 &m2){
	return mat4(m1.m00 - m2.m00, m1.m01 - m2.m01, m1.m02 - m2.m02, m1.m03 - m2.m03,
				m1.m10 - m2.m10, m1.m11 - m2.m11, m1.m12 - m2.m12, m1.m13 - m2.m13,
				m1.m20 - m2.m20, m1.m21 - m2.m21, m1.m22 - m2.m22, m1.m23 - m2.m23,
				m1.m30 - m2.m30, m1.m31 - m2.m31, m1.m32 - m2.m32, m1.m33 - m2.m33);
}

inline mat2 operator*(const mat2 &m1, const mat2 &m2){
	return mat2(m1.m00 * m2.m00 + m1.m01 * m2.m10, m1.m00 * m2.m01 + m1.m01 * m2.m11,
				m1.m10 * m2.m00 + m1.m11 * m2.m10, m1.m10 * m2.m01 + m1.m11 * m2.m11);
}

inline mat3 operator*(const mat3 &m1, const mat3 &m2){
	return mat3(m1.m00 * m2.m00 + m1.m01 * m2.m10 + m1.m02 * m2.m20, m1.m00 * m2.m01 + m1.m01 * m2.m11 + m1.m02 * m2.m21, m1.m00 * m2.m02 + m1.m01 * m2.m12 + m1.m02 * m2.m22,
				m1.m10 * m2.m00 + m1.m11 * m2.m10 + m1.m12 * m2.m20, m1.m10 * m2.m01 + m1.m11 * m2.m11 + m1.m12 * m2.m21, m1.m10 * m2.m02 + m1.m11 * m2.m12 + m1.m12 * m2.m22,
				m1.m20 * m2.m00 + m1.m21 * m2.m10 + m1.m22 * m2.m20, m1.m20 * m2.m01 + m1.m21 * m2.m11 + m1.m22 * m2.m21, m1.m20 * m2.m02 + m1.m21 * m2.m12 + m1.m22 * m2.m22);
}

inline mat4 operator*(const mat4 &m1, const mat4 &m2){
	return mat4(m1.m00 * m2.m00 + m1.m01 * m2.m10 + m1.m02 * m2.m20 + m1.m03 * m2.m30, m1.m00 * m2.m01 + m1.m01 * m2.m11 + m1.m02 * m2.m21 + m1.m03 * m2.m31, m1.m00 * m2.m02 + m1.m01 * m2.m12 + m1.m02 * m2.m22 + m1.m03 * m2.m32, m1.m00 * m2.m03 + m1.m01 * m2.m13 + m1.m02 * m2.m23 + m1.m03 * m2.m33,
		m1.m10 * m2.m00 + m1.m11 * m2.m10 + m1.m12 * m2.m20 + m1.m13 * m2.m30, m1.m10 * m2.m01 + m1.m11 * m2.m11 + m1.m12 * m2.m21 + m1.m13 * m2.m31, m1.m10 * m2.m02 + m1.m11 * m2.m12 + m1.m12 * m2.m22 + m1.m13 * m2.m32, m1.m10 * m2.m03 + m1.m11 * m2.m13 + m1.m12 * m2.m23 + m1.m13 * m2.m33,
		m1.m20 * m2.m00 + m1.m21 * m2.m10 + m1.m22 * m2.m20 + m1.m23 * m2.m30, m1.m20 * m2.m01 + m1.m21 * m2.m11 + m1.m22 * m2.m21 + m1.m23 * m2.m31, m1.m20 * m2.m02 + m1.m21 * m2.m12 + m1.m22 * m2.m22 + m1.m23 * m2.m32, m1.m20 * m2.m03 + m1.m21 * m2.m13 + m1.m22 * m2.m23 + m1.m23 * m2.m33,
		m1.m30 * m2.m00 + m1.m31 * m2.m10 + m1.m32 * m2.m20 + m1.m33 * m2.m30, m1.m30 * m2.m01 + m1.m31 * m2.m11 + m1.m32 * m2.m21 + m1.m33 * m2.m31, m1.m30 * m2.m02 + m1.m31 * m2.m12 + m1.m32 * m2.m22 + m1.m33 * m2.m32, m1.m30 * m2.m03 + m1.m31 * m2.m13 + m1.m32 * m2.m23 + m1.m33 * m2.m33);
}

inline mat2 operator/(const mat2 &m1, const mat2 &m2){
	return mat2(m1.m00 / m2.m00, m1.m01 / m2.m01,
				m1.m10 / m2.m10, m1.m11 / m2.m11);
}

inline mat3 operator/(const mat3 &m1, const mat3 &m2){
	return mat3(m1.m00 / m2.m00, m1.m01 / m2.m01, m1.m02 / m2.m02,
				m1.m10 / m2.m10, m1.m11 / m2.m11, m1.m12 / m2.m12,
				m1.m20 / m2.m20, m1.m21 / m2.m21, m1.m22 / m2.m22);
}

inline mat4 operator/(const mat4 &m1, const mat4 &m2){
	return mat4(m1.m00 / m2.m00, m1.m01 / m2.m01, m1.m02 / m2.m02, m1.m03 / m2.m03,
				m1.m10 / m2.m10, m1.m11 / m2.m11, m1.m12 / m2.m12, m1.m13 / m2.m13,
				m1.m20 / m2.m20, m1.m21 / m2.m21, m1.m22 / m2.m22, m1.m23 / m2.m23,
				m1.m30 / m2.m30, m1.m31 / m2.m31, m1.m32 / m2.m32, m1.m33 / m2.m33);
}

inline vec2 operator*(const mat2 &m, const vec2 &v){
	return vec2(m.m00 * v.x + m.m01 * v.y, m.m10 * v.x + m.m11 * v.y);
}

inline vec3 operator*(const mat3 &m, const vec3 &v){
	return vec3(m.m00 * v.x + m.m01 * v.y + m.m02 * v.z, m.m10 * v.x + m.m11 * v.y + m.m12 * v.z, m.m20 * v.x + m.m21 * v.y + m.m22 * v.z);
}

inline vec4 operator*(const mat4 &m, const vec4 &v){
	return vec4(m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03 * v.w, m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13 * v.w, m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23 * v.w, m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33 * v.w);
}

inline mat2 operator*(const mat2 &m, float s){
	return mat2(m.m00 * s, m.m01 * s,
				m.m10 * s, m.m11 * s);
}

inline mat3 operator*(const mat3 &m, float s){
	return mat3(m.m00 * s, m.m01 * s, m.m02 * s,
				m.m10 * s, m.m11 * s, m.m12 * s,
				m.m20 * s, m.m21 * s, m.m22 * s);
}

inline mat4 operator*(const mat4 &m, float s){
	return mat4(m.m00 * s, m.m01 * s, m.m02 * s, m.m03 * s,
				m.m10 * s, m.m11 * s, m.m12 * s, m.m13 * s,
				m.m20 * s, m.m21 * s, m.m22 * s, m.m23 * s,
				m.m30 * s, m.m31 * s, m.m32 * s, m.m33 * s);
}

inline mat2 operator*(float s, const mat2 &m){
	return mat2(s * m.m00, s * m.m01,
				s * m.m10, s * m.m11);
}

inline mat3 operator*(float s, const mat3 &m){
	return mat3(s * m.m00, s * m.m01, s * m.m02,
				s * m.m10, s * m.m11, s * m.m12,
				s * m.m20, s * m.m21, s * m.m22);
}

inline mat4 operator*(float s, const mat4 &m){
	return mat4(s * m.m00, s * m.m01, s * m.m02, s * m.m03,
				s * m.m10, s * m.m11, s * m.m12, s * m.m13,
				s * m.m20, s * m.m21, s * m.m22, s * m.m23,
				s * m.m30, s * m.m31, s * m.m32, s * m.m33);
}

} //namespace ml
#endif //ML_MAT_H