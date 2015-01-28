#ifndef ARLIB_FRAME_EVALUATOR_H
#define ARLIB_FRAME_EVALUATOR_H

/***************************************************
This class manages the client application, which communicates with the Motive stream server,
and the Oculus Rift Handle
***************************************************/

#include "ARLib/Tracking/RigidBodyEventListener.h"
#include "tinythread.h"
#include <Windows.h>
#include <vector>


namespace ARLib{

    typedef enum FRAME_EVALUATION_METHOD{
        FRAME_NONE = 0x00, 
        FRAME_ROUND = 0x01,
        FRAME_INTERPOLATE_LINEAR = 0x02,
        FRAME_CEIL = 0x03,
        FRAME_FLOOR = 0x05
    };


    //TODO write a Mutex for Framebuffer!
    
    class FrameEvaluator{
	public:
		FrameEvaluator(unsigned int frameBufferSize = 0);
		~FrameEvaluator();
		void evaluate(float retroActiveQueryTime);
		void updateFrame(RBFrame *frame);
		void addRigidBodyEventListener(RigidBodyEventListener* listener);
        void setEvaluationMethod(FRAME_EVALUATION_METHOD evalMethod);
	private:
        unsigned int mFrameBufferSize;
        RBFrame **mFrames;
		std::vector<RigidBodyEventListener*> mRigidBodies;
        FRAME_EVALUATION_METHOD mEval;

        LARGE_INTEGER mStarttime;
        LARGE_INTEGER mFreq;
        tthread::mutex mMutex;
	};
};

#endif
