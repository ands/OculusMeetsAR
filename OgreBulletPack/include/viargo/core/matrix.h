
#ifndef VIARGO_MATRIX_H
#define VIARGO_MATRIX_H

#include "core/vector.h"

namespace viargo{

//Matrix3
// TODO: the support for Matrix3 will be dropped in future releases

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//forward declarations
template <typename T> class Matrix4;
template <typename T> class Quaternion;

template <typename T>
class Matrix3{

public:
    union{
//        struct{
//            Vector3D<T> xvec;
//            Vector3D<T> yvec;
//            Vector3D<T> zvec;
//        };
        T elem[9];
		T elem3x3[3][3];
    };

	Matrix3() {//: xvec(), yvec(), zvec(){
	    for(int i = 0; i < 9; ++i)
            elem[i] = 0;
    }

	Matrix3(T value) {//: xvec(value), yvec(value), zvec(value){
        for(int i = 0; i < 9; ++i)
            elem[i] = value;
    }

    Matrix3(const T* values){
        for(int i = 0; i < 9; ++i)
            elem[i] = values[i];
    }

	Matrix3(T a11, T a12, T a13, T a21, T a22, T a23, T a31, T a32, T a33){

			elem[0]  = a11; elem[1]  = a12; elem[2]  = a13;
			elem[3]  = a21; elem[4]  = a22; elem[5]  = a23;
			elem[6]  = a31; elem[7]  = a32; elem[8]  = a33;
    }

    inline T operator()(int row, int col) const {
        return elem[3 * row + col];
    }

    inline T operator()(int idx) const {
        return elem[idx];
    }

    inline Matrix3<T>& operator = (const Matrix3<T>& op){
        for(int i = 0; i < 9; ++i)
            elem[i] = op.elem[i];
        return *this;
    }

    inline Matrix4<T> matrix4() const{
        T values[16] = {elem[0], elem[1], elem[2], 0,
                        elem[3], elem[4], elem[5], 0,
                        elem[6], elem[7], elem[8], 0,
                              0,       0,       0, 1};
        return Matrix4<T>(values);
    }
};

//operations
template <typename T>
inline bool operator == (const Matrix3<T>& lop, const Matrix3<T>& rop){
	for(int i = 0; i < 9; ++i)
		if(lop.elem[i] != rop.elem[i])
			return false;
	return true;
}

template <typename T>
inline bool operator != (const Matrix3<T>& lop, const Matrix3<T>& rop){
	return !(lop == rop);
}

template <typename T>
inline Matrix3<T> operator + (const Matrix3<T>& lop, const Matrix3<T>& rop){
    T elem[9];
    for(int i = 0; i < 9; ++i)
        elem[i] = lop(i) + rop(i);

    return Matrix3<T>(elem);
}

template <typename T>
inline Matrix3<T> operator - (const Matrix3<T>& lop, const Matrix3<T>& rop){
    T elem[9];
    for(int i = 0; i < 9; ++i)
        elem[i] = lop(i) - rop(i);

    return Matrix3<T>(elem);
}

template <typename T>
inline Matrix3<T> operator - (const Matrix3<T>& op){
    T elem[9];
    for(int i = 0; i < 9; ++i)
        elem[i] = -op(i);

    return Matrix3<T>(elem);
}

template <typename T> //TODO:dimi: there is a better way to do this
inline Matrix3<T> operator * (const Matrix3<T>& lop, const Matrix3<T>& rop){
    T elem[3][3];
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            for(int k = 0, elem[i][j] = 0; k < 3; k++)
                elem[i][j] += lop(i, k) * rop(k, j);

    return Matrix3<T>(elem);
}

template <typename T>
inline Vector3D<T> operator * (const Matrix3<T>& lop, const Vector3D<T>& rop){

    Vector3D<T> v;

    v.x = rop.x * lop(0, 0) +
          rop.y * lop(0, 1) +
          rop.z * lop(0, 2);

    v.y = rop.x * lop(1, 0) +
          rop.y * lop(1, 1) +
          rop.z * lop(1, 2);

    v.z = rop.x * lop(2, 0) +
          rop.y * lop(2, 1) +
          rop.z * lop(2, 2);

    return v;
}

//functions
template <typename T>
inline Matrix3<T> transpose(const Matrix3<T>& rop){
    T values[9] = {rop.elem[0], rop.elem[3], rop.elem[6],
                   rop.elem[1], rop.elem[4], rop.elem[7],
                   rop.elem[2], rop.elem[5], rop.elem[8]};

    return Matrix3<T>(values);
}



//Matrix4
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/**
* Basic generic matrix class. All matrix are internaly ROW-COL and
* right-handed. For conversions @see get(...) method and ctors.
*
*/
template <typename T>
class Matrix4{

public:
	union {
		struct {T _11, _12, _13, _14,
				  _21, _22, _23, _24,
				  _31, _32, _33, _34,
				  _41, _42, _43, _44;
		};
		struct {T m11, m12, m13, m14,
				  m21, m22, m23, m24,
				  m31, m32, m33, m34,
				  m41, m42, m43, m44;
		};
		T elem[16];
		T elem4x4[4][4];
	};

	static const Matrix4<T> zero;
	static const Matrix4<T> unit;
	static const Matrix4<T> identity;
	static const Matrix4<T> zReflection;

