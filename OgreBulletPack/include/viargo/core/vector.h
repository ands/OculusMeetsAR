
#ifndef VIARGO_VECTOR_H
#define VIARGO_VECTOR_H

#include "core/mathfunctions.h"

namespace viargo{

//Vector2D
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
class Vector2D{
    
public:
    union{
        struct{
            T x, y;
        };
        T elem[2];
    };
    
    //constructor
	Vector2D() : x(0), y(0){
    }

    explicit Vector2D(T xx) : x(xx), y(xx){
    }

    explicit Vector2D(T xx, T yy) : x(xx), y(yy){
    }

	explicit Vector2D(const T* arr) : x(arr[0]), y(arr[1]){
    }

	// constants
	static const Vector2D<T> zero;
	static const Vector2D<T> unitX;
	static const Vector2D<T> unitY;

	void get(T* arr) const {
		arr[0] = x; arr[1] = y;
	}

    inline Vector2D<T>& operator = (const Vector2D<T>& op){
        x = op.x; y = op.y;
        return *this;
    }
    inline Vector2D<T>& operator += (const Vector2D<T>& op){
        x += op.x; y += op.y;
		return *this;
    }
    inline Vector2D<T>& operator -= (const Vector2D<T>& op){
        x -= op.x; y -= op.y;
		return *this;
    }
    inline Vector2D<T>& operator *= (T op){
        x *= op; y *= op;
		return *this;
    }
    inline Vector2D<T>& operator /= (T op){
        x /= op; y /= op;
		return *this;
    }

 };

// constants
template<typename T> const Vector2D<T> Vector2D<T>::zero  = Vector2D<T>((T)0, (T)0);
template<typename T> const Vector2D<T> Vector2D<T>::unitX = Vector2D<T>((T)1, (T)0);
template<typename T> const Vector2D<T> Vector2D<T>::unitY = Vector2D<T>((T)0, (T)1);

//operations
template <typename T>
inline Vector2D<T>  operator + (const Vector2D<T>& lop, const Vector2D<T>& rop){
    return Vector2D<T>(lop.x + rop.x, lop.y + rop.y);
}

template <typename T>
inline Vector2D<T>  operator - (const Vector2D<T>& lop, const Vector2D<T>& rop){
    return Vector2D<T>(lop.x - rop.x, lop.y - rop.y);
}

template <typename T>
inline Vector2D<T>  operator - (const Vector2D<T>& op){
    Vector2D<T> res(-op.x, -op.y);
    return res;
}

template <typename T>
inline Vector2D<T>  operator * (T lop, const Vector2D<T>& rop){
    return Vector2D<T>(lop * rop.x, lop * rop.y);
}

template <typename T>
inline Vector2D<T>  operator * (const Vector2D<T>& lop, T rop){
    return Vector2D<T>(lop.x * rop, lop.y * rop);
}

template <typename T>
inline Vector2D<T>  operator / (T lop, const Vector2D<T>& rop){
    return Vector2D<T>(lop / rop.x, lop / rop.y);
}

template <typename T>
inline Vector2D<T>  operator / (const Vector2D<T>& lop, T rop){
    return Vector2D<T>(lop.x / rop, lop.y / rop);
}

template <typename T>
inline bool  operator == (const Vector2D<T>& lop, const Vector2D<T>& rop){
    return (lop.x == rop.x) && (lop.y == rop.y);
}

template <typename T>
inline bool  operator != (const Vector2D<T>& lop, const Vector2D<T>& rop){
    return !(lop == rop);
}

template <typename T>
inline bool  operator < (const Vector2D<T>& lop, const Vector2D<T>& rop){
    return (lop.x < rop.x ? true : (lop.x == rop.x ? (lop.y < rop.y) : false));
}

template <typename T>
inline bool  operator <= (const Vector2D<T>& lop, const Vector2D<T>& rop){
    return (lop < rop) || (lop == rop);
}

template <typename T>
inline bool  operator > (const Vector2D<T>& lop, const Vector2D<T>& rop){
    return !(lop <= rop);
}

template <typename T>
inline bool  operator >= (const Vector2D<T>& lop, const Vector2D<T>& rop){
    return !(lop < rop);
}

//functions
template <typename T>
inline T dot(const Vector2D<T>& lop, const Vector2D<T>& rop){
    return (lop.x * rop.x + lop.y * rop.y);
}

template <typename T>
inline T squaredLength(const Vector2D<T>& lop){
    return (dot(lop, lop));
}

template <typename T>
inline T length(const Vector2D<T>& lop){
    return Math::sqrt(squaredLength(lop));
}

template <typename T>
inline T squaredDistance(const Vector2D<T>& lop, const Vector2D<T>& rop){
    return (squaredLength(lop - rop));
}

template <typename T>
inline T distance(const Vector2D<T>& lop, const Vector2D<T>& rop){
    return (length(lop - rop));
}

template <typename T>
inline Vector2D<T> normalize(const Vector2D<T>& lop){
    T lght = length(lop);
    return (lght != 0 ? lop / lght : Vector2D<T>());
}

//Vector3D
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
class Vector3D{
    
public:
    union{
        struct{
            T x, y, z;
        };
        T elem[3];
    };
    
    //constructor
	Vector3D() : x(0), y(0), z(0){
    }

	explicit Vector3D(T xx, Math::Handedness csh = Math::CSH_RIGHT) 
		: x(xx), y(xx), z(xx){
			z = (csh == Math::CSH_RIGHT ? z : -z);
    }

	explicit Vector3D(T xx, T yy, T zz, Math::Handedness csh = Math::CSH_RIGHT) 
		: x(xx), y(yy), z(zz){
			z = (csh == Math::CSH_RIGHT ? z : -z);
    }

