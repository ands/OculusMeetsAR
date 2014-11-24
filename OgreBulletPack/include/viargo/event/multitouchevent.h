
#ifndef VIARGO_MULTI_TOUCH_EVENT_H
#define VIARGO_MULTI_TOUCH_EVENT_H

#include <vector>
#include <time.h>
#include <algorithm>

#include "core/event.h"
#include "core/vrgmath.h"

#include "core/metaphor.h"
#include "core/device.h"
#include "core/propertylist.h"

namespace viargo {
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/**
 * Base class for RAW multi-touch data. For Viargo it can
 * be seen as a black box for basic low-level multi-touch 
 * information from a connected surface
 * 
 * Notes:
 * - Although oriented on TUIO protocol, the event class is
 * not limited to it, and should be used for all kind of multi-touch
 * input sources
 *
 */
class RawMultiTouchEvent : public Event {
public:
	// Invalid session or fiducial id for a touch item
    static const int INVALID_ID = -1;
	// ...
    static const int INVALID_FRAME = -1;

	// Type of the touch item: "cursor" is ment for finger input or pointing 
	// devices, "object" - for fiducial objects.
    enum ObjectType {
        INVALID_OBJECT_TYPE,
        CURSOR_2D,
        CURSOR_25D,
        CURSOR_3D,
        OBJECT_2D,
        OBJECT_25D,
        OBJECT_3D
    };

	// ...
    enum Action {
        INVALID_ACTION,	// ...
        FRAME,			// End of the current interaction frame.
        APPEARED,		// A new touch item has appeared on the surface
        DISAPPEARED,	// Touch item was removed from the surface
        UPDATED			// The properties of a touch item has changed
    };

    // Constuctor
    RawMultiTouchEvent( std::string source,		                  // sender name (i.e. TouchWall, iPhone, etc.)         
						ObjectType  type,                         // type of the object
						Action      act,                          // action performed
						long        sessionID       = INVALID_ID, // id of the item for this session   
						long        fidID           = INVALID_ID, // object-typ id
						vec3        position        = vec3f(0.f), // new position
						float       angle           = 0,          // rotation angle (if any)
						vec3        motionSpeed     = vec3f(0.f), // speed
						float       rotationSpeed   = 0,          // rotational speed
						float       motionAccel     = 0,          // acceleration
						float       rotationAccel   = 0           // rotational acceleration
						)
		:Event(source),
		_sessionID(sessionID), 
		_fiducialID(fidID),
		_position(position), 
		_motionSpeed(motionSpeed),  
		_motionAccel(motionAccel),
		_angle(angle), 
		_rotationSpeed(rotationSpeed),
		_rotationAccel(rotationAccel),  
		_type(type), 
		_action(act)
	{
    }

    // Destuctor
	virtual ~RawMultiTouchEvent() {}

    // Getter
    long sessionID() const          {return _sessionID;}
    long fiducalID() const          {return _fiducialID;}

    // Getter
    vec3f position() const          {return _position;}
    vec3f motionSpeed() const       {return _motionSpeed;}
    float motionAccel() const       {return _motionAccel;}

    // Getter
    float angle() const             {return _angle;}
    float rotationSpeed() const     {return _rotationSpeed;}
    float rotationAccel() const     {return _rotationAccel;}

    // Getter
    ObjectType objectType() const   {return _type;}
    Action action() const           {return _action;}

    // Getter
    bool isCursor() const   {return (_type == CURSOR_2D || _type == CURSOR_25D || _type == CURSOR_3D);}
    bool isObject() const   {return !isCursor();}
    bool is2D() const       {return (_type == OBJECT_2D ||  _type == CURSOR_2D);}
    bool is3D() const       {return (_type == OBJECT_3D ||  _type == CURSOR_3D);}
    bool is25D() const      {return (_type == OBJECT_25D || _type == CURSOR_25D);}
 
private: 
    long _sessionID;
    long _fiducialID;