	Matrix4(){
        *this = identity;
    }

	Matrix4(T value, Math::Handedness csh = Math::CSH_RIGHT){
        for(int i = 0; i < 16; ++i)
            elem[i] = value;
		*this = convertMatrix(csh, Math::MO_ROWCOL);
    }

    Matrix4(const T* values, Math::Handedness csh = Math::CSH_RIGHT, Math::MatrixOrder mo = Math::MO_ROWCOL){
        for(int i = 0; i < 16; ++i)
            elem[i] = values[i];
		*this = convertMatrix(csh, mo);
    }

	Matrix4(T a11, T a12, T a13, T a14, T a21, T a22, T a23, T a24,
			T a31, T a32, T a33, T a34, T a41, T a42, T a43, T a44,
			Math::Handedness csh = Math::CSH_RIGHT, Math::MatrixOrder mo = Math::MO_ROWCOL){

		elem[0]  = a11; elem[1]  = a12; elem[2]  = a13; elem[3]  = a14;
		elem[4]  = a21; elem[5]  = a22; elem[6]  = a23; elem[7]  = a24;
		elem[8]  = a31; elem[9]  = a32; elem[10] = a33; elem[11] = a34;
		elem[12] = a41; elem[13] = a42; elem[14] = a43; elem[15] = a44;

		*this = convertMatrix(csh, mo);
    }

	void get(T* array, Math::Handedness csh = Math::CSH_RIGHT, Math::MatrixOrder mo = Math::MO_ROWCOL) const {
		Matrix4<T> result = convertMatrix(csh, mo);
		for (int i = 0; i < 16; ++i)
			array[i] = result(i);
	}

    inline T operator()(int row, int col) const {
        return elem4x4[row][col];
    }

    inline T operator()(int idx) const {
        return elem[idx];
    }

    inline Matrix4<T>& operator = (const Matrix4<T>& op) {
        for(int i = 0; i < 16; ++i)
            elem[i] = op.elem[i];
        return *this;
    }

	inline bool isZero() const {
		return (*this == Matrix4<T>::zero);
	}

	inline bool isIdentity() const {
		return (*this == Matrix4<T>::identity);
	}

	// matrix creation
	static Matrix4<T> makeTranslation(T x, T y, T z) {
		return Matrix4<T>(	0, 0, 0, x,
							0, 0, 0, y,
							0, 0, 0, z,
							0, 0, 0, 1);
	}

	static Matrix4<T> makeTranslation(Vector3D<T> trans) {
		return Matrix4<T> ( 1,  0,  0, trans.x,
							0,  1,  0, trans.y,
							0,  0,  1, trans.z,
							0,  0,  0, 1);
	}

	static Matrix4<T> makeRotation(Vector3D<T> axis, T angle) {
			T c = Math::cos(angle);
			T s = Math::sin(angle);
			T t = 1 - c;
			Vector3D<T> ax = normalize(axis);

			return Matrix4<T>(	t*ax.x*ax.x + c,		t*ax.x*ax.y + s*ax.z,	t*ax.x*ax.z - s*ax.y,	0,
								t*ax.x*ax.y - s*ax.z,	t*ax.y*ax.y + c,		t*ax.y*ax.z + s*ax.x,	0,
								t*ax.x*ax.y + s*ax.y,	t*ax.y*ax.z - s*ax.x,	t*ax.z*ax.z + c,		0,
								0,						0,						0,						1);
	}

	static Matrix4<T> makeRotationX(T pitch) {
		T c = Math::cos(pitch);
		T s = Math::sin(pitch);
		return Matrix4<T> ( 1,  0,  0, 0,
							0,  c, -s, 0,
							0,  s,  c, 0,
							0,  0,  0, 1);
	}

	static Matrix4<T> makeRotationY(T yaw) {
		T c = Math::cos(yaw);
		T s = Math::sin(yaw);
		return Matrix4<T> ( c, 0,  s, 0,
							0, 1,  0, 0,
						   -s, 0,  c, 0,
							0, 0,  0, 1);
	}

	static Matrix4<T> makeRotationZ(T roll) {
		T c = Math::cos(roll);
		T s = Math::sin(roll);
		return Matrix4<T> (  c, -s, 0, 0,
							 s,  c, 0, 0,
							 0,  0, 1, 0,
							 0,  0, 0, 1);
	}

	// implemented after definition of Quaternion 
	static Matrix4<T> makeRotation(const Quaternion<T>& quat); 
	static Matrix4<T> makeTRS(const Vector3D<T>& translation, const Quaternion<T>& rotation, const Vector3D<T>& scale);
	static Matrix4<T> makeInverseTRS(const Vector3D<T>& translation, const Quaternion<T>& rotation, const Vector3D<T>& scale);
	static Matrix4<T> makeTRS(const Vector3D<T>& translation, const Matrix4<T>& rotation, const Vector3D<T>& scale);
	static Matrix4<T> makeInverseTRS(const Vector3D<T>& translation, const Matrix4<T>& rotation, const Vector3D<T>& scale);

	static Matrix4<T> makeScale(T x, T y, T z) {
		return Matrix4<T> ( x, 0, 0, 0,
							0, y, 0, 0,
							0, 0, z, 0,
							0, 0, 0, 1);
	}

