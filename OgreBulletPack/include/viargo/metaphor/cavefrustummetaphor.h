
#ifndef VIARGO_CAVE_FRUSTUM_METAPHOR_H
#define VIARGO_CAVE_FRUSTUM_METAPHOR_H

#include <string>
#include <vector>
#include <cassert>

#include "core/core.h"

namespace viargo{

const float wallWidth  = 2.44f;
const float wallHeight = 1.88f;
const float ww2 = wallWidth / 2.0f;
const float wh2 = wallHeight / 2.0f;
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class ProjectionWallDescription {
	
public:
	// Default constuctor
	ProjectionWallDescription()
		:_width(0),
		_height(0),
		_cameraName(""),
		_transform(mat4f::identity)
	{
	}

	// Constuctor
	// NOTE: width and height in meters
	ProjectionWallDescription(std::string cameraName, float width, float height, mat4f transform)
		:_width(width),
		_height(height),
		_cameraName(cameraName),
		_transform(transform)
	{
		assert(_width > 0);
		assert(_height > 0);
	}

	// Constuctor
	// NOTE: width and height in meters
	ProjectionWallDescription(std::string cameraName, float width, float height, 
							  vec3f translation, float yaw, float pitch, float roll)
		:_width(width),
		_height(height),
		_cameraName(cameraName)
	{
		assert(_width > 0);
		assert(_height > 0);

		_transform = mat4f::translation(translation)
					* mat4::rotationZ(roll)
					* mat4f::rotationX(pitch)
					* mat4f::rotationY(yaw);
	}
	
	// Destructor
	~ProjectionWallDescription(){
	}

	// Getter
	float width()  const {return _width;}
	float height() const {return _height;}
	std::string cameraName() const {return _cameraName;}

	// Transforms a point from tracking coordinates to wall coordinates
	inline vec3f transformTrackingPosition(vec3f trackingPosition){
		return _transform * trackingPosition;
	}

	// Sets the transformation from tracking coordinate to geometrie coordinate 
	void addTrackerTransformation(mat4f trackerTransformation){
		_transform = _transform * trackerTransformation;
	}

private:
	float _width;
	float _height;

	std::string _cameraName;
	mat4f _transform;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
typedef std::vector<ProjectionWallDescription> ProjectionWallDescriptionList;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CaveFrustumMetaphor : public RenderingMetaphor {

public:
	
	// Constructor
	CaveFrustumMetaphor(const ProjectionWallDescriptionList& pwdl,  
		const mat4f& trackerTransformation = mat4f::identity)
		:RenderingMetaphor(),
		_currentTrackingPosition(),
		_wallDescriptionList(pwdl)
	{
		//setEventManagmentPolicy(EMP_PROCESS_INCOMMING);
		//setLockPolicy(LP_LOCK_NONE);

		ProjectionWallDescription description;
		foreach(description, _wallDescriptionList) {
			description.addTrackerTransformation(trackerTransformation);
		}
	}

	// Constructor
	CaveFrustumMetaphor(const ProjectionWallDescriptionList& pwdl, const vec3f& translation,
		float yaw, float pitch, float roll, vec3f scale)
		:RenderingMetaphor(),
		_currentTrackingPosition(),
		_wallDescriptionList(pwdl)
	{
		mat4f trackerTransformation = mat4f::translation(translation)
									* mat4::rotationZ(roll)
									* mat4f::rotationX(pitch)
									* mat4f::rotationY(yaw)
									* mat4f::scale(scale);
		
		ProjectionWallDescription description;
		foreach(description, _wallDescriptionList) {
			description.addTrackerTransformation(trackerTransformation);
		}
	}

	// Destructor
	~CaveFrustumMetaphor(){
	}

	// Handle incomming events
	void handleEvent(Event* event){

        if (typeid(*event) == typeid(SensorPositionEvent)) {
			SensorPositionEvent* spe = (SensorPositionEvent*)event;
			if (spe->sensorId() == 0) { // FIXME:dimi: EOS sends always 0
				_currentTrackingPosition = spe->pos();
			}
		}
		else if (typeid(*event) == typeid(SensorOrientationEvent)) {
			// just do nothing.. so far
		}
	}

	// Frame update
	void update(float /*timeSinceLastUpdate*/){
		// Avoid threading issues (if any)
		vec3f currentTrackingPosition = _currentTrackingPosition;

		// Correct the frustum for each wall/camera
		ProjectionWallDescription description;
		foreach(description, _wallDescriptionList) {
			// wall's half dimensions
			float wd2 = description.width() / 2.0f;
			float hd2 = description.height() / 2.0f;

			// Tracking position in wall-coordinate
			vec3f pos = description.transformTrackingPosition(currentTrackingPosition);

			// Get the current frustum
			float left, right, bottom, top, zNear, zFar;
			Viargo.camera(description.cameraName()).projectionMatrix().asFrustum(
				left, right, bottom, top, zNear, zFar);

			// Calculate the new frustum values 
			right  = zNear * ((wd2 - pos.x) / pos.z);
			left   = -zNear * ((wd2 + pos.x) / pos.z);

			top    = zNear * ((hd2 - pos.y) / pos.z);
			bottom = -zNear * ((hd2 + pos.y) / pos.z);

			// Set the new frustum
			Viargo.camera(description.cameraName()).setProjectionMatrix(
				viargo::projectionMatrixFromFrustum(left, right, bottom, top, zNear, zFar));
		}
	}

private:
	vec3f _currentTrackingPosition;
	ProjectionWallDescriptionList _wallDescriptionList;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class SimpleHeadTrackingMetaphor : public Metaphor {

public:
	// Constructor
	SimpleHeadTrackingMetaphor(const mat4f& trackerTransformation)
		:Metaphor(),
		_currentTrackingPosition(),
		_lastTrackingPosition(),
		_trackerTransformation(trackerTransformation),
		_initialized(false)
	{
		//setEventManagmentPolicy(EMP_PROCESS_INCOMMING);
		//setLockPolicy(LP_LOCK_NONE);
	}

	// Destructor
	~SimpleHeadTrackingMetaphor(){
	}

	// Handle incomming events
	void handleEvent(Event* event){

        if (typeid(*event) == typeid(SensorPositionEvent)) {
			SensorPositionEvent* spe = (SensorPositionEvent*)event;
			if (spe->sensorId() == 0) { // FIXME:dimi: EOS sends always 0
				_lastTrackingPosition = _currentTrackingPosition;
				_currentTrackingPosition = spe->pos();

				if (!_initialized) {
					_lastTrackingPosition = _currentTrackingPosition;
					_initialized = true;
				}
			}
		}
		else if (typeid(*event) == typeid(SensorOrientationEvent)) {
			// just do nothing.. so far
		}
	}

	// Frame update
	void update(){
		// Avoid threading issues (if any)
		vec3f currentTrackingPosition = _currentTrackingPosition;
		//vec3f lastTrackingPosition = _lastTrackingPosition;
		
		//float factor = Viargo.camera("main").eyeSeparation() / (6.5f / 100.0f); // 6.5cm

		//vec3f offset = currentTrackingPosition - lastTrackingPosition;
		//offset = _trackerTransformation * offset;

        //vec3f pos, dir, up, strafe;
        //Viargo.camera("main").viewMatrix().asVectorSystem(pos, dir, up, strafe);
		
		vec3 eye	 = currentTrackingPosition;
		vec3 target  = vec3(eye.x, eye.y, -ww2);
		vec3 upv	 = vec3(0, 1, 0);
        
		Viargo.camera("main").setViewMatrix(viargo::viewMatrixFromLookAt(eye, target, upv));
        
		// Correct the focal length
		Viargo.camera("main").setFocalLenght(distance(eye, target));
        Viargo.camera("right").setFocalLenght(distance(eye, vec3(ww2, eye.y, eye.z)));
		Viargo.camera("left").setFocalLenght(distance(eye, vec3(-ww2, eye.y, eye.z)));
	}

private:
	vec3f _currentTrackingPosition;
	vec3f _lastTrackingPosition;

	mat4f _trackerTransformation;

	float _initialized;
};

} // namespace viargo
#endif // VIARGO_CAVE_FRUSTUM_METAPHOR_H