    vec3f _position;      //[0, 1]x[0, 1]x[0, 1]          z = 0.0f if 2D
    vec3f _motionSpeed;   //[0, inf]x[0, inf]x[0, inf]    z = 0.0f if 2D or 2.5D
    float _motionAccel;   //[-inf, inf]                   z = 0.0f if 2D or 2.5D

    float _angle;         //[0, 2Pi]      = 0.0f if cursor
    float _rotationSpeed; //[0, inf]      = 0.0f if cursor
    float _rotationAccel; //[-inf, inf]   = 0.0f if cursor

    ObjectType _type;
    Action _action;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/**
* Basic data struct for a touch data at specific time-point
*/
struct TouchInfo {

	/// Default constructor
	TouchInfo() 
		:timeStamp((float)clock())
	{
	}

	/// Parameter constructor
	TouchInfo(unsigned long blobId, vec3f blobPosition, float blobAngle = 0.0f, vec2f blobSize = vec2f::zero) 
		:rawId(blobId),
		timeStamp((float)clock()),
		position(blobPosition),
		speed(vec3f::zero),
		acceleration(0.0f),
		angle(blobAngle),
		rotationSpeed(0.0f),
		rotationAcceleration(0.0f),
		size(blobSize)
	{
	}
	
	unsigned long rawId;	// <- current hardware id of the tracked blob
	float timeStamp;		// <- timeStamp for this state
	
	vec3f position;			// <- position in virtualScreen coordinates ([-1, 1] x [-1, 1])
	vec3f speed;			// <- speed in virtualScreen coordinates
	float acceleration;		// <- acceleration along the speed vector

	float angle;			// <- orientation in radians (if any)	
	float rotationSpeed;	// <- rotational speed (if any)
	float rotationAcceleration; // <- rotational acceleration (if any)

	// geometry properties
	vec2f size;				// <- axis-aligned bounding rect of the blob
	// float angle			// <- orientation of the main axis of the blob
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/**
* Basic data struct to represent a touch point and some of it's history
*/
struct Touch {

	/// NIL touch
	static Touch& nil;

	/// The state of the corresponding raw touch point 
	enum RawTouchState {
		RTS_FORESEEN,		// <- a touch is detected, but still not sure 
							// if it is a noise or stable touch

		RTS_ALIVE,			// <- stable touch

		RTS_ZOMBIE,			// <- touch disappeared; the system is scanning 
							// for new RAW touches close to the last ALIVE point 

		RTS_DEAD			// no new touch appeared within the treashold time
							// close to the last ALIVE position. The touch will
							// be removed in the next iteration.
	};

	/// Default constructor
	Touch()
		:state(RTS_FORESEEN),
		id(INVALID_ID)
	{
	}

	/// Parameter constructor
	Touch(TouchInfo initialData, int classID = 0, int userID = 0)
		:state(RTS_FORESEEN),
		id(INVALID_ID),
		classId(classID),
		userId(userID),
		initial(initialData),
		current(initialData),
		tag()
	{
	}

	RawTouchState   state; // <- the state of the current RAW touch
	
	int id;			// <- unique id of this touch
	static const int INVALID_ID = -1; // <- invalid touch id
	
	int classId;	// <- id of the object class associated with this touch
					// ids 1 - 10 are reserved for finger identification (if possible)
					// 1 - thumb of the right hand
					// 2 - pointing finger of the right hand
					// ...
					// id == 0 is reserved for UNKNOWN
					// ids < 0 are reserved for simulated touches (mouse for instance)
					// all other ids are user-defined
	
	int userId;		// <- unique id of the user (if supported)
					// user-id = 0 is reserved for UNKNOWN

	TouchInfo initial;			// <- the parameters of the touch (pos, size, etc.) by the initial detection
	TouchInfo current;			// <- the current parameters of the touch 

	std::vector<TouchInfo> tag; // <- a touch produces a 'tag' while moving, i.e. this is a vector
								// containing a set of DIFFERENT previous parameter sets in timed order.

	PropertyList properties;	// <- list of named properties for the metaphors to communicate
								// with each other!

