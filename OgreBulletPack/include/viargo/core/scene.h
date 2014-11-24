
#ifndef VIARGO_SCENE_H
#define VIARGO_SCENE_H

#include <string>
#include <map>
#include <set>
#include <algorithm>

#include "core/vrgmath.h"
#include "core/shape.h"
#include "core/propertylist.h"

#include "core/scenenode.h"

// TODO:dimi: comments!!!
// TODO:dimi: shape interface
namespace viargo {

class Scene { 

public:

	Scene(SceneNode::GraphMode graphMode = SceneNode::GM_DAG);
	~Scene();

	//root
	SceneNode& root() {
		return *_root;
	}

	// node traversal methods
	SceneNode& findNode(bool (*searchFunction)(SceneNode&, const mat4f&), bool applyOnRoot = true) {
		return root().findNode(searchFunction, applyOnRoot);
	}

	SceneNodeList findAllNodes(bool (*searchFunction)(SceneNode&, const mat4f&), bool applyOnRoot = true) {
		return root().findAllNodes(searchFunction, applyOnRoot);
	}

	void applyOnNode(void (*modifier)(SceneNode&, const mat4f&), bool applyOnRoot = true) {
		root().applyOnNode(modifier, applyOnRoot);
	}

	template <typename ClassType>
	SceneNode& findNode(ClassType* classPtr,  bool (ClassType::*searchFunction)(SceneNode&, const mat4f&), 
						bool applyOnRoot = true) {
		return root().findNode(classPtr, searchFunction, applyOnRoot);
	}
	
	template <typename ClassType>
	SceneNodeList findAllNodes(ClassType* classPtr,  bool (ClassType::*searchFunction)(SceneNode&, const mat4f&), 
							   bool applyOnRoot = true) {
		return root().findAllNodes(classPtr, searchFunction, applyOnRoot);
	}

	template <typename ClassType>
	void applyOnNode(ClassType* classPtr,  void (ClassType::*modifier)(SceneNode&, const mat4f&), bool applyOnRoot = true) {
		root().applyOnNode(classPtr, modifier, applyOnRoot);
	}

	
	// object traversal methods
	SceneObject findObject(std::string objectName) {
		return root().findObject(objectName);
	}
	
	SceneObject findObject(bool (*searchFunction)(SceneObject&), bool applyOnRoot = false) {
		return root().findObject(searchFunction, applyOnRoot);
	}
	
	std::vector<SceneObject> findAllObjects(bool (*searchFunction)(SceneObject&), bool applyOnRoot = false) {
		return root().findAllObjects(searchFunction, applyOnRoot);
	}
	
	void applyOnObject(void (*modifier)(SceneObject&), bool applyOnRoot = false) {
		root().applyOnObject(modifier, applyOnRoot);
	}
	
	template <typename ClassType>
	SceneObject findObject(ClassType* classPtr, bool (ClassType::*searchFunction)(SceneObject&), 
						   bool applyOnRoot = false) {
		return root().findObject(classPtr, searchFunction, applyOnRoot);
	}

	template <typename ClassType>
	std::vector<SceneObject> findAllObjects(ClassType* classPtr, bool (ClassType::*searchFunction)(SceneObject&), 
											bool applyOnRoot = false) {
		return root().findAllObjects(classPtr, searchFunction, applyOnRoot);
	}

	template <typename ClassType>
	void applyOnObject(ClassType* classPtr, void (ClassType::*modifier)(SceneObject&), bool applyOnRoot = false) {
		root().applyOnObject(classPtr, modifier, applyOnRoot);
	}

	// checks for ray collision
	bool collide(const ray3f& ray, float t0 = 0.0f, float t1 = Math::Float::maxValue) {
		return root().collide(ray, true, t0, t1);
	}
	// get all objects intersected by a ray
	std::vector<SceneObject> select(const ray3f& ray, float t0 = 0.0f, float t1 = Math::Float::maxValue) {
		return root().select(ray, t0, t1);
	}
	// get all objects (fully) inside a box
	virtual std::vector<SceneObject> select(const Box3D& box) {
		return root().select(box);
	}
	// get all objects with center inside a sphere
	virtual std::vector<SceneObject> select(const Sphere3D& sp) {
		return root().select(sp);
	}

	// shapes
	void addShape(std::string shapeName, Shape* shape);
	void removeShape(std::string shapeName);
	Shape* shape(std::string shapeName);

private:
	typedef std::map<std::string, Shape*> NameShapeMap;
	
	RootNode*    _root;
	NameShapeMap _shapes;
};
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

} // namespace viargo
#endif // VIARGO_SCENE_H




