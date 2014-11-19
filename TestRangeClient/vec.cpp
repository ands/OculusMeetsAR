#include "vec.h"

namespace ml{
const vec2 vec2::Zero = vec2(0.0f);

const vec2 vec2::One = vec2(1.0f);

const vec3 vec3::Zero = vec3(0.0f);

const vec3 vec3::One = vec3(1.0f);

const vec4 vec4::Zero = vec4(0.0f);

const vec4 vec4::One = vec4(1.0f);

vec3::vec3(const vec4& v) : x(v.x), y(v.y), z(v.z){}
vec4::vec4(const vec3& v, float _w): x(v.x), y(v.y), z(v.z), w(_w) {}

float sum(const vec2 &v){
	return v.x + v.y;
}
float sum(const vec3 &v){
	return v.x + v.y + v.z;
}
float sum(const vec4 &v){
	return v.x + v.y + v.z + v.w;
}

float dot(const vec2 &v1, const vec2 &v2){
	return v1.x * v2.x + v1.y * v2.y;
}

float dot(const vec3 &v1, const vec3 &v2){
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float dot(const vec4 &v1, const vec4 &v2){
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

float squaredLength(const vec2& v){
	return v.x * v.x + v.y * v.y;
}
float squaredLength(const vec3& v){
	return v.x * v.x + v.y * v.y + v.z * v.z;
}
float squaredLength(const vec4& v){
	return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}
float length(const vec2& v){
	return std::sqrt(squaredLength(v));
}
float length(const vec3& v){
	return std::sqrt(squaredLength(v));
}
float length(const vec4& v){
	return std::sqrt(squaredLength(v));
}
vec2 lerp(const vec2& v1, const vec2& v2, float alpha){
	return (1.0f - alpha) * v1 + alpha * v2;
}
vec3 lerp(const vec3& v1, const vec3& v2, float alpha){
	return (1.0f - alpha) * v1 + alpha * v2;
}
vec4 lerp(const vec4& v1, const vec4& v2, float alpha){
	return (1.0f - alpha) * v1 + alpha * v2;
}
vec2 normalize(const vec2& v){
	return v/length(v);
}
vec3 normalize(const vec3& v){
	return v/length(v);
}
vec4 normalize(const vec4& v){
	return v/length(v);
}

vec3 cross(const vec3& v1, const vec3& v2){
    vec3 res;
    res.x = v1.y * v2.z - v1.z * v2.y;
    res.y = v1.z * v2.x - v1.x * v2.z;
    res.z = v1.x * v2.y - v1.y * v2.x;
    return res;
}

std::ostream& operator<<(std::ostream& os, const vec2& v){
    os<<"["<<v.x<<" "<<v.y<<"]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const vec3& v){
    os<<"["<<v.x<<" "<<v.y<<" "<<v.z<<"]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const vec4& v){
    os<<"["<<v.x<<" "<<v.y<<" "<<v.z<<" "<<v.w<<"]";
    return os;
}

} //namespace ml