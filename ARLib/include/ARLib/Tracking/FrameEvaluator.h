#ifndef ARLIB_FRAME_EVALUATOR_H
#define ARLIB_FRAME_EVALUATOR_H

/***************************************************
This class manages the client application, which communicates with the Motive stream server,
and the Oculus Rift Handle
***************************************************/

#include "ARLib/Tracking/RigidBodyEventListener.h"
#include "ARLib/Oculus/Rift.h"
#include "tinythread.h"
#include <utility>
#include <vector>
#include <map>


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

		RigidBody* evaluateRigidBody(unsigned int ID, const long long& retroActiveQueryTime);
        virtual void evaluate() = 0;
		void addRigidBodyEventListener(RigidBodyEventListener* listener);
        void setEvaluationMethod(FRAME_EVALUATION_METHOD evalMethod);
	protected:
        unsigned int mFrameBufferSize;
        FRAME_EVALUATION_METHOD mEval;
        tthread::mutex mMutex;

		//create histories for certain rigid Bodies
		std::map<unsigned int, TimedFrame*> mRigidBodyHistories;
		std::vector<RigidBodyEventListener*> mRigidBodies;
		std::vector<std::pair<Rift*, unsigned int> > mRifts;
		RBFrame *mCurrentFrame;
	};

	class GenericNatNetEvaluator : public FrameEvaluator{
	public:
		GenericNatNetEvaluator(unsigned int frameBufferSize);
		virtual void updateFrame(RBFrame *frame) = 0;
	protected:
	};

	class NatNetRiftEvaluator : public GenericNatNetEvaluator{
	public:
		NatNetRiftEvaluator(unsigned int frameBufferSize = 0);
		~NatNetRiftEvaluator();

		virtual void evaluate();
		virtual void updateFrame(RBFrame *frame);
	};

	class NatNetEvaluator : public GenericNatNetEvaluator{
	public:
		NatNetEvaluator(unsigned int frameBufferSize = 0);
		~NatNetEvaluator();

		virtual void evaluate();
		virtual void updateFrame(RBFrame *frame);
	};

	class RiftEvaluator : public FrameEvaluator{
	public:
		RiftEvaluator(unsigned int frameBufferSize = 0);
		~RiftEvaluator();

		virtual void evaluate();
	private:
	};
};

#endif
