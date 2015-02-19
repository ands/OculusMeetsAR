#ifndef ARLIB_FRAME_EVALUATOR_H
#define ARLIB_FRAME_EVALUATOR_H

/***************************************************
This class manages the client application, which communicates with the Motive stream server,
and the Oculus Rift Handle
***************************************************/

#include "ARLib/Tracking/RigidBodyEventListener.h"
#include "tinythread.h"
#include <vector>


namespace ARLib{

    typedef enum FRAME_EVALUATION_METHOD{
        FRAME_NONE = 0x00, 
        FRAME_ROUND = 0x01,
        FRAME_INTERPOLATE_LINEAR = 0x02,
        FRAME_CEIL = 0x03,
        FRAME_FLOOR = 0x05
    };

    const unsigned int RIFT_BODY_ID = 1;

    typedef struct _TimedFrame{
        long long mTimestamp;
        RigidBody* mBody;
    } TimedFrame;

    class FrameEvaluator{
	public:
		FrameEvaluator(unsigned int frameBufferSize = 0);
		~FrameEvaluator();

		RigidBody* evaluateRift(const long long& retroActiveQueryTime);
        void evaluate();
		void updateFrame(RBFrame *frame);
		void addRigidBodyEventListener(RigidBodyEventListener* listener);
        void setEvaluationMethod(FRAME_EVALUATION_METHOD evalMethod);
	private:
        unsigned int mFrameBufferSize;

        long long mFreq;
        TimedFrame *mRiftFrames;
		std::vector<RigidBodyEventListener*> mRigidBodies;
        FRAME_EVALUATION_METHOD mEval;

        RBFrame *mFrame;

        tthread::mutex mMutex;
	};
};

#endif
