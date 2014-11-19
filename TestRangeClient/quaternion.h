#ifndef QUATERNION_H
#define QUATERNION_H

#include "vec.h"

#define PIf 3.1415926f
#define DEG_TO_RAD( a ) ((PIf / 180.f)*a)
#define RAD_TO_DEG( a ) ((180.0f/PIf)*a)


namespace ml{

struct mat3;

struct quaternion{
	union{
		struct{
			float x, y, z, w;
		};
		struct{
			vec3 v;
			float s;
		};
		struct{
			vec4 vector;
		};
		float elem[4];
	};

	quaternion();
	quaternion(float f);
	quaternion(const vec4 &v);
	quaternion(float x, float y, float z, float w);
	quaternion(const vec3 &v, float w);
	quaternion(const mat3 &rot);

	float& operator[](size_t index){
		//assert
		return elem[index];
	}
	const float& operator[](size_t index)const{
		//assert
		return elem[index];
	}

	static quaternion createRotation(float angle, const vec3 &rotationAxis);/*{
		quaternion res;
		res.v = rotationAxis * std::sin(angle/2.0f);
		res.s = std::cos(angle/2.0f);
		return res;	
	}*/

	static quaternion lerp(const quaternion &q1, const quaternion &q2, float alpha);/*{
		return normalize((1.0f-alpha)*q1+alpha*q2);
	}*/

	static quaternion slerp(const quaternion &q1, const quaternion &q2, float alpha);/*{
		float dot = glm::dot(q1.vector, q2.vector);

		static const double DOT_THRESHOLD = 0.995;
		if(dot > DOT_THRESHOLD){
			return lerp(q1, q2, alpha);
		}

		dot = std::min(1.0f, std::max(-1.0f, dot)); //clamp to valid range
		float theta = std::acos(dot);
		float sin = std::sin(theta);
		return  std::sin((1-alpha)*theta)/sin * q1 + std::sin(alpha*theta)/sin * q2;
	}*/
	static const quaternion One;
};


quaternion operator+(const quaternion &q1, const quaternion &q2);
quaternion operator-(const quaternion &q1, const quaternion &q2);
quaternion operator*(const quaternion &q1, const quaternion &q2);
quaternion operator/(const quaternion &q1, const quaternion &q2);
quaternion& operator+=(quaternion &q1, const quaternion &q2);
quaternion& operator-=(quaternion &q1, const quaternion &q2);
quaternion& operator*=(quaternion &q1, const quaternion &q2);
quaternion& operator/=(quaternion &q1, const quaternion &q2);



//------------------------quaternion/Scalar Operations ------------------------
quaternion operator+(const quaternion &q, float f);
quaternion operator-(const quaternion &q, float f);
quaternion operator*(const quaternion &q, float f);
quaternion operator/(const quaternion &q, float f);
quaternion operator+(float f, const quaternion &q);
quaternion operator-(float f, const quaternion &q);
quaternion operator*(float f, const quaternion &q);
quaternion operator/(float f, const quaternion &q);
quaternion& operator+=(quaternion &q, float f);
quaternion& operator-=(quaternion &q, float f);
quaternion& operator*=(quaternion &q, float f);
quaternion& operator/=(quaternion &q, float f);

std::ostream& operator<<(std::ostream &os, const quaternion &q);

//------------------------quaternion Functions------------------------

quaternion conjugate(const quaternion &q);
//rot has to be unit length
vec3 rotate(const vec3& v, quaternion rot);
//rot has to be unit length
vec3 rotate(const vec4& v, quaternion rot);
float squaredLength(const quaternion &q);

quaternion invert(const quaternion &q);

float length(const quaternion &q);

quaternion normalize(const quaternion &q);
} //namespace ml
#endif // ML_QUATERNION_H
