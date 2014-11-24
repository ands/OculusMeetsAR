
#ifndef VIARGO_SHAPE_H
#define VIARGO_SHAPE_H

#include <vector>
#include "core/vrgmath.h"

namespace viargo {

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/**
* Basis class for description of geometric shapes
*/
class Shape {

public:
	// Constructor
	Shape();

	// Destructor
	virtual ~Shape();

	// intersects the shape with a ray
	// @returns true if the shape is intersected from the ray
	virtual bool intersect(const ray3& ray, float t0, float t1) const = 0;

	// contains a 3D point
	// @returns true if the box is contained in the shape, false otherwise
	virtual bool contains(const vec3f& b) const = 0;

	// gets the center of the shape
	virtual vec3f center() const = 0;

	// gets the AABBox dimensions of the shape
	virtual vec3f size() const = 0;

protected:

};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/**
* Basic class for description of a AABBox
*/
struct Box3D : public Shape {

	// Default constructor
	Box3D();

	// Parameter constructor
	Box3D(const vec3f& llf, const vec3f& urb);

	// Parameter constructor
	Box3D(const vec3f& center, float width, float height, float depth);

	// Parameter constructor
	Box3D(const Shape& sh);

	static Box3D transform(const Box3D& box, const mat4f& transformation);

	void addVertex(const vec3f& v, const mat4f& transformation = mat4f::identity);
	void addShape(const Shape& sh, const mat4f& transformation = mat4f::identity);

	// Returns the center of the box
	vec3f center() const;

	// Returns the size of this box (width, height, depth)
	vec3f size() const;

    // Returns the diagonal from llf to urb
    vec3f diagonal() const;

    // Returns the volume of this box
    float volume() const;

	// Returns true if the box contains a point
	bool contains(const vec3f& p) const;

	// Returns true if the box contains a box
	bool contains(const Box3D& b) const;

	// Intersects this box with another one
	bool intersect(const Box3D& b) const;

	// Intersects this box with a ray using the algorithm described in
	// Amy Williams et al., "An Efficient and Robust Ray-Box Intersection Algorithm",
	// Journal of graphics tools, 10(1):49-54, 2005
	//
    // @param:   (t0, t1) - the interval for valid hits
	// @returns: true if the box is intersected within the range
	// @note: the algorithm relays on the correct implementation of ANSI/IEEE 754-1985 Standart.
    bool intersect(const ray3f& r, float t0, float t1) const;

    // Members
    public:
        vec3f llf;
		vec3f urb;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/**
* Basic class for description of a sphere
*/
struct Sphere3D : public Shape {

	Sphere3D();
	Sphere3D(const vec3f& cc, float rr);
	Sphere3D(const vec3f& cc, float width, float height, float depth);

	~Sphere3D();
	static Sphere3D transform(const Sphere3D& sp, const mat4f& transform);

	void addVertex(const vec3f& vx);
	void addShape(const Shape& sh);

	bool intersect(const ray3& ray, float t0, float t1) const;
	bool intersect(const Sphere3D& s) const;

	bool contains(const vec3f& p) const;
	bool contains(const Sphere3D& s) const;

	vec3f center() const;
	float radius() const;
	vec3f size() const;

    // Members
	public:
        vec3f c;
		float r;
};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/**
* Basic class for description of composite geometric shapes
*/
class CompositeShape : public Shape {

public:
	CompositeShape();
	~CompositeShape();

	void addShape(Shape* shape);

	void removeShape(Shape* shape);

	std::vector<Shape*> shapes() const;
	bool empty() const;

	bool intersect(const ray3& ray, float t0, float t1) const;

	vec3f center() const;
	vec3f size() const;

protected:
	std::vector<Shape*> _shapes;
	Box3D _bounds;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
typedef Box3D box3f;
typedef box3f box3;



} // nemespace viarfo
#endif // VIARGO_SHAPE_H