	explicit Vector3D(const T* arr, Math::Handedness csh = Math::CSH_RIGHT) 
		: x(arr[0]), y(arr[1]), z(arr[2]){
			z = (csh == Math::CSH_RIGHT ? z : -z);
    }

	// constants
	static const Vector3D<T> zero;
	static const Vector3D<T> unitX;
	static const Vector3D<T> unitY;
	static const Vector3D<T> unitZ;

	void get(T* arr, Math::Handedness csh = Math::CSH_RIGHT) const {
		arr[0] = x;
		arr[1] = y;
		arr[2] = (csh == Math::CSH_RIGHT ? z : -z);
	}

    inline Vector3D<T>& operator =  (const Vector3D<T>& op){
        x = op.x; y = op.y; z = op.z;
        return *this;
    }

    inline Vector3D<T>& operator += (const Vector3D<T>& op){
        x += op.x; y += op.y; z += op.z;
		return *this;
    }

    inline Vector3D<T>& operator -= (const Vector3D<T>& op){
        x -= op.x; y -= op.y; z-= op.z;
		return *this;
    }

    inline Vector3D<T>& operator *= (T op){
        x *= op; y *= op; z *= op;
		return *this;
    }

    inline Vector3D<T>& operator /= (T op){
        x /= op; y /= op; z /= op;
		return *this;
    }
 };


// constants
template<typename T> const Vector3D<T> Vector3D<T>::zero  = Vector3D<T>((T)0, (T)0, (T)0);
template<typename T> const Vector3D<T> Vector3D<T>::unitX = Vector3D<T>((T)1, (T)0, (T)0);
template<typename T> const Vector3D<T> Vector3D<T>::unitY = Vector3D<T>((T)0, (T)1, (T)0);
template<typename T> const Vector3D<T> Vector3D<T>::unitZ = Vector3D<T>((T)0, (T)0, (T)1);

//operations
template <typename T>
inline Vector3D<T>  operator + (const Vector3D<T>& lop, const Vector3D<T>& rop){
    return Vector3D<T>(lop.x + rop.x, lop.y + rop.y, lop.z + rop.z);
}

template <typename T>
inline Vector3D<T>  operator - (const Vector3D<T>& lop, const Vector3D<T>& rop){
    return Vector3D<T>(lop.x - rop.x, lop.y - rop.y, lop.z - rop.z);
}

template <typename T>
inline Vector3D<T>  operator - (const Vector3D<T>& op){
    Vector3D<T> res(-op.x, -op.y, -op.z);
    return res;
}

template <typename T>
inline Vector3D<T>  operator * (T lop, const Vector3D<T>& rop){
    return Vector3D<T>(lop * rop.x, lop * rop.y, lop * rop.z);
}

template <typename T>
inline Vector3D<T>  operator * (const Vector3D<T>& lop, T rop){
    return Vector3D<T>(lop.x * rop, lop.y * rop, lop.z * rop);
}

template <typename T>
inline Vector3D<T>  operator / (T lop, const Vector3D<T>& rop){
    return Vector3D<T>(lop / rop.x, lop / rop.y, lop / rop.z);
}

template <typename T>
inline Vector3D<T>  operator / (const Vector3D<T>& lop, T rop){
    return Vector3D<T>(lop.x / rop, lop.y / rop, lop.z / rop);
}

template <typename T>
inline bool  operator == (const Vector3D<T>& lop, const Vector3D<T>& rop){
    return (lop.x == rop.x) && (lop.y == rop.y) && (lop.z == rop.z);
}

template <typename T>
inline bool  operator != (const Vector3D<T>& lop, const Vector3D<T>& rop){
    return !(lop == rop);
}

template <typename T>
inline bool  operator < (const Vector3D<T>& lop, const Vector3D<T>& rop){
    return  (lop.x < rop.x ? true : 
            (lop.x > rop.x ? false :
            (lop.y < rop.y ? true :
            (lop.y > rop.y ? false :
            (lop.z < rop.z)))));
}

template <typename T>
inline bool  operator <= (const Vector3D<T>& lop, const Vector3D<T>& rop){
    return (lop < rop) || (lop == rop);
}

template <typename T>
inline bool  operator > (const Vector3D<T>& lop, const Vector3D<T>& rop){
    return !(lop <= rop);
}

template <typename T>
inline bool  operator >= (const Vector3D<T>& lop, const Vector3D<T>& rop){
    return !(lop < rop);
}

//functions
template <typename T>
inline T dot(const Vector3D<T>& lop, const Vector3D<T>& rop){
    return (lop.x * rop.x + lop.y * rop.y + lop.z * rop.z);
}

template <typename T>
inline Vector3D<T> cross(const Vector3D<T>& lop, const Vector3D<T>& rop){
    return Vector3D<T>(lop.y * rop.z - lop.z * rop.y,
                       lop.z * rop.x - lop.x * rop.z,
                       lop.x * rop.y - lop.y * rop.x);
}

template <typename T>
inline T squaredLength(const Vector3D<T>& lop){
    return (dot(lop, lop));
}

template <typename T>
inline T length(const Vector3D<T>& lop){
    return Math::sqrt(squaredLength(lop));
}

template <typename T>
inline T squaredDistance(const Vector3D<T>& lop, const Vector3D<T>& rop){
    return (squaredLength(lop - rop));
}

template <typename T>
inline T distance(const Vector3D<T>& lop, const Vector3D<T>& rop){
    return (length(lop - rop));
}

template <typename T>
inline Vector3D<T> normalize(const Vector3D<T>& lop){
    T lght = length(lop);
    return (lght != 0 ? lop / lght : Vector3D<T>());
}


} // namespace viargo

#endif // VIARGO_VECTOR_H