	static Matrix4<T> makeScale(Vector3D<T> s) {
		return makeScale(s.x, s.y, s.z);
	}

	static Matrix4<T> makeLookAt(const Vector3D<T>& eye,
							     const Vector3D<T>& target,
								 const Vector3D<T>& up) {

		Vector3D<T> focus = normalize(target - eye);
		Vector3D<T> side  = normalize(cross(focus, up));
		Vector3D<T> nup   = normalize(cross(side, focus));

		Matrix4<T> result ( side.x,   side.y,   side.z,   (T)0,
							nup.x,    nup.y,    nup.z,    (T)0,
							-focus.x, -focus.y, -focus.z, (T)0,
							(T)0,     (T)0,     (T)0,     (T)1);
		result = result * makeTranslation(-eye);
		return result;
	}

	static Matrix4<T> makePerspective(T fovy, T aspect, T zNear, T zFar) {

		T f = (T)1 / Math::tan(fovy / (T)2);
		T a11 = f / aspect;
		T a33 = (zFar + zNear) / (zNear - zFar);
		T a34 = ((T)2 * zFar * zNear) / (zNear - zFar);

		Matrix4<T> result ( a11,  0,   0,    0,
							0,    f,   0,    0,
							0,    0,   a33,  a34,
							0,    0,   -1,   0);
		return result;
	}

	static Matrix4<T> makeOrtho(T left, T right, T bottom, T top, T zNear, T zFar) {

		T a11 = (T)2 / (right - left);
		T a14 = (right + left) / (left - right);

		T a22 = (T)2 / (top - bottom);
		T a24 = (top + bottom) / (bottom - top);

		T a33 = (T)2 / (zNear - zFar);
		T a34 = (zFar + zNear) / (zNear - zFar);

		Matrix4<T> result ( a11,   0,     0,      a14,
							0,     a22,   0,      a24,
							0,     0,     a33,    a34,
							0,     0,     0,      1);
		return result;
	}

	static Matrix4<T> makeOrtho2D(T left, T right, T bottom, T top) {
		return Matrix4<T>::makeOrtho(left, right, bottom, top, (T)-1, (T)1);
	}

	static Matrix4<T> makeFrustum(T left, T right, T bottom, T top, T zNear, T zFar) {

		T a11 = (2 * zNear) / (right - left);
		T a13 = (right + left) / (right - left);

		T a22 = (2 * zNear) / (top - bottom);
		T a23 = (top + bottom) / (top - bottom);

		T a33 = (zFar + zNear) / (zNear - zFar);
		T a34 = (2 * zFar * zNear) / (zNear - zFar);

		Matrix4<T> result ( a11,   0,     a13,    0,
							0,     a22,   a23,    0,
							0,     0,     a33,    a34,
							0,     0,     -1,     0);
		return result;
	}

	inline Matrix4<T> scaleRotation() const {
		Matrix4<T> result (m11,   m12,  m13, (T)0,
						   m21,   m22,  m23, (T)0,
						   m31,   m32,  m33, (T)0,
						  (T)0,  (T)0, (T)0, (T)1);
		return result;
	}

	inline void asVectorSystem(Vector3D<T>& position, Vector3D<T>& dir, Vector3D<T>& up, Vector3D<T>& strafe) const {
		dir		 = Vector3D<T>(-elem[8], -elem[9], -elem[10]);
		up		 = Vector3D<T>( elem[4],  elem[5],  elem[6]);
		strafe	 = Vector3D<T>( elem[0],  elem[1],  elem[2]);

		Matrix4<T> invOrthonormal(m11,   m21,  m31, (T)0,
						  	      m12,   m22,  m32, (T)0,
							      m13,   m23,  m33, (T)0,
							     (T)0,  (T)0, (T)0, (T)1);
		Matrix4<T> translation = invOrthonormal * (*this);
		position = Vector3D<T>(-translation.m41, -translation.m42, -translation.m43);
	}

	// angles are in radians
	// roll is rotation about X, pitch is rotation about Y, yaw is about Z.
	// order of yaw, pitch, roll:
    // p' = roll( pitch( yaw(p) ) )
	inline void asYPR(float& yaw, float& pitch, float& roll) {
		T sinPitch, cosPitch, sinRoll, cosRoll, sinYaw, cosYaw;
		sinPitch = -m13;
		cosPitch = Math::sqrt((T)1 - sinPitch * sinPitch);

		if (Math::abs(cosPitch) > (T)Math::epsilon) {
			sinRoll = m23 / cosPitch;
			cosRoll = m33 / cosPitch;
			sinYaw  = m12 / cosPitch;
			cosYaw  = m11 / cosPitch;
		}
		else {
			sinRoll = -m32;
			cosRoll = m22;
			sinYaw = (T)0.0;
			cosYaw = (T)1.0;
		}

		yaw   = Math::atan2(sinYaw, cosYaw);
		pitch = Math::atan2(sinPitch, cosPitch);
		roll  = Math::atan2(sinRoll, cosRoll);
	}

