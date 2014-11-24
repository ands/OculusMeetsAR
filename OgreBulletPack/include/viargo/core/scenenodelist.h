#ifndef VIARGO_SCENE_NODE_LIST
#define VIARGO_SCENE_NODE_LIST

#include <vector>

namespace viargo {

// forward declarations
class SceneNode;

class SceneNodeList {
	friend class SceneNodePath;
public:
	
	// default ctor
	SceneNodeList();
	// copy ctor
	SceneNodeList(const SceneNodeList& cp);	
	// data ctor
	SceneNodeList(std::vector<SceneNode*> data);
	// dtor
	virtual ~SceneNodeList();
	// assignment op
	SceneNodeList& operator= (const SceneNodeList& cp);

	// size
	inline int  size()  const { return (int)_list.size(); }
	inline bool empty() const { return _list.empty(); }

	// access
	inline SceneNode& operator[](int idx) { return at(idx); }
	inline SceneNode& front() { return at(0); }
	inline SceneNode& back()  { return at(size() - 1); }
	SceneNode& at(int idx);

	inline const SceneNode& operator[](int idx) const { return at(idx); }
	inline const SceneNode& front() const { return at(0); }
	inline const SceneNode& back()  const { return at(size() - 1); }
	const SceneNode& at(int idx) const;

protected:
	std::vector<SceneNode*> _list;

private:

};

} // namespace viargo
#endif // VIARGO_SCENE_NODE_LIST
