#include "OGRE/OgreSceneManager.h"
#include "ARLib/Ogre/RiftSceneNode.h"
#include "ARLib/Ogre/RenderTarget.h"

namespace ARLib {

RiftSceneNode::RiftSceneNode(Rift *_rift, Ogre::SceneManager *sceneManager, float zNear, float zFar, unsigned int rigidBodyID)
	: RiftRigidBodyEventListener(_rift, rigidBodyID)
	, rift(_rift)
{
	// create a virtual body node that the rift is correctly attached to
	Ogre::SceneNode *rootNode = sceneManager->getRootSceneNode();
	bodyNode = rootNode->createChildSceneNode("ARLib/Oculus/BodyNode");
	bodyNode->setFixedYawAxis(true);

	bodyTiltNode = bodyNode->createChildSceneNode();
	headCalibrationNode = bodyTiltNode->createChildSceneNode("ARLib/Oculus/HeadCalibrationNode");
	headNode = headCalibrationNode->createChildSceneNode("ARLib/Oculus/HeadNode");

	cameras[0] = sceneManager->createCamera("ARLib/Oculus/LeftCamera");
	cameras[1] = sceneManager->createCamera("ARLib/Oculus/RightCamera");

	headNode->attachObject(cameras[0]);
	headNode->attachObject(cameras[1]);

	cameras[0]->setNearClipDistance(zNear);
	cameras[1]->setNearClipDistance(zNear);

	cameras[0]->setFarClipDistance(zFar);
	cameras[1]->setFarClipDistance(zFar);

	// initialize the interpupillary distance
	float ipd = 0.064f;

	if (rift)
	{
		// set correct interpupillary distance
		ipd = rift->getInterpupillaryDistance();

		// set correct projections
		float aspectRatios[2], projections[2][16];
		rift->getProjections(
			zNear, zFar, true,
			&aspectRatios[0], projections[0],
			&aspectRatios[1], projections[1]);

		for (int eyeNum = 0; eyeNum < 2; eyeNum++)
		{
			cameras[eyeNum]->setAspectRatio(aspectRatios[eyeNum]);
	
			float *p = projections[eyeNum];
			cameras[eyeNum]->setCustomProjectionMatrix(true, Ogre::Matrix4(
				p[ 0], p[ 1], p[ 2], p[ 3],
				p[ 4], p[ 5], p[ 6], p[ 7],
				p[ 8], p[ 9], p[10], p[11],
				p[12], p[13], p[14], p[15]));
		}
	}

	cameras[0]->setPosition(-ipd/2.0f, -0.05f, 0.25f);
	cameras[1]->setPosition( ipd/2.0f, -0.05f, 0.25f);
}

RiftSceneNode::~RiftSceneNode()
{
}

void RiftSceneNode::setPitch(Ogre::Radian angle)
{
	bodyTiltNode->pitch(angle, Ogre::Node::TS_PARENT);
}

void RiftSceneNode::setYaw(Ogre::Radian angle)
{
	bodyNode->yaw(angle);
}

void RiftSceneNode::addRenderTarget(RenderTarget *renderTarget)
{
	assert(renderTarget);
	renderTarget->setCameras(cameras[0], cameras[1]);

	// set visibility masks for new viewports
	cameras[0]->getViewport()->setVisibilityMask(VISIBILITY_FLAG_LEFT);
	cameras[1]->getViewport()->setVisibilityMask(VISIBILITY_FLAG_RIGHT);

	renderTargets.push_back(renderTarget);
}

void RiftSceneNode::removeRenderTarget(RenderTarget *renderTarget)
{
	assert(renderTarget);
	for(std::vector<RenderTarget*>::iterator it = renderTargets.begin(); it != renderTargets.end(); it++)
	{
		if (*it == renderTarget)
		{
			renderTargets.erase(it);
			break;
		}
	}

	// TODO: remove cameras from renderTarget!
}

void RiftSceneNode::removeAllRenderTargets()
{
	renderTargets.clear();
	// TODO: remove cameras from renderTargets!
}

void RiftSceneNode::onChange(const RigidBody *rb)
{
	headNode->setOrientation(rb->mqW, rb->mqX, rb->mqY, rb->mqZ);
	bodyNode->setPosition(rb->mX, rb->mY, rb->mZ);

	// calibrate (the rift is rotated 180 degrees)
	headCalibrationNode->setOrientation(Ogre::Quaternion(0, 0, 1, 0) * Ogre::Quaternion(mRefQW, mRefQX, mRefQY, mRefQZ).UnitInverse());

	/*if (rift && renderTargets.size())
	{
		float s[2][16], e[2][16];
		rift->getTimewarpMatrices(s[0], e[0], s[1], e[1]);

		Ogre::Matrix4 ms[2], me[2];
		ms[0] = Ogre::Matrix4(
			s[0][0], s[0][1], s[0][2], s[0][3],
			s[0][4], s[0][5], s[0][6], s[0][7],
			s[0][8], s[0][9], s[0][10], s[0][11],
			s[0][12], s[0][13], s[0][14], s[0][15]);
		ms[1] = Ogre::Matrix4(
			s[1][0], s[1][1], s[1][2], s[1][3],
			s[1][4], s[1][5], s[1][6], s[1][7],
			s[1][8], s[1][9], s[1][10], s[1][11],
			s[1][12], s[1][13], s[1][14], s[1][15]);
		me[0] = Ogre::Matrix4(
			e[0][0], e[0][1], e[0][2], e[0][3],
			e[0][4], e[0][5], e[0][6], e[0][7],
			e[0][8], e[0][9], e[0][10], e[0][11],
			e[0][12], e[0][13], e[0][14], e[0][15]);
		me[1] = Ogre::Matrix4(
			e[1][0], e[1][1], e[1][2], e[1][3],
			e[1][4], e[1][5], e[1][6], e[1][7],
			e[1][8], e[1][9], e[1][10], e[1][11],
			e[1][12], e[1][13], e[1][14], e[1][15]);

		for (int i = 0; i < renderTargets.size(); i++)
			renderTargets[i]->SetTimewarpMatrices(ms[0], me[0], ms[1], me[1]);
	}*/
}

}; // ARLib namespace