	// some usefull methods
	/// Returns the time in ms, since the touch was initially detected
	float timeSinceDetected() const {
		return timeSinceDetected((float)clock());
	}

	float timeSinceDetected(float stamp) const {
		return (1000.0f * ((stamp - initial.timeStamp) / CLOCKS_PER_SEC));
	}

	/// Returns the time in ms, since the touch has chenged it's position for the last time
	float timeSinceLastUpdate() const {
		return timeSinceLastUpdate((float)clock());
	}

	float timeSinceLastUpdate(float stamp) const {
		return (1000.0f * ((stamp - current.timeStamp) / CLOCKS_PER_SEC));
	}

	/// Returns the relative motion between the current and the last positions
	vec3f lastMotion() const {
		if (tag.empty())
			return vec3f::zero;
		return (current.position - tag[0].position);
	} 

	/// Returns the relative translation between the current state and the creation state
	vec3f displacement() const {
		return current.position - initial.position;
	}
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/**
* Data structure to hold all the touches on the surface (and their state).
*/
class TouchPool {

public:
	TouchPool() 
	{
	}

	~TouchPool() 
	{
	}

	bool exists(int id) const {				// true if a touch with ID exists
		return (_idMap.find(id) != _idMap.end());
	}
	const Touch& operator[] (int id) const { // get the operator with ID
											 // returns a dead touch if not found!
		IDMap::const_iterator it = _idMap.find(id);
		if (it != _idMap.end())
			return *(it->second);

		return Touch::nil;
	}
	int size() const {	 // get the number of touches
		return _idMap.size();
	}

	bool empty() const {	 // is there a touch detected
		return _idMap.empty();
	}

	// for (resetIterator(); hasNext(); ) {
	//	
	//		do something with the touch
	//}
	void resetIterator() {
		_idMapIterator = _idMap.begin();
	}

	bool hasNext() const {
		return _idMapIterator != _idMap.end();
	}

	Touch& next() {
		if (hasNext()) {
			Touch& result = *(_idMapIterator->second);
			++_idMapIterator;
			return result;
		}
		return Touch::nil;
	}
    
	typedef std::vector<int> IDList;
	IDList& updated() {
		return _updated;
	}

private:
	friend class TouchPoolManager; // <- this will be the class, that manages 
								  // the maps below

	// map of all touches which have a raw-id
	typedef std::map<unsigned long, Touch*> RawIDMap;
	RawIDMap _rawIdMap;
	
	// map of all touches which have an id
	typedef std::map<int, Touch*> IDMap;
	IDMap	 _idMap;
	IDMap::const_iterator _idMapIterator;

	// updated map
	IDList _updated;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/**
* Standard implementation of the RawTouchManager
*/
class TouchPoolManager {

public:
	// ctor
	TouchPoolManager(size_t tagSize = 10, float zombieDist = 0.01f) 
		:_tagSize(tagSize),
		_zombieDist(zombieDist)
	{
	}

	
	// methods
	void onAppeared(TouchPool& pool, const TouchInfo& touchInfo, int classID = 0, int userID = 0) {

		// serch in the undead map
		TouchPool::IDMap::iterator it = pool._idMap.begin();
		for ( ; it != pool._idMap.end(); ++it) {
			if (it->second->current.rawId == touchInfo.rawId) {
				// there is an alive touch with the same raw-id (zombies have no valid raw-id)
				// -> must be an error
				return;
			}
			// if the touch is a zombie - check for re-detection
			if (it->second->state == Touch::RTS_ZOMBIE) {
				Touch& zombie = *(it->second);
				// if the zombie had the same user and class ids
				if (zombie.userId == userID && zombie.classId == classID) {
					// if it is was at the same position
					if (distance(zombie.current.position, touchInfo.position) <= _zombieDist) {
						// update the zombie and make it alive
						zombie.current = touchInfo;
						zombie.state = Touch::RTS_ALIVE;
						pool._rawIdMap.insert(std::make_pair(zombie.current.rawId, it->second));

						// we are finished with this touch -> return
						return;
					}
				}
			}
		}

		// there was no appropriate zombie -> must be a new touch
		Touch* touch = new Touch(touchInfo, classID, userID);
		touch->state = Touch::RTS_FORESEEN; // ensure the state
		
		// still not sure what that is.. 
		pool._rawIdMap.insert(std::make_pair(touch->current.rawId, touch));
	}
	
