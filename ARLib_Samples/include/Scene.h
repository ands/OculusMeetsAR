#ifndef SCENE_H
#define SCENE_H

#include "OGRE/Ogre.h"
#include "OIS/OIS.h"
#include "Oculus/RiftNode.h"

class Scene
{
	public:
		Scene(Ogre::Root* root, OIS::Mouse* mouse, OIS::Keyboard* keyboard);
		~Scene();

		Ogre::SceneManager* getSceneMgr() { return mSceneMgr; }
		ARLib::RiftNode* getRiftNode() { return mRiftNode; }

		void update(float dt);

		// Keyboard and mouse events:
		bool keyPressed(const OIS::KeyEvent&);
		bool keyReleased(const OIS::KeyEvent&);
		bool mouseMoved(const OIS::MouseEvent&);
		bool mousePressed(const OIS::MouseEvent&, OIS::MouseButtonID);
		bool mouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID);

	private:
		void createRoom();

		Ogre::Root* mRoot;
		OIS::Mouse* mMouse;
		OIS::Keyboard* mKeyboard;
		Ogre::SceneManager* mSceneMgr;
		Ogre::SceneNode* mRoomNode;
		ARLib::RiftNode* mRiftNode;
};

#endif
