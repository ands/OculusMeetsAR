
#ifndef VIARGO_MULTI_TOUCH_CAMERA_METAPHOR_H
#define VIARGO_MULTI_TOUCH_CAMERA_METAPHOR_H

#include <vector>
#include <algorithm>

#include "viargo.h"
#include "event/multitouchevent.h"

namespace viargo {

class MTNavigation : public Metaphor{

public:

	MTNavigation(std::string name)
		:Metaphor(name){
	}
	
	~MTNavigation(){
	}

	std::vector<vec2f> touchPoints() const {
		return _oldPoints;
	}

	void handleEvent(Event* event) {
		if (typeid(*event) == typeid(RawMultiTouchEvent)) {
			RawMultiTouchEvent* mte = (RawMultiTouchEvent*)event;

			if (!mte->is2D())
				return;

			// remove callibration errors
			if (mte->position() == vec3f(0.0f, 0.0f, 0.0f)) {
				if (mte->action() == RawMultiTouchEvent::APPEARED)
					return; 
				if (mte->action() == RawMultiTouchEvent::UPDATED)
					return;
			}

			std::vector<Touch>::iterator it;
			Touch current(mte->sessionID(), mte->position());
			it = std::find(_touches.begin(), _touches.end(), current);

			switch (mte->action()) {
				case RawMultiTouchEvent::APPEARED: {
					if (it == _touches.end() && _touches.size() < 2)
						_touches.push_back(current);
					break;
				}

				case RawMultiTouchEvent::DISAPPEARED:{
					if(it != _touches.end())
						_touches.erase(it);
					break;
				}

				case RawMultiTouchEvent::UPDATED:{
					if(it != _touches.end())
						it->_pos = vec2f(mte->position().x, mte->position().y);
					else if (_touches.size() < 2) // no add event, but update -> add anyway
						_touches.push_back(current);
					else
						;
					break;
				}

				case RawMultiTouchEvent::INVALID_ACTION:
					break;

				case RawMultiTouchEvent::FRAME:
					break;
				
				default:
					break;
			}
		}
	}
	
	void update(float /*timeSinceLastUpdate*/) {

		// copy the positions
		std::vector<vec2f> points;
		for (size_t i = 0; i < _touches.size(); ++i) {
			points.push_back(_touches[i]._pos);
		}
		
		// clean up neighbourhood
		int cnt = 0;
		while (cnt < (int)points.size() - 1) {
			vec2f pt = points[cnt];
			int cnt2 = cnt + 1;
			while (cnt2 < (int)points.size()) {
				if (distance(points[cnt], points[cnt2]) <= 0.02f) {
					//std::cout << "\ndist (" << cnt << ", " << cnt2 << ") = " << distance(points[cnt], points[cnt2]);
					points.erase(points.begin() + cnt2);
				}
				else
					cnt2++;
			}
			cnt++;
		}

		// check what happens
		if (points.size() == 0) { // no touch
			_oldPoints.clear();
			return;
		}
		else if (points.size() == 1) { // single touch
			if (_oldPoints.size() == 1)
				singleFingerPan(points[0] - _oldPoints[0]);
		}
		else { // min. dual touch
			if (_oldPoints.size() > 1) {
				vec2f v1 = _oldPoints[1] - _oldPoints[0];
				vec2f v2 = points[1] - points[0];
				
				// make a zoom
				float l1 = length(v1);
				float l2 = length(v2);

				if (l1 > Math::epsilonf && Math::abs(l2 - l1) > 0.002f) {
					dualFingerZoom(l2 / l1);
				}

				// make a rotation
				float angle = Math::acos(dot(normalize(v1), normalize(v2)));
				float s = Math::sign(cross(vec3f(v1.x, v1.y, 0), vec3f(v2.x, v2.y, 0)).z);
				if (!(s != s) && !(angle != angle)) {
					vec2f center;
					if (distance(_oldPoints[1], points[1]) <= 0.0001f) {
						center = points[1];
					}
					else if (distance(_oldPoints[0], points[0]) <= 0.0001f) {
						center = points[0];
					}
					else {
						center = 0.5f * (points[0] + points[1]);
					}

					// make a rotation
					dualFingerRotate(center, s*angle);
				}
			}
		}

		_oldPoints = points;
	}

private:

	struct Touch {
		Touch(int id, vec3f pos) : _id(id), _pos(pos.x, pos.y){}
		bool operator==(const Touch& t){return _id == t._id;}
		int _id;
		vec2f _pos;
	};

	void singleFingerPan(vec2f translation) {

		Camera& camera = Viargo.camera("main");
		float h = (camera.focalLength() / camera.zNear()) * (camera.top() - camera.bottom());
		float w = -h * camera.aspectRatio();

		camera.move(vec3f(w * translation.x, h * translation.y, 0.0f));

		//std::cout << "\ntranslate dir: " << camera.direction() << " pos: " << camera.position();
	}
	
	void dualFingerZoom(float zoomFactor) {
		if (zoomFactor != zoomFactor) // NaN?
			return;
		
		if (zoomFactor > 1.5f || zoomFactor < 0.5) {
			std::cout << "\nfalse zoomFactor... ignoring!!!";
			return;
		}

		std::cout << "\nzoomFactor: " << zoomFactor;

		//const float zoomFactor = 200.0f;
		Camera& camera = Viargo.camera("main");

		//float translation = zoomFactor * ztrans;
		float translation = camera.focalLength() * (zoomFactor - 1.0f);
		// debug - we don't go through the wall, so that is not going to happen..
		translation = (translation >= camera.focalLength() ? 0.0f : translation);

		camera.moveForward(translation);
		camera.setFocalLength(camera.focalLength() - translation);

		//std::cout << "\nzoom dir: " << camera.direction() << " pos: " << camera.position();
	}

	void dualFingerRotate(vec2f center, float angle) {

		Camera& camera = Viargo.camera("main");
		float h = (camera.focalLength() / camera.zNear()) * (camera.top() - camera.bottom());
		float w = -h * camera.aspectRatio();
		
		vec3f c = vec3f(w*(center.x - 0.5f), h*(center.y-0.5f), 0.0f);
		
		camera.move(-c);
		//camera.rotate(-angle, 0.0f, 0.0f);
		camera.rotate(camera.direction(), -angle);
		camera.move(c);

		//std::cout << "\nrotate dir: " << camera.direction() << " pos: " << camera.position();
	}

	std::vector<Touch> _touches;
	std::vector<vec2f> _oldPoints;
};

} // namespace viargo
#endif // VIARGO_MULTI_TOUCH_CAMERA_METAPHOR_H