	void onDisappeared(TouchPool& pool, unsigned long rawId) {
		// search into the living map
		TouchPool::RawIDMap::iterator it = pool._rawIdMap.find(rawId);
		if (it != pool._rawIdMap.end()) {
			Touch& touch = *(it->second);
			// if alive -> make it zombie
			if (touch.state == Touch::RTS_ALIVE) {
				touch.state = Touch::RTS_ZOMBIE;
				touch.current.rawId = Touch::INVALID_ID;
				pool._rawIdMap.erase(it);
			}
			// if it foreseen -> remove
			else if (touch.state == Touch::RTS_FORESEEN) {
				delete it->second;
				pool._rawIdMap.erase(it);
			}
			// no other case
			else {
				// ERROR!
			}
		}
	}
	
	void onUpdate(TouchPool& pool, const TouchInfo& touchInfo, int classID = 0, int userID = 0) {

		// if the touch is known (has rawID) then update
		TouchPool::RawIDMap::iterator it = pool._rawIdMap.find(touchInfo.rawId);
		if (it != pool._rawIdMap.end()) {
			Touch& touch = *(it->second);
			// if alive - filter the position and update the tag
			if (touch.state == Touch::RTS_ALIVE) {
				//TODO: if (filter(touch, touchInfo)) { 
					touch.tag.insert(touch.tag.begin(), touch.current);
					touch.current = touchInfo;
					while (touch.tag.size() > _tagSize) {
						touch.tag.pop_back();
					}
					pool._updated.push_back(touch.id);
				//}
			}
			else if (touch.state == Touch::RTS_FORESEEN) {
				touch.current = touchInfo;
			}
		}
		// if unknown i've probably missed the add message -> insert the touch
		else {
			onAppeared(pool, touchInfo, classID, userID);
		}
	}
	
	void onFrame(TouchPool& pool, float timeSinceLastFrame = -1.0f) {
	
		float minInsertionTime = 50.0f; // 50ms
		float maxZombieTime = 50.0f; // 50ms

		float timeStamp = (float)clock();

		// forall living
		TouchPool::RawIDMap::iterator it = pool._rawIdMap.begin();
		for ( ; it != pool._rawIdMap.end(); ++it) {
			Touch& touch = *(it->second);
			if (touch.state == Touch::RTS_FORESEEN && touch.timeSinceDetected(timeStamp) >= minInsertionTime) {
				// make it alive
				touch.state = Touch::RTS_ALIVE;
				touch.initial = touch.current;
				touch.id = generateId();
				pool._idMap.insert(std::make_pair(touch.id, it->second));
				pool._updated.push_back(touch.id);
			}
			else {
				// nothing to do if already alive
			}
		}

		// forall undead
		TouchPool::IDMap::iterator idit = pool._idMap.begin();
		for ( ; idit != pool._idMap.end(); /*NOP*/) {
			Touch& touch = *(idit->second);
			if (touch.state == Touch::RTS_ZOMBIE && touch.timeSinceLastUpdate(timeStamp) >= maxZombieTime) {
				pool._updated.push_back(touch.id);
				delete idit->second;
				
				TouchPool::IDMap::iterator next_idit = idit;
				++next_idit;
				pool._idMap.erase(idit);
				idit = next_idit;
			}
			else {
				++idit;
			}
		}

		// sort the ids in _updated
		std::sort(pool._updated.begin(), pool._updated.end());
		TouchPool::IDList::iterator newEnd = std::unique(pool._updated.begin(), pool._updated.end());
		pool._updated.resize(newEnd - pool._updated.begin());
	}

