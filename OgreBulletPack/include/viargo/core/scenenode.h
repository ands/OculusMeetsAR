#ifndef VIARGO_SCENE_NODE_H
#define VIARGO_SCENE_NODE_H

#include <algorithm>
#include <string>
#include <map>

#include "core/stringutils.h"

#include "core/vrgmath.h"
#include "core/shape.h"
#include "core/propertylist.h"

#include "core/scenenodelist.h"
#include "core/scenenodepath.h"

// TODO:dimi: comments!!!
// TODO:dimi: shape interface
namespace viargo {

	/// Some comment here :)
class SceneNode {

public:

	enum GraphMode {
		GM_TREE,
		GM_DAG,
		GM_GRAPH
	};

	enum TraversalOrder {
		TO_PREORDER_CHILDREN  = 0x00,
		TO_PREORDER_PARENTS   = 0x01,
		TO_POSTORDER_CHILDREN = 0x02,
		TO_POSTORDER_PARENTS  = 0x03
	};

	// NIL scene node
	static SceneNode& nil;

	// Tests two scene nodes for equality. 
	// NOTE  this operator only compare the names..
	bool operator == (const SceneNode& node) const;

	// Tests two scene nodes for un-equality.
	// @see <code>operator ==(...)</code>
	bool operator != (const SceneNode& node) const;

	// ctor
	SceneNode(std::string name);

	// name
	std::string name() const;

	// accessors
	virtual vec3f relativePosition() const;
	virtual void  setRelativePosition(vec3f pos);

	virtual vec3f relativeScale() const;
	virtual void  setRelativeScale(vec3f scale);

	virtual quat relativeRotation() const;
	virtual void setRelativeRotation(quat q);

	// manipulators
	virtual void translateRelative(vec3f trans);
	virtual void rotateRelative(quat q);
	virtual void scaleRelative(vec3f s);			
	virtual void scaleRelative(float uniformScale);	

	// node traversal methods
	SceneNode& findNode(bool (*searchFunction)(SceneNode&, const mat4f&), bool applyOnThis = false, 
		int maxSearchDepth = Math::Int::infinity, TraversalOrder to = TO_PREORDER_CHILDREN);

	SceneNodeList findAllNodes(bool (*searchFunction)(SceneNode&, const mat4f&), bool applyOnThis = false, 
		int maxSearchDepth = Math::Int::infinity, TraversalOrder to = TO_PREORDER_CHILDREN);

	void applyOnNode(void (*modifier)(SceneNode&, const mat4f&), bool applyOnThis = false, 
		int maxDepth = Math::Int::infinity, TraversalOrder to = TO_PREORDER_CHILDREN);
	

	template <typename ClassType>
	SceneNode& findNode(ClassType* classPtr,  bool (ClassType::*searchFunction)(SceneNode&, const mat4f&), 
		bool applyOnThis = false, int maxSearchDepth = Math::Int::infinity, TraversalOrder to = TO_PREORDER_CHILDREN);
	
	template <typename ClassType>
	SceneNodeList findAllNodes(ClassType* classPtr,  bool (ClassType::*searchFunction)(SceneNode&, const mat4f&), 
		bool applyOnThis = false, int maxSearchDepth = Math::Int::infinity, TraversalOrder to = TO_PREORDER_CHILDREN);


	template <typename ClassType>
	void applyOnNode(ClassType* classPtr,  void (ClassType::*modifier)(SceneNode&, const mat4f&), 
		bool applyOnThis = false, int maxDepth = Math::Int::infinity, TraversalOrder to = TO_PREORDER_CHILDREN);

	
	// object traversal methods
	SceneObject findObject(std::string objectName);
	SceneObject findObject(bool (*searchFunction)(SceneObject&), bool applyOnThis = false);
	std::vector<SceneObject> findAllObjects(bool (*searchFunction)(SceneObject&), bool applyOnThis = false);
	void applyOnObject(void (*modifier)(SceneObject&), bool applyOnThis = false);
	
	template <typename ClassType>
	SceneObject findObject(ClassType* classPtr, bool (ClassType::*searchFunction)(SceneObject&), 
		bool applyOnThis = false);


	template <typename ClassType>
	std::vector<SceneObject> findAllObjects(ClassType* classPtr, bool (ClassType::*searchFunction)(SceneObject&), 
		bool applyOnThis = false);

	
	template <typename ClassType>
	void applyOnObject(ClassType* classPtr, void (ClassType::*modifier)(SceneObject&), bool applyOnThis = false);
	
	// child methods
	virtual bool hasChild(std::string childName, bool recursive = false);
	virtual bool addChild(SceneNode* child);
	virtual bool removeChild(SceneNode& child);
	virtual SceneNode& child(std::string childName);
	virtual SceneNodeList children(bool recursive = false);
		
