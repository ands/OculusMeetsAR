
#ifndef VIARGO_CAMERA_H
#define VIARGO_CAMERA_H

#include <string>
#include <vector>

#include "core/vrgmath.h"

/**
 * Class representing a virtual camera of the application.
 *
 * Notes:
 *
 * Todo:
 * - do we need master/slave camera concept?
 * - test it all !!!
 */
namespace viargo{

class Camera {

public:

	// Simple view point description struct
	struct Viewpoint {
		vec3f position; // The camera position
		vec3f target;   // The point at which the camera looks
		vec3f up;       // up vector
	};

	// Simple projection description struct
	struct Frustum {
		float left, right, bottom, top, zNear, zFar; // parameters of the viewing frustum
		bool ortho; // true for orthographic projections
	};

	// Simple stereo description struct
	struct Stereo {
		enum StereoType {
			ST_ON_AXIS,
			ST_OFF_AXIS
		};
		StereoType stereoType; // type of the stereo
		float iod;			   // eye separation
	};

	// Display rect description class
	class VirtualDisplay {
		friend class Camera;
		public:
			vec3f center()     const;
			vec2f dimensions() const;
			
			vec3f up()        const;
			vec3f strafe()    const;
			vec3f direction() const;

			// coord -> [-1.0; 1.0] x [-1.0; 1.0] y-up, x-right; 
			// parallax - distance to virtual screen in scene units; neg->in-front; pos->behind
			vec3f unprojectPoint(vec2f displayPoint)   const;
			vec3f unprojectVector(vec2f displayVector, float parallaxPlane = 0.0f) const;
			float parallax(vec3f scenePoint) const;
			ray3f cameraRay(vec2f displayPoint, bool isStereo = false, float isLeftDominant = false, float alpha = 0.4f) const;
		
		private:
			// Constructor
			VirtualDisplay();
			
			// Destructor
			~VirtualDisplay();
			const Camera* _camera;
	};

	// Rpocedural class for calculating the lookAt and Frustum parameters
	// for the left and the right eye from the camera settings
	class Eye {
		friend class Camera;
		public:
			// returns the position of the eye
			vec3f position() const;
			
			// returns the attitude of the eye
			quatf attitude() const;

			// gives the lookAt parameters for the accordant eye
			void lookAt(vec3f& eye, vec3f& at, vec3f& upv) const;

			// gives the lookAt parameters for the accordant eye as 3 x float[3]
			void lookAt(float* eye, float* at, float* upv, Math::Handedness csh = Math::CSH_RIGHT) const;

			// gives the frustum parameters for the accordant eye
			void frustum(float& l, float& r, float& b, float& t, float& n, float& f) const;

			// returns the projection matrix for this eye
			mat4f projectionMatrix() const;

			// returns the projection matrix for this eye as float[16]
			void projectionMatrix(float* pm, Math::Handedness csh = Math::CSH_RIGHT,
				Math::MatrixOrder mo = Math::MO_ROWCOL) const;

		private:
			// Describes which eye is represented
			enum EyeDescriptor {
				LEFT,
				RIGHT
			};

			// Constructor
			Eye(EyeDescriptor ed);
			// Destructor
			~Eye();

			EyeDescriptor _eye;
			const Camera* _camera;
	};

	// Constructor
	// @param: name	the name of the camera to be created
	Camera(std::string name = "[undefined]");

	// Destructor
	~Camera();

	// A nil camera
	static Camera& nil;

	// Tests two cameras for equality. The cameras are equal if they have
	// the same name; all other values doesent matter.
	// @param   camera: a reference to the camera to be compared with <code>this</code>
	// @returns true if <code>this</code> is equal to the camera passed as parameter
	// NOTE     since the cameras in viargo all have unique names this operator
	// only compare the names..
	bool operator == (const Camera& camera) const;

	// Tests two cameras for un-equality.
	// @see <code>operator ==(...)</code>
	bool operator != (const Camera& camera) const;

	// Returns the name of this camera
	std::string name() const;

	// @group stereo

	// Returns the eye separation for this camera (IOD)
	float eyeSeparation() const;

	// Sets the eye separation for this camera (IOD)
	// @param eyeSeparation: the new eye separation (IOD) to be set
	void  setEyeSeparation(float eyeSeparation);

	// Returns the focal length of this camera
	float focalLength() const;

	// Sets the the focal length of this camera
	// @param focal: the new focal length to be set
	void setFocalLength(float focal);


	// @group Translation

	// Moves the camera by a vector according to the camera's current orientation
	// @param step: the relative motion to be applied
	void move(vec3f step);