	// TODO: make it better
	int generateId() {
		static int _id_ = 1;
		return _id_++;
	}

	private:
		const size_t _tagSize;
		const float _zombieDist;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class MultiTouchEvent : public Event {

public:
	MultiTouchEvent(std::string deviceName, TouchPool& pool, std::vector<int> updated) 
		:Event(deviceName),
		_pool(pool),
		_changedIds(updated)
	{
	}

	/// Returns a pool of all touches currently detected 
	TouchPool& touches() {
		return _pool;
	}

	/// returns the ids of the touches changed since the last event
	std::vector<int> updated() const {
		return _changedIds;
	}

private:
	TouchPool& _pool;
	std::vector<int> _changedIds;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class RawTouchManager : public Metaphor, public Device {

public:
	RawTouchManager(std::string name, TouchPoolManager* manager = 0, size_t tagSize = 10, float zombieDistance = 0.01f) 
		:Metaphor(name, true),
		Device(name),
		_manager(manager) 
	{
		if (!_manager)
			_manager = new TouchPoolManager(tagSize, zombieDistance);
		Device::enable();
		Metaphor::enable();
	}
	
	~RawTouchManager() {
		delete _manager;
	}

	bool onEventReceived(Event* e) {
		if (Device::enabled() && Metaphor::enabled() && typeid(*e) == typeid(RawMultiTouchEvent))
			return true;
		
		return false;
	}
	
	void handleEvent(Event* e) {
		RawMultiTouchEvent& evt = *((RawMultiTouchEvent*)e); // safe

		int classID = (evt.fiducalID() == RawMultiTouchEvent::INVALID_ID ? 0 : ((int)evt.fiducalID() + 10));
		int userID  = 0;
		vec2f size  = vec2f::zero;

		switch (evt.action()) {
			// dummy.. should never appear
			case RawMultiTouchEvent::INVALID_ACTION:
				break;

			case RawMultiTouchEvent::FRAME:
				break;

			case RawMultiTouchEvent::APPEARED: {
				vec3f unifiedPos = vec3f(2.0f * evt.position().x - 1.0f, 
										 -(2.0f * evt.position().y - 1.0f), 
										 evt.position().z);
				
				TouchInfo touchInfo(evt.sessionID(), unifiedPos, evt.angle(), size);
				_manager->onAppeared(_touches, touchInfo, classID, userID);
				break;
			}

			case RawMultiTouchEvent::DISAPPEARED: {
				_manager->onDisappeared(_touches, evt.sessionID());
				break;
			}

			case RawMultiTouchEvent::UPDATED: {
				
				TouchInfo touchInfo;
				touchInfo.rawId = evt.sessionID();
				touchInfo.size  = size;

				touchInfo.position = vec3f(2.0f * evt.position().x - 1.0f, 
										 -(2.0f * evt.position().y - 1.0f), 
										 evt.position().z);
				
				touchInfo.speed = vec3f(2.0f * evt.motionSpeed().x, 
										-(2.0f * evt.motionSpeed().y), 
										evt.motionSpeed().z);
				
				touchInfo.acceleration = evt.motionAccel();
								
				touchInfo.angle	= evt.angle();
				touchInfo.rotationSpeed = evt.rotationSpeed();
				touchInfo.rotationAcceleration = evt.rotationAccel();
				
				_manager->onUpdate(_touches, touchInfo, classID, userID); 
				break;
			}
		}
	}
	
	void update(float timeSinceLastUpdate) {
		_manager->onFrame(_touches, timeSinceLastUpdate);
	
		if (!_touches.updated().empty()) {
			broadcastEvent(new MultiTouchEvent(Device::name(), _touches, _touches.updated()));
		}
		_touches.updated().clear();
	}

	TouchPool& touches() {
		return _touches;
	}

private:
	TouchPoolManager* _manager;
	TouchPool _touches;

};


} // namespace viargo
#endif // VIARGO_MULTI_TOUCH_EVENT_H