	// parents methods
	virtual bool isRoot() const;
	virtual bool isConnectedToParent(std::string parentName,  bool recursive = false);
	virtual bool connectToParent(SceneNode& parent);
	virtual bool disconnectFromParent(SceneNode& parent);
	virtual SceneNode& parent(std::string parentName);
	virtual SceneNodeList parents(bool recursive = true);

	// paths
	// Get the 'first' path for this node
	virtual SceneNodePath path();
	// Get all paths for this node
	virtual std::vector<SceneNodePath> paths();

	// geometry methods
	virtual void setShape(Shape* shape);
	virtual Shape* shape() const;
	virtual box3f bounds();

	// checks for ray collision
	virtual bool collide(const ray3f& ray, bool recursive = false, float t0 = 0.0f, float t1 = Math::Float::maxValue);
	// get all objects intersected by a ray
	virtual std::vector<SceneObject> select(const ray3f& ray, float t0 = 0.0f, float t1 = Math::Float::maxValue);
	// get all objects (fully) inside a box
	virtual std::vector<SceneObject> select(const Box3D& box);
	// get all objects with center inside a sphere
	virtual std::vector<SceneObject> select(const Sphere3D& sp);
	
	// property methods
	template <typename T> void setProperty(std::string propertyName, T value);
	template <typename T> T    getProperty(std::string propertyName, T defaultValue);
	void removeProperty(std::string propertyName);
	bool hasProperty(std::string propertyName) const;

protected:
	// protect the destructor to prevent creation on the stack
	virtual ~SceneNode();

	// transformation parameters (relative)
	vec3f _position;
	quatf _orientation;
	vec3f _scale;

	// geometry parameters
	Shape* _shape;

	// scene graph
	typedef std::map<std::string, SceneNode*> NamedNodeMap;
	NamedNodeMap _children;
	NamedNodeMap _parents;
	GraphMode    _graphMode;

	// set of named properties
	PropertyList _propertyList;

private:
	// name of the node (don't have to be unique)
	std::string _name;

	// helper methods
	void drop();

	void children(std::vector<SceneNode*>& result);
	void parents(std::vector<SceneNode*>& res);
	void clearCheckFlags();
	void clearCheckParentsFlags();
	bool _checked;

	void path(SceneNode* node, std::vector<SceneNode*>& p);
	std::vector<std::vector<SceneNode*> > allPaths();

	template <typename ClassName> 
	SceneNode* findNode(ClassName*, bool (ClassName::*)(SceneNode&, const mat4f&), int, TraversalOrder, bool, mat4f);
	
	template <typename ClassName> 
	std::vector<SceneNode*> findAllNodes(ClassName*, bool (ClassName::*)(SceneNode&, const mat4f&), int, 
		TraversalOrder, bool, mat4f);
	
