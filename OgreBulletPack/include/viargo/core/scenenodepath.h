#ifndef VIARGO_SCENE_NODE_PATH_H
#define VIARGO_SCENE_NODE_PATH_H

#include <string>
#include <vector>

#include "core/vrgmath.h"
#include "core/scenenodelist.h"

namespace viargo {

// forward declaration
class Shape;

//-------------------------------------------------------------------------------------------------
class SceneNodePath : public SceneNodeList {

public:
	// nil value
	static SceneNodePath& nil;

	// default ctor
	SceneNodePath();
	// copy ctor
	SceneNodePath(const SceneNodePath& cp);	
	// data ctor
	SceneNodePath(std::vector<SceneNode*> data);
	// dtor
	virtual ~SceneNodePath();
	// assignment op
	SceneNodePath& operator= (const SceneNodePath& cp);

	// operators
	bool operator<= (const SceneNodePath& cp) const;
	bool operator>= (const SceneNodePath& cp) const;
	bool operator== (const SceneNodePath& cp) const;
	bool operator!= (const SceneNodePath& cp) const;
	bool operator<  (const SceneNodePath& cp) const;
	bool operator>  (const SceneNodePath& cp) const;

	// name
	std::string name() const {
		return pathString();
	}

	// path methods
	bool isRootPath() const;	
	std::string pathString() const;
	SceneNode& root();

	// valid check
	bool valid() const {
		return (_list.size() > 0);
	}

	SceneNodePath parent();
	std::vector<SceneNodePath> children();

	// SceneNode methods	
	virtual vec3f relativePosition() const;
	virtual void  setRelativePosition(vec3f pos);

	virtual vec3f relativeScale() const;
	virtual void  setRelativeScale(vec3f scale);

	virtual quat relativeRotation() const;
	virtual void setRelativeRotation(quat q);

	virtual vec3f absolutePosition();
	virtual void  setAbsolutePosition(vec3f gpos);

	virtual vec3f absoluteScale();
	virtual void  setAbsoluteScale(vec3f gscale);

	virtual quat  absoluteRotation();
	virtual void  setAbsoluteRotation(quatf gori);
	
	virtual mat4f absoluteMatrix();
	virtual mat4f absoluteMatrixInverse();

	// manipulators
	virtual void translateRelative(vec3f trans);
	virtual void translateAbsolute(vec3f gtrans);
	
	virtual void rotateRelative(quat q);
	virtual void rotateAbsolute(quatf gquat);
	
	virtual void scaleRelative(vec3f s);	
	virtual void scaleRelative(float uniformScale);
	virtual void scaleAbsolute(vec3f gscale); 	
	virtual void scaleAbsolute(float gscale); 
		
	// geometry methods
	virtual void setShape(Shape* shape);
	virtual Shape* shape() const;
	virtual box3f bounds();

	// property methods
	// implemented in scenenode.h after the definition of SceneNode!!!
	template <typename T> void setProperty(std::string propertyName, T value);	
	template <typename T> T    getProperty(std::string propertyName, T defaultValue);
	void removeProperty(std::string propertyName);
	bool hasProperty(std::string propertyName) const;

protected:

private:
	mat4f _globalToLocalTransform;
	mat4f _localToGlobalTransform;

	vec3f _globalPosition;
	quat  _globalOrientation;
	vec3f _globalScale;

	// helper
	void recalculateGlobalMatrix();
	bool normalizePath();
};
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// alias
typedef SceneNodePath SceneObject;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

} // namespace viargo
#endif // VIARGO_SCENE_NODE_PATH_H