	inline void asFrustum(T& left, T& right, T& bottom, T& top, T& zNear, T& zFar) const {

		T k    = (elem[10] - 1) / (elem[10] + 1);

		zNear  = elem[11] * ((1 - k) / (2 * k));
		zFar   = k * zNear;

		bottom = -zNear * ((1 - elem[6]) / (elem[5]));
		top    = zNear * ((1 + elem[6]) / (elem[5]));

		left   = -zNear * ((1 - elem[2]) / (elem[0]));
		right  = zNear * ((1 + elem[2]) / (elem[0]));
	}

	inline void asOrtho(T& left, T& right, T& bottom, T& top, T& zNear, T& zFar) const {

		zNear  = -((elem[11] +1) / (elem[10]));
		zFar   = ((1 - elem[11]) / (elem[10]));

		bottom = -((elem[7] +1) / (elem[5]));
		top    = ((1 - elem[7]) / (elem[5]));

		left   = -((elem[3] +1) / (elem[0]));
		right  = ((1 - elem[3]) / (elem[0]));
	}

	inline void asOrtho2D(T& left, T& right, T& bottom, T& top) const {

		T zNear, zFar;
		asOrtho(left, right, bottom, top, zNear, zFar);
	}

	inline void asPerspective(T& fovy, T& aspect, T& zNear, T& zFar) const {

		T k    = (elem[10] - 1) / (elem[10] + 1);

		zNear  = elem[11] * ((1 - k) / (2 * k));
		zFar   = k * zNear;

		fovy   = 2 * Math::atan((T)1 / elem[5]);
		aspect = elem[5] / elem[0];
	}

	// TODO: we are dropping support for Matrix3!!!
	// this will disappear soon
    inline Matrix3<T> matrix3() const {
        T values[9] = {elem[0], elem[1], elem[2],
                       elem[4], elem[5], elem[6],
                       elem[8], elem[9], elem[10]};

        return Matrix3<T>(values);
    }

    //TODO:dimi: do we need to define the operations here (Matrix4)
    //functions
private:
	Matrix4<T> convertMatrix(Math::Handedness csh, Math::MatrixOrder mo) const;
};

//implementations
template <typename T> const Matrix4<T> Matrix4<T>::zero
								= Matrix4<T>(0, 0, 0, 0,
											 0, 0, 0, 0,
											 0, 0, 0, 0,
											 0, 0, 0, 0);

template <typename T> const Matrix4<T> Matrix4<T>::unit
								= Matrix4<T>(1, 0, 0, 0,
											 0, 1, 0, 0,
											 0, 0, 1, 0,
											 0, 0, 0, 1);

template <typename T> const Matrix4<T> Matrix4<T>::identity
								= Matrix4<T>(1, 0, 0, 0,
											 0, 1, 0, 0,
											 0, 0, 1, 0,
											 0, 0, 0, 1);

template <typename T> const Matrix4<T> Matrix4<T>::zReflection
								= Matrix4<T>(1, 0,  0, 0,
											 0, 1,  0, 0,
											 0, 0, -1, 0,
											 0, 0,  0, 1);

//operations
template <typename T>
inline bool operator == (const Matrix4<T>& lop, const Matrix4<T>& rop) {
	for(int i = 0; i < 16; ++i)
		if(lop.elem[i] != rop.elem[i])
			return false;
	return true;
}

template <typename T>
inline bool operator != (const Matrix4<T>& lop, const Matrix4<T>& rop) {
	return !(lop == rop);
}

template <typename T>
inline Matrix4<T> operator + (const Matrix4<T>& lop, const Matrix4<T>& rop) {
    T elem[16];
    for(int i = 0; i < 16; ++i)
        elem[i] = lop(i) + rop(i);

    return Matrix4<T>(elem);
}

template <typename T>
inline Matrix4<T> operator - (const Matrix4<T>& lop, const Matrix4<T>& rop) {
    T elem[16];
    for(int i = 0; i < 16; ++i)
        elem[i] = lop(i) - rop(i);

    return Matrix4<T>(elem);
}

template <typename T>
inline Matrix4<T> operator - (const Matrix4<T>& op) {
    T elem[16];
    for(int i = 0; i < 16; ++i)
        elem[i] = -op(i);

    return Matrix4<T>(elem);
}

template <typename T>
inline Matrix4<T> operator + (const Matrix4<T>& op) {
    return op;
}

template <typename T> //TODO:dimi: there is a better way to do this
inline Matrix4<T> operator * (const Matrix4<T>& lop, const Matrix4<T>& rop) {
    T elem[4][4];
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			elem[i][j] = 0;
			for(int k = 0; k < 4; k++){
                elem[i][j] += lop(i, k) * rop(k, j);
			}
		}
	}

    return Matrix4<T>(&elem[0][0]);
}

template <typename T>
inline Vector3D<T> operator * (const Matrix4<T>& lop, const Vector3D<T>& rop) {

    Vector3D<T> v;

    v.x = rop.x * lop(0, 0) +
          rop.y * lop(0, 1) +
          rop.z * lop(0, 2) +
                  lop(0, 3);

    v.y = rop.x * lop(1, 0) +
          rop.y * lop(1, 1) +
          rop.z * lop(1, 2) +
                  lop(1, 3);

    v.z = rop.x * lop(2, 0) +
          rop.y * lop(2, 1) +
          rop.z * lop(2, 2) +
                  lop(2, 3);

    return v;
}