	template <typename ClassName> 
	void applyOnNode(ClassName*, void (ClassName::*)(SceneNode&, const mat4f&), int, TraversalOrder, bool, mat4f);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class RootNode : public SceneNode {
	friend class Scene;
public:

	// ctor
	RootNode(GraphMode gm = SceneNode::GM_DAG) 
		:SceneNode("[root]")
	{
		_graphMode = gm;
	}

	virtual bool isRoot() const {
		return true;
	}
	virtual bool isConnectedToParent(std::string,  bool) {
		return false;
	}
	virtual bool connectToParent(SceneNode& parent) {
		return false;
	}
	virtual bool disconnectFromParent(SceneNode& parent) {
		return false;
	}
	virtual SceneNode& parent(std::string parentName) {
		return SceneNode::nil;
	}
	virtual SceneNodeList parents(bool recursive = true) {
		return SceneNodeList();
	}
	virtual void drop() {
		// nothing to do.. never die!
	}
};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
template <typename ClassName>
SceneObject SceneNode::findObject(ClassName* obj, bool (ClassName::*searchPredicate)(SceneObject&), bool applyOnThis) {

	// try on this
	if (applyOnThis) {
		std::vector<SceneNodePath> paths = this->paths();
		for (size_t i = 0; i < paths.size(); ++i) {
			if ((obj->*searchPredicate)(paths[i]))
				return paths[i];
		} 
	}
	
	// search into the children
	SceneNodeList allChildren = children(true);//recursive);	
	for (int i = 0; i < allChildren.size(); ++i) {
		std::vector<SceneNodePath> paths = allChildren[i].paths();
		for (size_t j = 0; j < paths.size(); ++j) {
			if ((obj->*searchPredicate)(paths[j]))
				return paths[j];
		}
	}
	return SceneObject::nil;
}

template <typename ClassName>
std::vector<SceneObject> SceneNode::findAllObjects(ClassName* obj, bool (ClassName::*searchPredicate)(SceneObject&), bool applyOnThis) {
	std::vector<SceneNodePath> result;

	// try on this
	if (applyOnThis) {
		std::vector<SceneNodePath> paths = this->paths();
		for (size_t i = 0; i < paths.size(); ++i) {
			if ((obj->*searchPredicate)(paths[i]))
				result.push_back(paths[i]);
		} 
	}

	// search into all children
	SceneNodeList allChildren = children(true);//recursive);
	for (int i = 0; i < allChildren.size(); ++i) {
		std::vector<SceneNodePath> paths = allChildren[i].paths();
		for (size_t j = 0; j < paths.size(); ++j) {
			if ((obj->*searchPredicate)(paths[j]))
				result.push_back(paths[j]);
		}
	}

	// remove duplicates
	std::sort(result.begin(), result.end());
	std::vector<SceneNodePath>::iterator newEnd = std::unique(result.begin(), result.end());
	result.resize(newEnd - result.begin());
	return result;
}

template <typename ClassName>
void SceneNode::applyOnObject(ClassName* obj, void (ClassName::*functor)(SceneObject&), bool applyOnThis) {
	// try on this
	if (applyOnThis) {
		std::vector<SceneNodePath> paths = this->paths();
		for (size_t i = 0; i < paths.size(); ++i) {
			(obj->*functor)(paths[i]);
		} 
	}

	// search into all children
	SceneNodeList allChildren = children(true);//recursive);
	for (int i = 0; i < allChildren.size(); ++i) {
		std::vector<SceneNodePath> paths = allChildren[i].paths();
		for (size_t j = 0; j < paths.size(); ++j) {
			(obj->*functor)(paths[j]);
		}
	}
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// node methods
template <typename ClassName>
SceneNode& SceneNode::findNode(ClassName* obj, bool (ClassName::*searchFunctor)(SceneNode&, const mat4f&), 
								   bool applyOnThis, int maxSearchDepth, SceneNode::TraversalOrder to) {
    SceneNode* ptr = findNode(obj, searchFunctor, maxSearchDepth, to, applyOnThis, mat4f::identity);
	if (ptr) 
		return *ptr;

	return SceneNode::nil;
}
template <typename ClassName>
SceneNode* SceneNode::findNode(ClassName* obj, bool (ClassName::*searchFunctor)(SceneNode&, const mat4f&), 
								   int maxSearchDepth, SceneNode::TraversalOrder to, bool applyOnThis, mat4f transform) {

	mat4f newTransform;

	NamedNodeMap* nodes;
	if (to == TO_PREORDER_CHILDREN || to == TO_POSTORDER_CHILDREN) {
		nodes = &_children;
		newTransform = mat4f::makeInverseTRS(relativePosition(), relativeRotation(), relativeScale()) * transform;
		transform = newTransform;
	}
	else {
		nodes = &_parents;
		newTransform = transform * mat4f::makeTRS(relativePosition(), relativeRotation(), relativeScale());
	}

	if (to == TO_PREORDER_CHILDREN || to == TO_PREORDER_PARENTS) { // preorder
		if (applyOnThis && (obj->*searchFunctor)(*this, transform))
			return this;

		if (maxSearchDepth > 0) {
			NamedNodeMap::iterator it = nodes->begin();
			for ( ; it != nodes->end(); ++it) {
				SceneNode* result = it->second->findNode(obj, searchFunctor, maxSearchDepth-1, to, true, newTransform);

				if (result)
					return result;
			}
		}
		return 0;
	}
	else { // post-order
		if (maxSearchDepth > 0) {
			NamedNodeMap::iterator it = nodes->begin();
			for ( ; it != nodes->end(); ++it) {
				SceneNode* result = it->second->findNode(obj, searchFunctor, maxSearchDepth-1, to, true, newTransform);
				if (result)
					return result;
			}
		}

		if (applyOnThis && (obj->*searchFunctor)(*this, transform))
			return this;
		
		return 0;
	}
}


template <typename ClassName>
SceneNodeList SceneNode::findAllNodes(ClassName* obj, bool (ClassName::*searchFunctor)(SceneNode&, const mat4f&), 
												  bool applyOnThis, int maxSearchDepth, SceneNode::TraversalOrder to) {
	
	std::vector<SceneNode*> result = findAllNodes(obj, searchFunctor, maxSearchDepth, to, applyOnThis, mat4::identity);
	// remove duplicates
	std::sort(result.begin(), result.end());
	std::vector<SceneNode*>::iterator newEnd = std::unique(result.begin(), result.end());
	result.resize(newEnd - result.begin());
	// return :)
	return SceneNodeList(result);
}

template <typename ClassName>
std::vector<SceneNode*> SceneNode::findAllNodes(ClassName* obj, bool (ClassName::*searchFunctor)(SceneNode&, const mat4f&),
												  int maxSearchDepth, SceneNode::TraversalOrder to, bool applyOnThis, mat4f transform) {
	
	mat4f newTransform;

	NamedNodeMap* nodes;
	if (to == TO_PREORDER_CHILDREN || to == TO_POSTORDER_CHILDREN) {
		nodes = &_children;
		newTransform = mat4f::makeInverseTRS(relativePosition(), relativeRotation(), relativeScale()) * transform;
		transform = newTransform;
	}
	else {
		nodes = &_parents;
		newTransform = transform * mat4f::makeTRS(relativePosition(), relativeRotation(), relativeScale());
	}

	std::vector<SceneNode*> result;
	if (to == TO_PREORDER_CHILDREN || to == TO_PREORDER_PARENTS) { // preorder
		if (applyOnThis && (obj->*searchFunctor)(*this, transform))
			result.push_back(this);

		if (maxSearchDepth > 0) {
			NamedNodeMap::iterator it = nodes->begin();
			for ( ; it != nodes->end(); ++it) {
				std::vector<SceneNode*> resultChild = 
					it->second->findAllNodes(obj, searchFunctor, maxSearchDepth-1, to, true, newTransform);
				std::copy(resultChild.begin(), resultChild.end(), std::back_inserter(result));
			}
		}
		return result;
	}
	else  { // post-order

		if (maxSearchDepth > 0) {
			NamedNodeMap::iterator it = nodes->begin();
			for ( ; it != nodes->end(); ++it) {
				std::vector<SceneNode*> resultChild = 
					it->second->findAllNodes(obj, searchFunctor, maxSearchDepth-1, to, true, newTransform);
				std::copy(resultChild.begin(), resultChild.end(), std::back_inserter(result));
			}
		}
		if (applyOnThis && (obj->*searchFunctor)(*this, transform))
			result.push_back(this);
		
		return result;
	}
}


template <typename ClassName>
void SceneNode::applyOnNode(ClassName* obj, void (ClassName::*functor)(SceneNode&, const mat4f&), bool applyOnThis, int maxDepth, 
							SceneNode::TraversalOrder to) {
	applyOnNode(obj, functor, maxDepth, to, applyOnThis, mat4f::identity);
}
template <typename ClassName>
void SceneNode::applyOnNode(ClassName* obj,  void (ClassName::*functor)(SceneNode&, const mat4f&), int maxDepth, 
							SceneNode::TraversalOrder to, bool applyOnThis, mat4f transform) {
	
	mat4f newTransform;

	NamedNodeMap* nodes;
	if (to == TO_PREORDER_CHILDREN || to == TO_POSTORDER_CHILDREN) {
		nodes = &_children;
		newTransform = mat4f::makeInverseTRS(relativePosition(), relativeRotation(), relativeScale()) * transform;
		transform = newTransform;
	}
	else {
		nodes = &_parents;
		newTransform = transform * mat4f::makeTRS(relativePosition(), relativeRotation(), relativeScale());
	}

	if (to == TO_PREORDER_CHILDREN || to == TO_PREORDER_PARENTS) { // preorder
		if (applyOnThis)
			(obj->*functor)(*this, transform);

		if (maxDepth > 0) {
			NamedNodeMap::iterator it = nodes->begin();
			for ( ; it != nodes->end(); ++it) 
				it->second->applyOnNode(obj, functor, maxDepth-1, to, true, newTransform);
		}
	}
	else { // post-order
		if (maxDepth > 0) {
			NamedNodeMap::iterator it = nodes->begin();
			for ( ; it != nodes->end(); ++it)
				it->second->applyOnNode(obj, functor, maxDepth-1, to, true, newTransform);
		}
		
		if (applyOnThis)
			(obj->*functor)(*this, transform);
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// property methods
template <typename T> void SceneNode::setProperty(std::string propertyName, T value) {
	_propertyList.set(propertyName, value);
}

template <typename T> T SceneNode::getProperty(std::string propertyName, T defaultValue) {
	return _propertyList.get(propertyName, defaultValue);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// ScenePath property methods
// THIS MUST BE HERE !!!!
template <typename T> void SceneNodePath::setProperty(std::string propertyName, T value) {
	back().setProperty(propertyName, value);
}	
template <typename T> T    SceneNodePath::getProperty(std::string propertyName, T defaultValue) {
	return back().getProperty(propertyName, defaultValue);
}



} // namespace viargo

#endif // VIARGO_SCENE_NODE_H
