#ifndef VIARGO_RAY3D_H
#define VIARGO_RAY3D_H

#include "core/vector.h"

namespace viargo {

template <typename T>
struct Ray3D {

	// Default contstructor
	Ray3D() {
	}
		
	// Parameter constructor 
    Ray3D(Vector3D<T> org, Vector3D<T> dir)
	: origin(org),
	  direction(normalize(dir)),
	  invDirection(Vector3D<double>(1.0 / direction.x, 1.0 / direction.y, 1.0 / direction.z)) {
	  
      sign[0] = (invDirection.x < 0);
      sign[1] = (invDirection.y < 0);
      sign[2] = (invDirection.z < 0);
    }
	
	// Copy Constructor
    Ray3D(const Ray3D<T>& r) {
      origin = r.origin;
      direction = r.direction;
      invDirection = r.invDirection;
      
	  sign[0] = r.sign[0]; 
	  sign[1] = r.sign[1]; 
	  sign[2] = r.sign[2];
    }

	static Ray3D<T> transform(const Ray3D<T>& ray, const Matrix4<T>& t) {
		Vector3D<T> o = t * ray.origin;
		Vector3D<T> pd = t * (ray.origin + ray.direction);
		Vector3D<T> d = normalize(pd - o);

		return Ray3D<T>(o, d);
	}

	// Members
    Vector3D<T> origin;
    Vector3D<T> direction;
    Vector3D<double> invDirection;
    int sign[3];
};

} // namespace viargo
#endif // VIARGO_RAY3D_H