//functions
template <typename T>
inline Matrix4<T> transpose(const Matrix4<T>& lop) {
    T values[16] = {lop.elem[0], lop.elem[4], lop.elem[8],  lop.elem[12],
                    lop.elem[1], lop.elem[5], lop.elem[9],  lop.elem[13],
                    lop.elem[2], lop.elem[6], lop.elem[10], lop.elem[14],
                    lop.elem[3], lop.elem[7], lop.elem[11], lop.elem[15]};

    return Matrix4<T>(values);
}

// TODO: this is quite unstable
template <typename T>
Matrix4<T> invert(const Matrix4<T>& lop) {
    float t;
    Matrix4<T> tmp = lop;
    Matrix4<T> result = Matrix4<T>( 1, 0, 0, 0,
									0, 1, 0, 0,
									0, 0, 1, 0,
									0, 0, 0, 1);

    for (int i = 0; i < 4; ++i) {
        // Look for largest element in column
        int swap = i;
        for (int j = i + 1; j < 4; ++j) {
			if (Math::abs(tmp(j, i)) > Math::abs(tmp(i,i)))
                swap = j;
        }

        if (swap != i) {
            // Swap rows.
            for (int k = 0; k < 4; ++k) {
                t = tmp(i,k);
				tmp.elem[4*i + k] = tmp(swap, k);
                tmp.elem[4*swap + k] = t;

                t = result.elem[i*4+k];
                result.elem[i*4+k] = result.elem[swap*4+k];
                result.elem[swap*4+k] = t;
            }
        }

        if (tmp(i, i) == 0) {
            // The matrix is singular
            return Matrix4<T>();
        }

        t = tmp(i, i);
        for (int k = 0; k < 4; k++) {
			tmp.elem[4*i + k] /= t;
            result.elem[i*4+k] /= t;
        }
        for (int j = 0; j < 4; j++) {
            if (j != i) {
                t = tmp(j, i);

                for (int k = 0; k < 4; k++) {
					tmp.elem[4 * j + k] -= tmp(i, k) * t;
                    result.elem[j*4+k] -= result.elem[i*4+k]*t;
                }
            }
        }
    }
    return result;
}

