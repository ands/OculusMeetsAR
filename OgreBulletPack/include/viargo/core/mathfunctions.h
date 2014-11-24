
#ifndef VIARGO_MATH_FUNCTIONS_H
#define VIARGO_MATH_FUNCTIONS_H

namespace viargo{
//macros
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define ALL_1P_MATH_FUNS \
    MATH_FUN(cos);\
    MATH_FUN(sin);\
    MATH_FUN(tan);\
    MATH_FUN(acos); \
    MATH_FUN(asin);\
    MATH_FUN(atan); \
    MATH_FUN(cosh);\
    MATH_FUN(sinh);\
    MATH_FUN(tanh);\
    MATH_FUN(exp);\
    MATH_FUN(log);\
    MATH_FUN(log10);\
    MATH_FUN(ceil);\
    MATH_FUN(floor);\
    MATH_FUN(sqrt);\
    MATH_FUN(fabs);

#define ALL_2P_MATH_FUNS \
    MATH_FUN(atan2);\
    MATH_FUN(pow);

#define ALL_CPP_NUMERIC_TYPES\
	_VIARGO_NUM_LIMITS_(float, Float)\
	_VIARGO_NUM_LIMITS_(double, Double)\
	_VIARGO_NUM_LIMITS_(long double, LongDouble)\
	_VIARGO_NUM_LIMITS_(int, Int)\
	_VIARGO_NUM_LIMITS_(long, Long)\
	_VIARGO_NUM_LIMITS_(short, Short)\
	_VIARGO_NUM_LIMITS_(long long, LongLong)
	//_VIARGO_NUM_LIMITS_(unsigned int,       cUInt)
	//_VIARGO_NUM_LIMITS_(unsigned short,     cUShort)
	//_VIARGO_NUM_LIMITS_(unsigned long,      cULong)
	//_VIARGO_NUM_LIMITS_(unsigned long long, cULongLong)


//Math
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class Math{

public:

	enum Handedness {
		CSH_LEFT,
		CSH_RIGHT
	};

	enum MatrixOrder{
		MO_ROWCOL,
		MO_COLROW
	};

    static const double pi;
    static const float  pif;
    static const double epsilon;
    static const float  epsilonf;

#define _VIARGO_NUM_LIMITS_(type, name) \
	struct name {\
		static const type maxValue;\
		static const type minValue;\
		static const type epsilon;\
		static const type infinity;\
		static const type NaN;\
		static bool isNaN(type t);\
		static bool isInf(type t);\
		static bool isPInf(type t);\
		static bool isNInf(type t);\
	};
	ALL_CPP_NUMERIC_TYPES
#undef _VIARGO_NUM_LIMITS_

#define MATH_FUN(fun) \
    static double fun(double x); \
    static float  fun(float x); 
    ALL_1P_MATH_FUNS
#undef MATH_FUN

#define MATH_FUN(fun) \
    static double fun(double x, double y); \
    static float  fun(float x, float y);
    ALL_2P_MATH_FUNS
#undef MATH_FUN

    template <typename T>
    static inline T sign(T x){
        return (x < (T)0 ? (T)-1 : (x == (T)0 ? (T)0 : (T)1));
    }

    template <typename T>
    static inline T  minimum(T x, T y){
        return (x < y ? x : y);
    }

    template <typename T>
    static inline T maximum(T x, T y){
        return (x > y ? x : y);
    }

    template <typename T>
    static inline T abs(T x){
        return (x < 0 ? -x : x);
    }

	static inline float  degToRad(float  deg) { return ((deg * pif) / 180.0f);}
	static inline double degToRad(double deg) { return ((deg * pi)  / 180.0);}
	static inline float  radToDeg(float  rad) { return ((rad * 180.0f) / pif); }
	static inline double radToDeg(double rad) { return ((rad * 180.0)  / pi); }

	template <typename T>
	static inline T diagonalFovToFovy(T diagonalFOV, T aspectRatio) {
		return 2 * atan(  tan(diagonalFOV / 2)  /  sqrt(1 + aspectRatio * aspectRatio)  );
	}

	template <typename T>
	static inline T diagonalFovToFovx(T diagonalFOV, T aspectRatio) {
		return 2 * atan(  tan(diagonalFOV / 2)  /  sqrt(1 + 1 / (aspectRatio * aspectRatio))  );
	}

	template <typename T>
	static inline T fovyToDiagonalFov(T fovy, T aspectRatio) {
		return 2 * atan(  tan(fovy / 2)  *  sqrt(1 + aspectRatio * aspectRatio)	);
	}

		template <typename T>
	static inline T reducePeriodic(T p) {
		// Reduces periodic p from real number space to [0, 2PI]
		const T pi = (T)Math::pi;
		while (p < 0) {
			p += 2 * pi;
		}
		while (p >= 2 * pi) {
			p -= 2 * pi;
		}
		return p;
	}

	template <typename T>
	static inline T periodicMinusAbsolute(T p, T a) {
		// p in [0, 2PI]
		// a is real number
		return reducePeriodic(p - a);
	}

	template <typename T>
	static inline T periodicPlusAbsolute(T p, T a) {
		// p in [0, 2PI]
		// a is real number
		return reducePeriodic(p + a);
	}
	
	template <typename T>
	static inline T periodicMinusPeriodic(T p1, T p2) {
		// p1 in [0, 2PI]
		// p2 in [0, 2PI]
		// result in [-PI, PI]

		// We make the assumption that there is only a slight difference.
		// This means that we can be in one of the following four cases:
		// 
		//        [0 . . . . . . . .2PI][0 . . . . . . . .2PI][0 . . . . . . . .2PI]
		// Case A:  p2            p1      p2            p1      p2            p1
		// Case B:  p1            p2      p1            p2      p1            p2
		// Case C:      p1    p2              p1    p2              p1    p2
		// Case C':     p2    p1              p2    p1              p2    p1
		const T pi = (const T)Math::pi;
		T caseA = p1 - (p2 + 2 * pi);
		T caseB = p1 - (p2 - 2 * pi);
		T caseC = p1 - p2;
		T absA = abs(caseA);
		T absB = abs(caseB);
		T absC = abs(caseC);

		T tmp = 0;
		if (absA < absB) {
			if (absA < absC) {
				// a < b, a < c
				tmp = caseA;
			} else {
				// c <= a < b
				tmp = caseC;
			}
		} else {
			if (absC < absB) {
				// c < b <= a
				tmp = caseC;
			} else {
				// b <= a, b <= c
				tmp = caseB;
			}
		}
		return tmp;
	}
};

} // namespace viargo

#endif // VIARGO_MATH_FUNCTIONS_H

