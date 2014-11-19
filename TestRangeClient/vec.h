#ifndef ML_VEC_H
#define ML_VEC_H

#include <iostream>
#include <cassert>
#include <sstream>

#ifdef __SSE2__
#include <emmintrin.h>
#endif

#ifdef __SSE3__
#include <pmmintrin.h>
#endif

#ifdef __SSE4_1__
#include <smmintrin.h>
#endif

#ifdef IMPLEMENT_SSE
#define SSE_VEC4
#endif

#ifdef SSE_VEC4
#define VEC4_ALIGNMENT __declspec(align(16))
#else
#define VEC4_ALIGNMENT
#endif


namespace ml{

struct vec2;
struct vec3;
struct vec4;

struct vec2{
    enum {size = 2};
    union{
        struct{
            float x, y;
        };
        struct{
            float r, g;
        };
		struct{
			float s, t;
		};
        float elem[size];
    };

    vec2(){}
    explicit vec2(const float &x_, const float& y_) : x(x_), y(y_) {}
    explicit vec2(const float &value) : x(value), y(value) {}

    float& operator[](size_t pos){
		assert(pos >= 0 && pos < size);
        return elem[pos];
    }

    const float& operator[](size_t pos) const{
		assert(pos >= 0 && pos < size);
        return elem[pos];
    }
	
	static const vec2 Zero;
	static const vec2 One;
};

struct vec3{
    enum {size = 3};
    union{
        struct{
            float x, y, z;
        };
        struct{
            float r, g, b;
        };
		struct{
			float s, t, p;
		};
        float elem[size];
    };

    vec3(){}
    explicit vec3(const float& x_, const float& y_, const float& z_) : x(x_), y(y_), z(z_) {}
    explicit vec3(const float& value) : x(value), y(value), z(value) {}
	explicit vec3(const vec4& v);

    float& operator[](size_t pos){
		assert(pos >= 0 && pos < size);
        return elem[pos];
    }

    const float& operator[](size_t pos) const{
		assert(pos >= 0 && pos < size);
        return elem[pos];
    }
	
	static const vec3 Zero;
	static const vec3 One;
};

struct VEC4_ALIGNMENT vec4{
    enum {size=4};
    union{
        struct{
            float x, y, z, w;
        };
        struct{
            float r, g, b, a;
        };
		struct{
			float s, t, p, q;
		};
#ifdef SSE_VEC4
		__m128 reg;
#endif //SSE_VEC4
        float elem[size];
    };

	//Constructors
    vec4(){}
    explicit vec4(const float& x_, const float& y_, const float& z_, const float& w_) : x(x_), y(y_), z(z_), w(w_) {}
    explicit vec4(const float& value) : x(value), y(value), z(value), w(value) {}
	explicit vec4(const vec3& v, float _w);

#ifdef SSE_VEC4
	explicit vec4(const __m128& r) : reg(r) {}
#endif //SSE_VEC4

    float& operator[](size_t pos){
		assert(pos >= 0 && pos < size);
        return elem[pos];
    }

    const float& operator[](size_t pos) const{
		assert(pos >= 0 && pos < size);
        return elem[pos];
    }