	// Moves the camera along its direction
	// @param step: the relative motion to be applied
	void moveForward(float step);

	// Moves the camera against its direction
	// @param step: the relative motion to be applied
	void moveBackward(float step);

	// Moves the camera along its up vector
	// @param step: the relative motion to be applied
	void moveUp(float step);

	// Moves the camera against its up vector
	// @param step: the relative motion to be applied
	void moveDown(float step);

	// Moves the camera along its right vector
	// @param step: the relative motion to be applied
	void moveRight(float step);

	// Moves the camera against its right vector
	// @param step: the relative motion to be applied
	void moveLeft(float step);


	// @group Rotation

	// Rotates the camera on 'angle' about 'axis'
	// @param axis:  the axis of rotation
	// @param angle: the angle of rotation (radians)
	void rotate(vec3f axis, float angle);

	// Rotates the camera on yaw-pith-roll according to its current position
	// @param yaw:     the yaw rotation (radians)
	// @param pitch:   the pitch rotation (radians)
	// @param roll:    the roll rotation (radians)
	// NOTE: note the sequence of application of the angles
	void rotate(float yaw, float pitch, float roll);

	// Rotates the camera with a matrix
	// @param rmx:  the rotation matrix to be applied
	void rotate(const mat4& rmx);

	// Rotates the camera with a quaternion
	// @param q:  the quaternion to be applied
	void rotate(quat q);


	// @group View Point

	// Returns the current position of the camera. If the camera is a
	// slave the returned position is relative to the position of the
	// master-camera.
	vec3f position() const;

	// Sets the position of the camera. If the camera is a slave,
	// the relative position according to the position of the master-camera is set.
	// @param pos: the new (relative) position to be set
	void setPosition(vec3f pos);

	// Returns the current target (focus) of the camera. If the camera is a
	// slave the returned target is relative to the position of the
	// master camera.
	vec3f target() const;

	// Sets the target (focus) of the camera. If the camera is a slave,
	// the relative target according to the position of the master-camera is set.
	// @param tar: the new (relative) target to be set
	void setTarget(vec3f tar);

	// Returns the up-vector of the camera
	vec3f up() const;

	// Sets the up vector of the camera
	// @param upv: the new up-vector to be set
	void setUp(vec3f upv);

	// Returns the strafe (right) vector of the camera, i. e.,
	// norm(dir x up)
	vec3f strafe() const;
	// no setter for this

	// Returns the direction vector of the camera, i. e.,
	// norm(target - position)
	vec3f direction() const;

	// Sets the direction of the camera
	// @param dir: the new direction to be set
	void setDirection(vec3f dir);

	// Returns the attitude of the camera as quad, i.e. the
	// rotation between (0, 0, -1) and dir
	quat attitude() const;

	// Sets the attitude of the camera
	// @param att: the new attitude to be set
	void setAttitude(quat att);

	// Returns the attitude of the camera as azimuth
	// and inclination
	void attitude(float& azimuth, float& inclination) const;

	// Sets the attitude of the camera
	// @param azimuth:		the new azimuth to be set
	// @param inclination:  the new inclination to be set
	void setAttitude(float azimuth, float inclination);

	// Returns the attitude of the camera as yaw, pitch, roll
	void attitude(float& yaw, float& pitch, float& roll) const;

	// Sets the attitude of the camera
	// @param yaw:		the new yaw to be set
	// @param pitch:	the new pitch to be set
	// @param roll:		the new roll to be set
	void setAttitude(float yaw, float pitch, float roll);

	// Returns the view matrix of the camera
	mat4f viewMatrix() const;

	// Sets the view matrix of the camera
	// @param vm: the new view matrix to be set
	void setViewMatrix(const mat4f& vm);

	// Gets the view matrix of the camera as float[16]
	// @param mx:	buffer to save the result (must be at least float[16])
	// @param csh:  coordinate system handedness, Math::CSH_RIGHT for right-handed coordinate system
	//											  or Math::CSH_LEFT for left-handed coordinate system.
	// @param mo:	matrix major order, Math::MO_ROWCOL or Math::MO_COLROW
	void  viewMatrix(float* mx, Math::Handedness csh = Math::CSH_RIGHT, Math::MatrixOrder mo = Math::MO_ROWCOL) const;

	// Sets the view matrix of the camera as float[16]
	// @param mx:	the new view matrix to be set (must be at least float[16])
	// @param csh:  coordinate system handedness, Math::CSH_RIGHT for right-handed coordinate system
	//											  or Math::CSH_LEFT for left-handed coordinate system.
	// @param mo:	matrix major order, Math::MO_ROWCOL or Math::MO_COLROW
	void setViewMatrix(float* mx, Math::Handedness csh = Math::CSH_RIGHT, Math::MatrixOrder mo = Math::MO_ROWCOL);