// help function (private)
template <typename T>
Matrix4<T> Matrix4<T>::convertMatrix(Math::Handedness csh, Math::MatrixOrder mo) const {
	Matrix4<T> mx = *this;

	if (mo == Math::MO_COLROW)
		mx = transpose(mx);

	if (csh == Math::CSH_LEFT)
		mx = Matrix4<T>::zReflection * mx;

	return mx;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Quaternion

template <typename T>
class Quaternion{

public:

    union{
        struct{
            T w, x, y, z;
        };
        T elem[4];
    };


	Quaternion()
		:w(1), x(0), y(0), z(0) {
	}

	Quaternion(T ww, T xx, T yy, T zz)
        : w(ww), x(xx), y(yy), z(zz){
    }

    Quaternion(const Matrix3<T>& matrix){ // from matrix3
        initializeFromMatrix3(matrix);
    }

    Quaternion(const Matrix4<T>& matrix){ //from matrix4
        initializeFromMatrix3(matrix.matrix3());
    }

	// angles are in radians
	// roll is rotation about X, pitch is rotation about Y, yaw is about Z.
    // p' = roll( pitch( yaw(p) ) )
    Quaternion(T yaw, T pitch, T roll){ //from euler (in rads)

        T cr = Math::cos(roll / T(2.0));
	    T cp = Math::cos(pitch / T(2.0));
	    T cy = Math::cos(yaw / T(2.0));

	    T sr = Math::sin(roll / T(2.0));
	    T sp = Math::sin(pitch / T(2.0));
	    T sy = Math::sin(yaw / T(2.0));

	    T cpcy = cp * cy;
	    T spsy = sp * sy;

	    w = cr * cpcy + sr * spsy;
	    x = sr * cpcy - cr * spsy;
	    y = cr * sp * cy + sr * cp * sy;
	    z = cr * cp * sy - sr * sp * cy;
    }

    Quaternion(const Vector3D<T>& axis, T angle){ //from axis/angle

        Vector3D<T> ax = normalize(axis);
        T sina = Math::sin(angle / (T)2);

        x = ax.x * sina;
        y = ax.y * sina;
        z = ax.z * sina;
        w = Math::cos(angle / (T)2);
    }

    Quaternion(const Vector3D<T>& from, const Vector3D<T>& to){ //from one vector to another

        T cost = dot(from, to);

        // check if parallel
        if (cost > 1 - Math::epsilon) {
            x = y = z = 0;
            w = 1;
            return;
        }
        else if (cost < -1.0 + Math::epsilon){	// check if opposite

            // check if we can use cross product of from vector with [1, 0, 0]
            Vector3D<T> tvec(0, from.x, -from.y);

            if (length(tvec) < Math::epsilon){

	            // nope! we need cross product of from vector with [0, 1, 0]
                tvec = Vector3D<T>(-from.z, 0, from.x);
            }

            // normalize
            tvec = normalize(tvec);

        	x = tvec.x; y = tvec.y; z = tvec.z;
            w = 0;

            return;
        }

        // ... else we can just cross two vectors
        Vector3D<T> tvec = normalize(cross(from, to));

        // we have to use half-angle formulae (sin^2 t = ( 1 - cos (2t) ) / 2)
        T ss = Math::sqrt((1 - cost) / 2);
        tvec *= ss;

        // scale the axis to get the normalized quaternion
        x = tvec.x; y = tvec.y; z = tvec.z;

        // cos^2 t = ( 1 + cos (2t) ) / 2
        // w part is cosine of half the rotation angle
        w = Math::sqrt((1 + cost) / 2);
    }

	//constants
	static const Quaternion<T> identity;
	static const Quaternion<T> zero;

	// the quaternion must NOT be unit!
    inline Matrix3<T> matrix3() const{

		T sl = dot(*this, *this);

        T wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

        x2 = (x + x) / sl;
		y2 = (y + y) / sl;
		z2 = (z + z) / sl;

        xx = x * x2;
		xy = x * y2;
		xz = x * z2;

		yy = y * y2;
		yz = y * z2;
		zz = z * z2;

        wx = w * x2;
		wy = w * y2;
		wz = w * z2;

		// a column matrix
        T values[] = {(T)1 - (yy + zz), xy - wz,            xz + wy,
                      xy + wz,          (T)1 - (xx + zz),   yz - wx,
                      xz - wy,          yz + wx,            (T)1 - (xx + yy)};

        return Matrix3<T>(values);
    }

    inline Matrix4<T> matrix4() const{
        return matrix3().matrix4();
    }

    inline Vector3D<T> axis(const Quaternion<T>& op) const{

        Vector3D<T> tvec(x, y, z);
        T lght = length(tvec);

        if(lght > Math::epsilonf)
            return tvec / lght;

        return Vector3D<T>(0, 0, 1);
    }

    inline Vector3D<T> angle(const Quaternion<T>& op) const{

        Vector3D<T> tvec(x, y, z);

        if(length(tvec) > Math::epsilonf)
            return 2 * Math::acos(w);

        return 0;
    }

	//
	//void ypr(T& yaw, T& pitch, T& roll) {
	//	if (x * y + z * w >= 0.5f - Math::epsilon){
	//		yaw   = 2 * Math::atan2(x, w);
	//		pitch = 0;
	//		roll  = 0;
	//		return;
	//	}

	//	if (x * y + z * w <= -0.5f + Math::epsilon){
	//		yaw   = -2 * Math::atan2(x, w);
	//		pitch = 0;
	//		roll  = 0;
	//		return;
	//	}

	//	yaw		= Math::atan2(2 * y * w - 2 * x * z , 1 - 2 * y * y - 2 * z * z);
	//	pitch	= Math::asin(2 * x * y + 2 * z * w);
	//	roll	= Math::atan2(2 * x * w - 2 * y * z, 1 - 2 * x * x - 2 * z * z);
	//}

	// angles are in radians
	// roll is rotation about X, pitch is rotation about Y, yaw is about Z.
	// order of yaw, pitch, roll:
    // p' = roll( pitch( yaw(p) ) )
	void ypr(T& yaw, T& pitch, T& roll) {
		matrix4().asYPR(yaw, pitch, roll);
	}

	void get(T* arr) const {
		arr[0] = w;
		arr[1] = x;
		arr[2] = y;
		arr[3] = z;
	}

    inline void scaleAngle(T scale) {
        *this = Quaternion<T>(axis(), scale * angle());
    }

    inline Quaternion<T>& operator = (const Quaternion<T>& op) {
        x = op.x; y = op.y; z = op.z; w = op.w;
        return *this;
    }

    inline Quaternion<T>& operator += (const Quaternion<T>& op) {
        x += op.x; y += op.y; z += op.z; w += op.z;
        return *this;
    }

    inline Quaternion<T>& operator -= (const Quaternion<T>& op) {
        x -= op.x; y -= op.y; z -= op.z; w -= op.z;
        return *this;
    }

    inline Quaternion<T>& operator *= (const Quaternion<T>& op) {
        *this = *this * op;
        return *this;
    }

    inline Quaternion<T>& operator /= (const Quaternion<T>& op) {
        *this = *this / op;
        return *this;
    }

    inline bool operator == (const Quaternion<T>& op) {
      if (x != op.x || y != op.y || z != op.z || w != op.w)
		  return false;
	  return true;
    }

    inline bool operator != (const Quaternion<T>& op) {
      return (! (*this == op));
    }


	// static members
	static Vector3D<T> rotate(const Vector3D<T>& v, const Quaternion<T>& q) {
		Vector3D<T> uv = cross(Vector3D<T>(q.x, q.y, q.z), v);
		return v + (T(2) * q.w) * uv + T(2) * cross(Vector3D<T>(q.x, q.y, q.z), uv);
	}

	static Vector3D<T> rotate(const Vector3D<T>& v, float angle, const Vector3D<T>& axis) {
		return  Quaternion<T>::rotate(v, Quaternion<T>(axis, angle));
	}

private:

	// matrix is row-cow (standart for viargo)
    void initializeFromMatrix3(const Matrix3<T>& matrix) { //from matrix3

        T tr, s, q[4];
        int i, j, k, nxt[3] = {1, 2, 0};

        tr = matrix(0, 0) + matrix(1, 1) + matrix(2, 2);

        // check the diagonal
        if (tr > (T)0){
            s = Math::sqrt(tr + (T)1.0);
            w = s / (T)2;

            s = (T)0.5 / s;
            x = (matrix(1, 2) - matrix(2, 1)) * s;
            y = (matrix(2, 0) - matrix(0, 2)) * s;
            z = (matrix(0, 1) - matrix(1, 0)) * s;

        }
        else {	// diagonal is negative
            i = 0;
            if(matrix(1, 1) > matrix(0, 0))
                i = 1;

            if(matrix(2, 2) > matrix(i, i))
                i = 2;

	        j = nxt[i];
            k = nxt[j];

            s = Math::sqrt((matrix(i, i) - (matrix(j, j) - matrix(k, k))) + (T)1.0);
            q[i] = s * (T)0.5;

			if(s != 0)
                s = (T)0.5 / s;

            q[3] = (matrix(j, k) - matrix(k, j)) * s;
            q[j] = (matrix(i, j) + matrix(j, i)) * s;
            q[k] = (matrix(i, k) + matrix(k, i)) * s;

	        x = q[0]; y = q[1]; z = q[2];
	        w = q[3];

			if (s == 0)
				w = 1;
        }
		// ensure the quat is unit
		*this = normalize(*this);
    }
};

// constants
template <typename T> const Quaternion<T> Quaternion<T>::identity = Quaternion<T>((T)1, (T)0, (T)0, (T)0);
template <typename T> const Quaternion<T> Quaternion<T>::zero     = Quaternion<T>((T)0, (T)0, (T)0, (T)0);

//operations
template <typename T>
inline Quaternion<T> operator * (const Quaternion<T>& lop, const Quaternion<T>& rop){

	Quaternion<T> result;
	result.x = lop.w * rop.x + lop.x * rop.w + lop.y * rop.z - lop.z * rop.y;
	result.y = lop.w * rop.y + lop.y * rop.w + lop.z * rop.x - lop.x * rop.z;
	result.z = lop.w * rop.z + lop.z * rop.w + lop.x * rop.y - lop.y * rop.x;
	result.w = lop.w * rop.w - lop.x * rop.x - lop.y * rop.y - lop.z * rop.z;

	// make sure the resulting quaternion is a unit quat.
	return normalize(result);
}

template <typename T>
inline Quaternion<T> operator * (Quaternion<T>& lop, Vector3D<T>& rop) {
	Quaternion<T> q;
	q.w = 0;
	q.vec = rop;
	return lop * q;
}

template <typename T>
inline Quaternion<T> operator * (Vector3D<T>& lop, Quaternion<T>& rop) {
	Quaternion<T> q;
	q.w = 0;
	q.vec = lop;
	return q * rop;
}

template <typename T>
inline Quaternion<T> operator / (const Quaternion<T>& lop, const Quaternion<T>& rop){

    Quaternion<T> q = Quaternion<T>::conjugate(rop);
    Quaternion<T> r = lop * q;
    Quaternion<T> s = q * q;

    return Quaternion<T>(r.w / s.w, r.x / s.w, r.y / s.w, r.z / s.w);
}

template <typename T>
inline Quaternion<T> operator + (const Quaternion<T>& lop, const Quaternion<T>& rop){
    Quaternion<T> result = lop;
    result += rop;
    return result;
}

template <typename T>
inline Quaternion<T> operator - (const Quaternion<T>& lop, const Quaternion<T>& rop){
    Quaternion<T> result = lop;
    result -= rop;
    return result;
}

template <typename T>
inline Quaternion<T> operator - (const Quaternion<T>& op){
    return Quaternion<T>(-op.w, -op.x, -op.y, -op.z);
}

template <typename T>
inline bool operator == (const Quaternion<T>& lop, const Quaternion<T>& rop){
  if ((lop.x != rop.x) || (lop.y != rop.y) || (lop.z != rop.z) || (lop.w != rop.w))
	return false;
  return true;
}

template <typename T>
inline bool operator != (const Quaternion<T>& lop, const Quaternion<T>& rop){
	return (!(lop == rop));
}

//functions
template <typename T>
inline T length(const Quaternion<T>& op){
    return Math::sqrt(dot(op, op));
}

template <typename T>
inline Quaternion<T> normalize(const Quaternion<T>& op){

    T square = dot(op, op);
	T f = (square > (T)Math::epsilon ? ((T)1.0 / Math::sqrt(square)) : (T)1.0);

    return Quaternion<T>(op.w * f, op.x * f, op.y * f, op.z * f);
}

template <typename T>
inline Quaternion<T> conjugate(const Quaternion<T>& op){
    return Quaternion<T>(op.w, -op.x, -op.y, -op.z);
}

template <typename T>
inline Quaternion<T> invert(const Quaternion<T>& op){

    T lght = dot(op, op);
    Quaternion<T> result;

    if(lght != 0) {
        result = conjugate(op);
        return Quaternion<T>(result.w / lght,
                             result.x / lght,
                             result.y / lght,
                             result.z / lght);
    }
    return op;
}

template <typename T>
inline Quaternion<T> exp(const Quaternion<T>& op){

	T theta = Math::sqrt(op.x*op.x + op.y*op.y + op.z*op.z);
	T scale = (T)1;

	if (theta > (T)Math::epsilon)
		scale = Math::sin(theta) / theta;

	return Quaternion<T>(Math::cos(theta),
						 scale * op.x,
						 scale * op.y,
						 scale * op.z);
}

template <typename T>
inline Quaternion<T> log(const Quaternion<T>& op){

	T scale = Math::sqrt(op.x*op.x + op.y*op.y + op.z*op.z);
	T theta = Math::atan2(scale, op.w);

	if (scale > (T)0)
		scale = theta / scale;

	return Quaternion<T>((T)0,
						 scale * op.x,
						 scale * op.y,
						 scale * op.z);
}

template <typename T>
inline T dot(const Quaternion<T>& lop, const Quaternion<T>& rop){
    return (lop.w * rop.w + lop.x * rop.x + lop.y * rop.y + lop.z * rop.z);
}

template <typename T>
inline Quaternion<T> slerp(const Quaternion<T>& from, const Quaternion<T>& to, T t){

        T omega, cosom, sinom;
        T scale0, scale1;

        Quaternion<T> to1;

        // calc cosine
        cosom = dot(from, to);

        // adjust signs (if necessary)
        if(cosom < (T)0){
		    cosom = -cosom;
		    to1 = -to;
        }
        else{
		    to1 = to;
        }

		if (((T)1 + cosom) > (T)Math::epsilon) {
			// calculate coefficients
			if ((1 - cosom) > (T)Math::epsilon){
				// standard case (slerp)
				omega = Math::acos(cosom);
				sinom = Math::sin(omega);
				scale0 = Math::sin(((T)1 - t) * omega) / sinom;
				scale1 = Math::sin(t * omega) / sinom;

			} else {
					// "from" and "to" quaternions are very close
					//  ... so we can do a linear interpolation
					scale0 = (T)1 - t;
					scale1 = t;
			}

			// calculate final values
			Quaternion<T> result;
			result.x = scale0 * from.x + scale1 * to1.x;
			result.y = scale0 * from.y + scale1 * to1.y;
			result.z = scale0 * from.z + scale1 * to1.z;
			result.w = scale0 * from.w + scale1 * to1.w;

			return result;
		}
		else {
			Quaternion<T> result(from.z, -from.y, from.x, -from.w);
			scale0 = Math::sin(((T)0.5 - t) * (T)Math::pi);
			scale1 = Math::sin(t * (T)Math::pi);

			result.x = scale0 * from.x + scale1 * result.x;
			result.y = scale0 * from.y + scale1 * result.y;
			result.z = scale0 * from.z + scale1 * result.z;
			result.w = scale0 * from.w + scale1 * result.w;

			return result;
		}
}

template <typename T>
inline Quaternion<T> lerp(const Quaternion<T>& from, const Quaternion<T>& to, T t){

    Quaternion<T> to1;
    T cosom, scale0, scale1;

    // calc cosine
    cosom = dot(from, to);

    // adjust signs (if necessary)
    if(cosom < 0)
	    to1 = -to;

    else
        to1 = to;

    // interpolate linearly
    scale0 = 1 - t;
    scale1 = t;

    // calculate final values
    Quaternion<T> result;
    result->x = scale0 * from.x + scale1 * to1.x;
    result->y = scale0 * from.y + scale1 * to1.y;
    result->z = scale0 * from.z + scale1 * to1.z;
    result->w = scale0 * from.w + scale1 * to1.w;

    return result;
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// Matrix4 functions using quaternions
template <typename T>
Matrix4<T> Matrix4<T>::makeRotation(const Quaternion<T>& quat) {
	return quat.matrix4();
}
// make TRS transformation with quaternion rotations
template <typename T>
Matrix4<T> Matrix4<T>::makeTRS(const Vector3D<T>& translation, const Quaternion<T>& rotation, const Vector3D<T>& scale) {
	return (Matrix4<T>::makeTranslation(translation) * (Matrix4<T>::makeRotation(rotation) * Matrix4<T>::makeScale(scale)));
}
// make inverse TRS transformation with quaternion rotations
template <typename T>
Matrix4<T> Matrix4<T>::makeInverseTRS(const Vector3D<T>& translation, const Quaternion<T>& rotation, const Vector3D<T>& scale) {
	Matrix4<T> result = Matrix4<T>::makeScale((T)1 / scale);
	result = (result * (Matrix4<T>::makeRotation(invert(rotation)) * Matrix4<T>::makeTranslation(-translation)));
	return result;
}
// make TRS transformation with mat4 rotations
template <typename T>
Matrix4<T> Matrix4<T>::makeTRS(const Vector3D<T>& translation, const Matrix4<T>& rotation, const Vector3D<T>& scale) {
	return makeTRS(translation, Quaternion<T>(rotation), scale);
}
// make inverse TRS transformation with mat4 rotations
template <typename T>
Matrix4<T> Matrix4<T>::makeInverseTRS(const Vector3D<T>& translation, const Matrix4<T>& rotation, const Vector3D<T>& scale) {
	return makeInverseTRS(translation, Quaternion<T>(rotation), scale);
}

} // namespace viargo
#endif // VIARGO_MATRIX_H