	static const vec4 Zero;
	static const vec4 One;
};

inline vec2 operator+(const vec2& v1, const vec2& v2){
	return vec2(v1.x + v2.x, v1.y + v2.y);
}

inline vec3 operator+(const vec3& v1, const vec3& v2){
	return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline vec4 operator+(const vec4& v1, const vec4& v2){
#ifdef SSE_VEC4
	return vec4(_mm_add_ps(v1.reg,v2.reg));
#else
	return vec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
#endif
}

inline vec2 operator-(const vec2& v1, const vec2& v2){
	return vec2(v1.x - v2.x, v1.y - v2.y);
}

inline vec3 operator-(const vec3& v1, const vec3& v2){
	return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline vec4 operator-(const vec4& v1, const vec4& v2){
#ifdef SSE_VEC4
	return vec4(_mm_sub_ps(v1.reg,v2.reg));
#else
	return vec4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
#endif
}

inline vec2 operator*(const vec2& v1, const vec2& v2){
	return vec2(v1.x * v2.x, v1.y * v2.y);
}

inline vec3 operator*(const vec3& v1, const vec3& v2){
	return vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

inline vec4 operator*(const vec4& v1, const vec4& v2){
#ifdef SSE_VEC4
	return vec4(_mm_mul_ps(v1.reg,v2.reg));
#else
	return vec4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
#endif
}

inline vec2 operator/(const vec2& v1, const vec2& v2){
	assert(v2.x != 0.0f && v2.y != 0.0f);
	return vec2(v1.x * v2.x, v1.y * v2.y);
}

inline vec3 operator/(const vec3& v1, const vec3& v2){
	assert(v2.x != 0.0f && v2.y != 0.0f && v2.z != 0.0f);
	return vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

inline vec4 operator/(const vec4& v1, const vec4& v2){
	assert(v2.x != 0.0f && v2.y != 0.0f && v2.z != 0.0f && v2.w != 0.0f);
#ifdef SSE_VEC4
	return vec4(_mm_div_ps(v1.reg,v2.reg));
#else
	return vec4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
#endif
}

inline vec2 operator+=(vec2& v1, const vec2& v2){
	v1.x += v2.x;
	v1.y += v2.y;
	return v1;
}

inline vec3 operator+=(vec3& v1, const vec3& v2){
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}

inline vec4 operator+=(vec4& v1, const vec4& v2){
#ifdef SSE_VEC4
	v1.reg = _mm_add_ps(v1.reg, v2.reg);
#else
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	v1.w += v2.w;
#endif
	return v1;
}

inline vec2 operator-=(vec2& v1, const vec2& v2){
	v1.x -= v2.x;
	v1.y -= v2.y;
	return v1;
}

inline vec3 operator-=(vec3& v1, const vec3& v2){
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	return v1;
}

inline vec4 operator-=(vec4& v1, const vec4& v2){
#ifdef SSE_VEC4
	v1.reg = _mm_sub_ps(v1.reg, v2.reg);
#else
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	v1.w -= v2.w;
#endif
	return v1;
}

inline vec2 operator*=(vec2& v1, const vec2& v2){
	v1.x *= v2.x;
	v1.y *= v2.y;
	return v1;
}

inline vec3 operator*=(vec3& v1, const vec3& v2){
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
	return v1;
}

inline vec4 operator*=(vec4& v1, const vec4& v2){
#ifdef SSE_VEC4
	v1.reg = _mm_mul_ps(v1.reg, v2.reg);
#else
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
	v1.w *= v2.w;
#endif
	return v1;
}

inline vec2 operator/=(vec2& v1, const vec2& v2){
	assert(v2.x != 0.0f && v2.y != 0.0f);
	v1.x /= v2.x;
	v1.y /= v2.y;
	return v1;
}

inline vec3 operator/=(vec3& v1, const vec3& v2){
	assert(v2.x != 0.0f && v2.y != 0.0f && v2.z != 0.0f);
	v1.x /= v2.x;
	v1.y /= v2.y;
	v1.z /= v2.z;
	return v1;
}

inline vec4 operator/=(vec4& v1, const vec4& v2){
	assert(v2.x != 0.0f && v2.y != 0.0f && v2.z != 0.0f && v2.w != 0.0f);
#ifdef SSE_VEC4
	v1.reg = _mm_div_ps(v1.reg, v2.reg);
#else
	v1.x /= v2.x;
	v1.y /= v2.y;
	v1.z /= v2.z;
	v1.w /= v2.w;
#endif
	return v1;
}

inline bool operator==(const vec2& v1, const vec2& v2){
	return (v1.x == v2.x) && (v1.y == v2.y);
}

inline bool operator==(const vec3& v1, const vec3& v2){
	return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
}

inline bool operator==(const vec4& v1, const vec4& v2){
	return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w);
}

inline bool operator!=(const vec2& v1, const vec2& v2){
	return (v1.x != v2.x) || (v1.y != v2.y);
}

inline bool operator!=(const vec3& v1, const vec3& v2){
	return (v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z);
}

inline bool operator!=(const vec4& v1, const vec4& v2){
	return (v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z) || (v1.w != v2.w);
}

//----------------------------------------Componentwise vector operations with scalars--------------------------

inline vec2 operator+(const vec2& v, const float& val){
	return vec2(v.x + val, v.y + val);
}

inline vec3 operator+(const vec3& v, const float& val){
	return vec3(v.x + val, v.y + val, v.z + val);
}

inline vec4 operator+(const vec4& v, const float& val){
#ifdef SSE_VEC4
	return vec4(_mm_add_ps(v.reg, _mm_set_ps1(val)));
#else
	return vec4(v.x + val, v.y + val, v.z + val, v.w + val);
#endif
}

inline vec2 operator+(const float& val, const vec2& v){
	return vec2(val + v.x, val + v.y);
}

inline vec3 operator+(const float& val, const vec3& v){
	return vec3(val + v.x, val + v.y, val + v.z);
}

inline vec4 operator+(const float& val, const vec4& v){
#ifdef SSE_VEC4
	return vec4(_mm_add_ps(_mm_set_ps1(val), v.reg));
#else
	return vec4(val + v.x, val + v.y, val + v.z, val + v.w);
#endif
}

inline vec2 operator-(const vec2& v, const float& val){
	return vec2(v.x - val, v.y - val);
}

inline vec3 operator-(const vec3& v, const float& val){
	return vec3(v.x - val, v.y - val, v.z - val);
}

inline vec4 operator-(const vec4& v, const float& val){
#ifdef SSE_VEC4
	return vec4(_mm_sub_ps(v.reg, _mm_set_ps1(val)));
#else
	return vec4(v.x - val, v.y - val, v.z - val, v.w - val);
#endif
}

inline vec2 operator-(const float& val, const vec2& v){
	return vec2(val - v.x, val - v.y);
}

inline vec3 operator-(const float& val, const vec3& v){
	return vec3(val - v.x, val - v.y, val - v.z);
}

inline vec4 operator-(const float& val, const vec4& v){
#ifdef SSE_VEC4
	return vec4(_mm_sub_ps(_mm_set_ps1(val), v.reg));
#else
	return vec4(val - v.x, val - v.y, val - v.z, val - v.w);
#endif
}

inline vec2 operator*(const vec2& v, const float& val){
	return vec2(v.x * val, v.y * val);
}

inline vec3 operator*(const vec3& v, const float& val){
	return vec3(v.x * val, v.y * val, v.z * val);
}

inline vec4 operator*(const vec4& v, const float& val){
#ifdef SSE_VEC4
	return vec4(_mm_mul_ps(v.reg, _mm_set_ps1(val)));
#else
	return vec4(v.x * val, v.y * val, v.z * val, v.w * val);
#endif
}

inline vec2 operator*(const float& val, const vec2& v){
	return vec2(val * v.x, val * v.y);
}

inline vec3 operator*(const float& val, const vec3& v){
	return vec3(val * v.x, val * v.y, val * v.z);
}

inline vec4 operator*(const float& val, const vec4& v){
#ifdef SSE_VEC4
	return vec4(_mm_mul_ps(_mm_set_ps1(val), v.reg));
#else
	return vec4(val * v.x, val * v.y, val * v.z, val * v.w);
#endif
}

inline vec2 operator/(const vec2& v, const float& val){
	assert(val != 0.0f);
	return vec2(v.x / val, v.y / val);
}

inline vec3 operator/(const vec3& v, const float& val){
	assert(val != 0.0f);
	return vec3(v.x / val, v.y / val, v.z / val);
}

inline vec4 operator/(const vec4& v, const float& val){
	assert(val != 0.0f);
#ifdef SSE_VEC4
	return vec4(_mm_div_ps(v.reg, _mm_set_ps1(val)));
#else
	return vec4(v.x / val, v.y / val, v.z / val, v.w / val);
#endif
}

inline vec2 operator/(const float& val, const vec2& v){
	assert(v.x != 0.0f && v.y != 0.0f);
	return vec2(val / v.x, val / v.y);
}

inline vec3 operator/(const float& val, const vec3& v){
	assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f);
	return vec3(val / v.x, val / v.y, val / v.z);
}

inline vec4 operator/(const float& val, const vec4& v){
	assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f && v.w != 0.0f);
#ifdef SSE_VEC4
	return vec4(_mm_div_ps(_mm_set_ps1(val), v.reg));
#else
	return vec4(val / v.x, val / v.y, val / v.z, val / v.w);
#endif
}

//-----------------------------------------------------------------------------------------------------------

inline vec2 operator+=(vec2& v, const float& val){
	v.x += val;
	v.y += val;
	return v;
}

inline vec3 operator+=(vec3& v, const float& val){
	v.x += val;
	v.y += val;
	v.z += val;
	return v;
}

inline vec4 operator+=(vec4& v, const float& val){
#ifdef SSE_VEC4
	v.reg = _mm_add_ps(v.reg, _mm_set_ps1(val));
#else
	v.x += val;
	v.y += val;
	v.z += val;
	v.w += val;
#endif
	return v;
}

inline vec2 operator-=(vec2& v, const float& val){
	v.x -= val;
	v.y -= val;
	return v;
}

inline vec3 operator-=(vec3& v, const float& val){
	v.x -= val;
	v.y -= val;
	v.z -= val;
	return v;
}

inline vec4 operator-=(vec4& v, const float& val){
#ifdef SSE_VEC4
	v.reg = _mm_sub_ps(v.reg, _mm_set_ps1(val));
#else
	v.x -= val;
	v.y -= val;
	v.z -= val;
	v.w -= val;
#endif
	return v;
}

inline vec2 operator*=(vec2& v, const float& val){
	v.x *= val;
	v.y *= val;
	return v;
}

inline vec3 operator*=(vec3& v, const float& val){
	v.x *= val;
	v.y *= val;
	v.z *= val;
	return v;
}

inline vec4 operator*=(vec4& v, const float& val){
#ifdef SSE_VEC4
	v.reg = _mm_mul_ps(v.reg, _mm_set_ps1(val));
#else
	v.x *= val;
	v.y *= val;
	v.z *= val;
	v.w *= val;
#endif
	return v;
}

inline vec2 operator/=(vec2& v, const float& val){
	assert(val != 0.0f);
	v.x /= val;
	v.y /= val;
	return v;
}

inline vec3 operator/=(vec3& v, const float& val){
	assert(val != 0.0f);
	v.x /= val;
	v.y /= val;
	v.z /= val;
	return v;
}

inline vec4 operator/=(vec4& v, const float& val){
	assert(val != 0.0f);
#ifdef SSE_VEC4
	v.reg = _mm_div_ps(v.reg, _mm_set_ps1(val));
#else
	v.x /= val;
	v.y /= val;
	v.z /= val;
	v.w /= val;
#endif
	return v;
}

//some other functions
inline float sum(const vec2 & v);
inline float sum(const vec3 & v);
inline float sum(const vec4 & v);
float dot(const vec2& v1, const vec2& v2);
float dot(const vec3& v1, const vec3& v2);
float dot(const vec4& v1, const vec4& v2);
float squaredLength(const vec2& v);
float squaredLength(const vec3& v);
float squaredLength(const vec4& v);
float length(const vec2& v);
float length(const vec3& v);
float length(const vec4& v);
vec2 lerp(const vec2& v1, const vec2& v2, float alpha);
vec3 lerp(const vec3& v1, const vec3& v2, float alpha);
vec4 lerp(const vec4& v1, const vec4& v2, float alpha);
vec2 normalize(const vec2& v);
vec3 normalize(const vec3& v);
vec4 normalize(const vec4& v);
vec3 cross(const vec3& v1, const vec3& v2);
std::ostream& operator<<(std::ostream& os, const vec2& v);
std::ostream& operator<<(std::ostream& os, const vec3& v);
std::ostream& operator<<(std::ostream& os, const vec4& v);

} //namespace ml
#endif // ML_VEC_H