	// Gets the viewpoint parameters as lookAt
	// @param eye:	the position of the camera
	// @param at:	the target of the camera
	// @param upv:	the up vector of the camera
	void lookAt(vec3& eye, vec3& at, vec3& upv) const;

	// Sets the viewpoint as lookAt
	// @param eye:	the position of the camera
	// @param at:	the target of the camera
	// @param upv:	the up vector of the camera
	void setLookAt(const vec3& eye, const vec3& at, const vec3& upv);

	// alias of lookAt(vec3f&, vec3f&, vec3f&) with float[3]
	// @see lookAt, viewMatrix
	void lookAt(float* eye, float* at, float* upv, Math::Handedness csh = Math::CSH_RIGHT) const;

	// alias of setLookAt(vec3f, vec3f, vec3f) with float[3]
	// @see setLookAt, setViewMatrix
	void setLookAt(float* eye, float* at, float* upv, Math::Handedness csh = Math::CSH_RIGHT);

	// convenience method to get both the position and the attitude of the camera
	// @param pos:	the position of the camera
	// @param att:	the attitude of the camera as quaternion
	// @see position(), attitude()
	void positionAttitude(vec3& pos, quat& att) const;

	// convenience method to set both the position and the attitude of the camera
	// @param pos:	the position of the camera
	// @param att:	the attitude of the camera as quaternion
	// @see setPosition(...), setAttitude(...)
	void setPositionAttitude(vec3f pos, quat att);

	// alias of positionAttitude(vec3f&, quat&) with float[3/4]
	// @see positionAttitude(vec3f&, quat&), viewMatrix
	void positionAttitude(float* pos, float* quatAttitude, Math::Handedness csh = Math::CSH_RIGHT) const;

	// alias of setPositionAttitude(vec3f, quat) with float[3/4]
	// @see setPositionAttitude(vec3f, quat), setViewMatrix
	void setPositionAttitude(float* pos, float* att, Math::Handedness csh = Math::CSH_RIGHT);


	// @group Projection

	// Returns 'true' if the projection is orthographic
	bool isOrtho() const;

	// Returns 'true' if the projection is 2D orthographic,
	// i.e. zNear = -1; zFar = 1;
	bool isOrtho2D() const;

	// Returns 'true' if the projection is general perspective
	bool isFrustum() const;

	// Returns 'true' if the projection is symmetric perspective
	bool isPerspective() const;

	// Returns 'true' if the projection is symmetric (either perspective or orthographic)
	bool isSymmetric() const;

	// Returns the 'left' distance of the viewing frustum
	float left()   const;

	// Returns the 'right' distance of the viewing frustum
	float right()  const;

	// Returns the 'bottom' distance of the viewing frustum
	float bottom() const;

	// Returns the 'top' distance of the viewing frustum
	float top()    const;

	// Returns the 'near' distance of the viewing frustum
	float zNear()  const;

	// Returns the 'far' distance of the viewing frustum
	float zFar()   const;

	// Sets the 'left' distance of the viewing frustum
	// @param v: the new 'left' value to be set
	void setLeft(float v);

	// Sets the 'right' distance of the viewing frustum
	// @param v: the new 'right' value to be set
	void setRight(float v);

	// Sets the 'bottom' distance of the viewing frustum
	// @param v: the new 'bottom' value to be set
	void setBottom(float v);

	// Sets the 'top' distance of the viewing frustum
	// @param v: the new 'top' value to be set
	void setTop(float v);

	// Sets the 'near' distance of the viewing frustum
	// @param v: the new 'near' value to be set
	void setNear(float v);

	// Sets the 'far' distance of the viewing frustum
	// @param v: the new 'far' value to be set
	void setFar(float v);

	// Returns the FoVy of the camera
	// NOTE: the FoVy will be erroneous if the projection is not symmetric
	// NOTE: returns 0.0f for orthographic projections
	float fovy() const;

	// Sets a symmetric perspective projection with the current aspect ratio,
	// near, far distances and the given FoVy
	// @param fov: the FoVy to be set
	void setFovy(float fov);

	// Returns the diagonal FoV of the camera
	// NOTE: the FoV will be erroneous if the projection is not symmetric
	// NOTE: returns 0.0f for orthographic projections
	float diagFov() const;

