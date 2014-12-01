#ifndef ARLIB_FRAME_EVALUATOR_H
#define ARLIB_FRAME_EVALUATOR_H

/***************************************************
This class manages the client application, which communicates with the Motive stream server,
and the Oculus Rift Handle
***************************************************/

#include "Tracking/RigidBodyEventListener.h"

namespace ARLib{

	class FrameEvaluator{
	public:
		FrameEvaluator();
		~FrameEvaluator();
		void evaluate();
		void updateFrame(RBFrame *frame);
		void registerRigidBodyEventListener(RigidBodyEventListener* listener);
	private:
		RBFrame *mLastFrame;
		RBFrame *mCurrentFrame;
		std::vector<RigidBodyEventListener*> mRigidBodies;
	};
};

#endif
