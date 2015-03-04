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

    enum FRAME_EVALUATION_METHOD{
        FRAME_NONE = 0x00, 
        FRAME_ROUND = 0x01,
        FRAME_INTERPOLATE_LINEAR = 0x02,
        FRAME_CEIL = 0x03,
        FRAME_FLOOR = 0x05
    };

    const unsigned int RIFT_BODY_ID = 1;

    struct TimedFrame{
        long long mTimestamp;
        RigidBody* mBody;
    };
	
	/**********************************************************************************************
	Base class for all operations on tracking frames.
	**********************************************************************************************/
    class FrameEvaluator{
	public:
		FrameEvaluator(unsigned int frameBufferSize = 0);
		~FrameEvaluator();

		//see TrackingManager::evaluateRigidBody
		RigidBody* evaluateRigidBody(unsigned int ID, const long long& retroActiveQueryTime);

		//notifys all EventListeners of a change in their respective positions and/or orientations
        virtual void evaluate() = 0;
		//see TrackingManager::addRigidBodyEventListener
		void addRigidBodyEventListener(RigidBodyEventListener* listener);

		//see TrackingManager::setEvaluationMethod
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

	
	/**********************************************************************************************
	Base class for all NatNet frame operations
	**********************************************************************************************/
	class GenericNatNetEvaluator : public FrameEvaluator{
	public:
		GenericNatNetEvaluator(unsigned int frameBufferSize);

		//retrieve the current tracking frame from the natnet server and save it to be queried "later"
		virtual void updateFrame(RBFrame *frame) = 0;
		virtual void evaluate();
	protected:
	};
	
	/**********************************************************************************************
	class for retrieving tracking frames from the natnet server and to supplement the data with 
	the inertial sensor data from the rifts.
	**********************************************************************************************/
	class NatNetRiftEvaluator : public GenericNatNetEvaluator{
	public:
		NatNetRiftEvaluator(unsigned int frameBufferSize = 0);
		~NatNetRiftEvaluator();

		//see GenericNatNetEvaluator::updateFrame
		virtual void updateFrame(RBFrame *frame);
	};
	
	/**********************************************************************************************
	class for retrieving tracking frames from the natnet server.
	**********************************************************************************************/
	class NatNetEvaluator : public GenericNatNetEvaluator{
	public:
		NatNetEvaluator(unsigned int frameBufferSize = 0);
		~NatNetEvaluator();

		//see GenericNatNetEvaluator::updateFrame
		virtual void updateFrame(RBFrame *frame);
	};
	
	/**********************************************************************************************
	Class for Analysing Rift sensor data und for constructing a tracking frame out of it
	**********************************************************************************************/
	class RiftEvaluator : public FrameEvaluator{
	public:
		RiftEvaluator(unsigned int frameBufferSize = 0);
		~RiftEvaluator();

		//retrieve sensory data and update listeners
		virtual void evaluate();
	private:
	};
};

#endif
