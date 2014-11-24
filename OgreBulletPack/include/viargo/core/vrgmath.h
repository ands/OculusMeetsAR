
#ifndef VIARGO_VRGMATH_H
#define VIARGO_VRGMATH_H

#include <iostream>

#include "core/matrix.h"
#include "core/quaternion.h"

#include "core/ray.h"

namespace viargo{

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

typedef Vector2D<int>       vec2i;
typedef Vector2D<float>     vec2f;
typedef Vector2D<double>    vec2d;

typedef Vector3D<int>       vec3i;
typedef Vector3D<float>     vec3f;
typedef Vector3D<double>    vec3d;

typedef Matrix3<float>      mat3f;
typedef Matrix3<double>     mat3d;

typedef Matrix4<float>      mat4f;
typedef Matrix4<double>     mat4d;

typedef Quaternion<float>   quatf;
typedef Quaternion<double>  quatd;

typedef Ray3D<float>  ray3f;
typedef Ray3D<double> ray3d;

//typedef Box<float>  box3f;
//typedef Box<double> box3d;

typedef vec2f vec2;
typedef vec3f vec3;
typedef mat3f mat3;
typedef mat4f mat4;
typedef quatf quat;
typedef ray3f ray3;
//typedef box3f box3;

//some output operations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <typename T>
std::ostream& operator << (std::ostream& os, Vector2D<T> const & vec){
    os << "(" << vec.x << ", "<< vec.y << ")";
    return os;
}

template <typename T>
std::ostream& operator << (std::ostream& os, Vector3D<T> const & vec){
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

template <typename T>
std::ostream& operator << (std::ostream& os, Quaternion<T> const & q){
    os << "[" << q.w << ", (" << q.x << ", " << q.y << ", " << q.z << ")]";
    return os;
}

} // namespace viargo

#endif // VIARGO_VRGMATH_H