	// Sets a symmetric perspective projection with the current aspect ratio,
	// near, far distances and the given diagonal FoV
	// @param fov: the diagonal FoV to be set
	void setDiagonalFov(float diagFov);

	// Returns the aspect ratio of the camera
	float aspectRatio() const;

	// Scales the left and the right values to fit the new aspect ratio
	// @param aspect: the new aspect ratio to be set
	void setAspectRatio(float aspect);

	// Gets the view frustum. The parameters are not changed, if the projection is orthographic.
	// @returns 'true' for perspective projections, 'false' otherwise
	// @param l, r, b, t, n, f: left, right, bottom, top, near, far distance of the view frustum
	bool frustum(float& l, float& r, float& b, float& t, float& n, float& f) const;

	// Sets a general perspective projection
	// @param l, r, b, t, n, f: left, right, bottom, top, near, far distance of the view frustum
	void setFrustum(float l, float r, float b, float t, float n, float f);

	// Gets the parameters of a symmetric perspective projection.
	// The parameters are not changed,  if the projection is orthographic or non-symmetric.
	// @returns 'true' for symmetric perspective projections, 'false' otherwise
	// @param fov:		the FoVy of the projection (radians)
	// @param ratio:	the aspect ration of the projection
	// @param n:		near plane distance
	// @param f:		far plane distance
	bool perspective(float& fov, float& ratio, float& n, float& f) const;

	// Sets a symmetric perspective projection.
	// @param fov:		the FoVy of the projection (radians)
	// @param ratio:	the aspect ration of the projection
	// @param n:		near plane distance
	// @param f:		far plane distance
	void setPerspective(float fov, float ratio, float n, float f);

	// Gets the parameters of the orthographic projection. The parameters are not changed,
	// if the projection is perspective.
	// @returns 'true' for orthographic projections, 'false' otherwise
	// @param l, r, b, t, n, f: left, right, bottom, top, near, far distance of the projection
	bool ortho(float& l, float& r, float& b, float& t, float& n, float& f) const;

	// Sets an orthographic projection
	// @param l, r, b, t, n, f: left, right, bottom, top, near, far
	void setOrtho(float l, float r, float b, float t, float n, float f);

	// Gets the parameters of the 2D orthographic projection.
	// The parameters are not changed, if the projection is not 2D ortho.
	// @returns 'true' for 2D orthographic projections, 'false' otherwise
	// @param l, r, b, t: left, right, bottom, top
	bool ortho2D(float& l, float& r, float& b, float& t) const;

	// Sets an 2D orthographic projection
	// @param l, r, b, t: left, right, bottom, top
	void setOrtho2D(float l, float r, float b, float t);

	// Returns a projection matrix.
	mat4f projectionMatrix() const;

	// Sets a general projection matrix. The element (4,4) of the matrix is checked
	// to determine if the projection is orthographic or perspective..
	// @param pm:	the matrix to be set
	// NOTE: only perspective and orthographic projections are currently supported
	void setProjectionMatrix(const mat4f& pm);

	// alias of projectionMatrix() with float[16]
	// @see projectionMatrix(), viewMatrix()
	void  projectionMatrix(float* mx, Math::Handedness csh = Math::CSH_RIGHT, Math::MatrixOrder mo = Math::MO_ROWCOL) const;

	// alias of setProjectionMatrix() with float[16]
	// @see setProjectionMatrix(), setViewMatrix()
	void  setProjectionMatrix(float* mx, Math::Handedness csh = Math::CSH_RIGHT, Math::MatrixOrder mo = Math::MO_ROWCOL);


	// @group Stereo Eyes

	// Inner object representing the left eye by stereo rendering
	// @use: get the left look-at param -> myCamera.leftEye.lookAt(eye, at, upv);
	// @use: get the left view-frustum  -> myCamera.leftEye.frustum(l, r, b, t, n, f);
	Eye leftEye;

	// Inner object representing the right eye by stereo rendering
	// @use: get the right look-at param -> myCamera.rightEye.lookAt(eye, at, upv);
	// @use: get the right view-frustum  -> myCamera.righttEye.frustum(l, r, b, t, n, f);
	Eye rightEye;

	// @group Virtual Display 
	// used to calculate scene elements defined in display space
	VirtualDisplay virtualDisplay;

private:
	// Camera parameters
	Viewpoint _viewpoint;
	Frustum   _frustum;
	Stereo    _stereo;

	// Name
	std::string _name;

	// Helper methods
	void projValues(float& l, float& r, float& b, float& t, float& n, float& f) const;
	void setProjValues(float l, float r, float b, float t, float n, float f);
};
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

} // namespace viargo
#endif // VIARGO_CAMERA_H
