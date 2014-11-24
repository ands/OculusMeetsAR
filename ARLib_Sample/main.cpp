#include "Ogre.h"
#include "OgreFrameListener.h"
#include "Compositor/OgreCompositorManager2.h"
#include "RTShaderSystem/OgreRTShaderSystem.h"
#include "Rift.h"

Ogre::Root* root;
Ogre::RenderWindow* window;
Ogre::RTShader::ShaderGenerator* shaderGenerator;
Ogre::SceneManager* sceneManager;
Ogre::SceneNode* roomNode, * bodyNode, * bodyTiltNode, * headNode;
/*Ogre::RenderWindow* mSmallWindow;
Ogre::Viewport* mViewportL, mViewportR;*/
//Rift* rift;

//Ogre::Camera* cameraLeft, * cameraRight;

/*void initializeRift()
{
	try {
		Rift::init();
		rift = new Rift(0, root, window, false);
		rift->setCameras(cameraLeft, cameraRight);
		setIPD(rift->getIPD());
	}
	catch (const char* e) {
		std::cout << ">> " << e << std::endl;
		rift = NULL;
	}
}*/

void createRoom()
{
	roomNode = sceneManager->getRootSceneNode()->createChildSceneNode(); // Ogre::SCENE_STATIC); // roomNode

	Ogre::SceneNode* cubeNode = roomNode->createChildSceneNode();
	Ogre::Entity* cubeEnt = sceneManager->createEntity("Cube.mesh");
	cubeEnt->getSubEntity(0)->setMaterialName("CubeMaterialRed");
	cubeNode->attachObject(cubeEnt);
	cubeNode->setPosition(1.0, 0.0, 0.0);
	Ogre::SceneNode* cubeNode2 = roomNode->createChildSceneNode();
	Ogre::Entity* cubeEnt2 = sceneManager->createEntity("Cube.mesh");
	cubeEnt2->getSubEntity(0)->setMaterialName("CubeMaterialGreen");
	cubeNode2->attachObject(cubeEnt2);
	cubeNode2->setPosition(3.0, 0.0, 0.0);
	cubeNode->setScale(0.5, 0.5, 0.5);
	cubeNode2->setScale(0.5, 0.5, 0.5);

	Ogre::SceneNode* cubeNode3 = roomNode->createChildSceneNode();
	Ogre::Entity* cubeEnt3 = sceneManager->createEntity("Cube.mesh");
	cubeEnt3->getSubEntity(0)->setMaterialName("CubeMaterialWhite");
	cubeNode3->attachObject(cubeEnt3);
	cubeNode3->setPosition(-1.0, 0.0, 0.0);
	cubeNode3->setScale(0.5, 0.5, 0.5);

	Ogre::Entity* roomEnt = sceneManager->createEntity("Room.mesh");
	roomEnt->setCastShadows(false);
	roomNode->attachObject(roomEnt);

	Ogre::Light* roomLight = sceneManager->createLight();
	roomLight->setType(Ogre::Light::LT_POINT);
	roomLight->setCastShadows(true);
	roomLight->setShadowFarDistance(30);
	roomLight->setAttenuation(65, 1.0, 0.07, 0.017);
	roomLight->setSpecularColour(.25, .25, .25);
	roomLight->setDiffuseColour(0.85, 0.76, 0.7);

	//roomLight->setPosition( 5, 5, 5 ); // TODO: andreas: set position

	roomNode->attachObject(roomLight);
}

/*void createCameras()
{
	cameraLeft = sceneManager->createCamera("LeftCamera"); cameraLeft->detachFromParent();
	cameraRight = sceneManager->createCamera("RightCamera"); cameraRight->detachFromParent();

	// Create a scene nodes which the cams will be attached to:
	bodyNode = sceneManager->getRootSceneNode()->createChildSceneNode(); // "BodyNode");
	bodyTiltNode = bodyNode->createChildSceneNode();
	headNode = bodyTiltNode->createChildSceneNode(); // "HeadNode");
	bodyNode->setFixedYawAxis(true); // don't roll!

	headNode->attachObject(cameraLeft);
	headNode->attachObject(cameraRight);

	// Position cameras according to interpupillary distance
	float dist = 0.05;
	//if (mRift->isAttached())
	//{
	//	// If possible, get the camera projection matricies given by the rift:
	//	mCamLeft->setCustomProjectionMatrix( true, mRift->getProjectionMatrix_Left() );
	//	mCamRight->setCustomProjectionMatrix( true, mRift->getProjectionMatrix_Right() );
	//	dist = mRift->getStereoConfig().GetIPD();
	//}
	cameraLeft->setPosition(-dist / 2.0f, 0.0f, 0.0f);
	cameraRight->setPosition(dist / 2.0f, 0.0f, 0.0f);

	cameraLeft->setFarClipDistance(50);
	cameraRight->setFarClipDistance(50);

	cameraLeft->setNearClipDistance(0.001);
	cameraRight->setNearClipDistance(0.001);

	//mHeadLight = mSceneMgr->createLight();
	//mHeadLight->setType(Ogre::Light::LT_POINT);
	//mHeadLight->setCastShadows( true );
	//mHeadLight->setShadowFarDistance( 30 );
	//mHeadLight->setAttenuation( 65, 1.0, 0.07, 0.017 );
	//mHeadLight->setSpecularColour( 1.0, 1.0, 1.0 );
	//mHeadLight->setDiffuseColour( 1.0, 1.0, 1.0 );
	//mHeadNode->attachObject( mHeadLight );

	bodyNode->setPosition(4.0, 1.5, 4.0);
	//mBodyNode->lookAt( Ogre::Vector3::ZERO, Ogre::SceneNode::TS_WORLD );

	Ogre::Light* light = sceneManager->createLight();
	light->setType(Ogre::Light::LT_POINT);
	light->setCastShadows(false);
	light->setAttenuation(65, 1.0, 0.07, 0.017);
	light->setSpecularColour(.25, .25, .25);
	light->setDiffuseColour(0.35, 0.27, 0.23);
	bodyNode->attachObject(light);
}*/

void createScene()
{
	sceneManager = root->createSceneManager(Ogre::ST_GENERIC, 1, Ogre::INSTANCING_CULLING_SINGLETHREAD);
	shaderGenerator->addSceneManager(sceneManager);

	Ogre::Camera* camera = sceneManager->createCamera("Camera");
	Ogre::CompositorManager2* compositorManager = root->getCompositorManager2();
	const Ogre::String workspaceName = "scene workspace";
	compositorManager->createBasicWorkspaceDef(workspaceName, Ogre::ColourValue::Black);
	compositorManager->addWorkspace(sceneManager, window, camera, workspaceName, true);

	sceneManager->setSkyBox(true, "Examples/MorningSkyBox");


	sceneManager->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.1f));
	//mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE); // TODO: andreas: configure shadows
	sceneManager->setShadowFarDistance(30);

	createRoom();
	//createCameras();
}

/*void setRiftPose(Ogre::Quaternion orientation, Ogre::Vector3 pos)
{
	headNode->setOrientation(orientation);
	headNode->setPosition(pos);
}

void setIPD(float IPD)
{
	cameraLeft->setPosition(-IPD / 2.0f, 0.0f, 0.0f);
	cameraRight->setPosition(IPD / 2.0f, 0.0f, 0.0f);
}*/

struct Listener : public Ogre::FrameListener, public Ogre::WindowEventListener
{
	bool running{ true };
	bool frameStarted(const Ogre::FrameEvent& evt) override
	{
		if (!running) return false;

		/*if (rift)
		{
			if (rift->update(evt.timeSinceLastFrame))
			{
				setRiftPose(rift->getOrientation(), rift->getPosition());
			}
			else {
				delete rift;
				rift = NULL;
			}
		}*/

		/*animState->addTime(evt.timeSinceLastFrame);*/

		return true;
	}
	bool windowClosing(Ogre::RenderWindow*) { running = false; return false; }
};

int CALLBACK WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show)
{
	root = new Ogre::Root("", "", "OgreLog.txt");
#ifdef _DEBUG
	Ogre::Root::getSingleton().loadPlugin("RenderSystem_GL_d");
	Ogre::Root::getSingleton().loadPlugin("Plugin_ParticleFX_d");
	//Ogre::Root::getSingleton().loadPlugin("Plugin_CgProgramManager_d");
#else
	Ogre::Root::getSingleton().loadPlugin("RenderSystem_GL");
	Ogre::Root::getSingleton().loadPlugin("Plugin_ParticleFX");
	//Ogre::Root::getSingleton().loadPlugin("Plugin_CgProgramManager");
#endif
	Ogre::RenderSystem* renderSys = root->getAvailableRenderers()[0];
	root->setRenderSystem(renderSys);

	renderSys->setConfigOption("Full Screen", "No");
	renderSys->setConfigOption("Video Mode", "800 x 600 @ 32-bit colour");

	window = root->initialise(true);

	Ogre::ResourceGroupManager& rm = Ogre::ResourceGroupManager::getSingleton();
	//rm.addResourceLocation(R"(../../../ogre/media/RTShaderLib)", "FileSystem", "Essential");
	rm.addResourceLocation(R"(../../../ogre/media/RTShaderLib/materials)", "FileSystem", "Essential");
	rm.addResourceLocation(R"(../../../ogre/media/RTShaderLib/GLSL150)", "FileSystem", "Essential");
	rm.initialiseResourceGroup("Essential");
	//rm.addResourceLocation(R"(../../../ogre/media/models)", "FileSystem", "General");
	rm.addResourceLocation(R"(../../../ogre/media/materials/scripts)", "FileSystem", "General");
	//rm.addResourceLocation(R"(../../../ogre/media/materials/textures)", "FileSystem", "General");
	rm.addResourceLocation(R"(../../../ogre/media/materials/programs/GLSL150)", "FileSystem", "General");
	rm.addResourceLocation(R"(../../../ogre/media/packs/cubemapsJS.zip)", "Zip", "General");
	rm.addResourceLocation(R"(media)", "FileSystem", "General");
	rm.initialiseResourceGroup("General");

	bool ok = Ogre::RTShader::ShaderGenerator::initialize();
	shaderGenerator = &Ogre::RTShader::ShaderGenerator::getSingleton();

	/*Ogre::SceneManager* pSceneMgr = root.createSceneManager(Ogre::ST_GENERIC, 1, Ogre::INSTANCING_CULLING_SINGLETHREAD);
	shaderGenerator.addSceneManager(pSceneMgr);

	Ogre::Camera* pCamera = pSceneMgr->createCamera("Camera");
	Ogre::CompositorManager2* pCompositorManager = root.getCompositorManager2();
	const Ogre::String workspaceName = "scene workspace";
	pCompositorManager->createBasicWorkspaceDef(workspaceName, Ogre::ColourValue::Black);
	pCompositorManager->addWorkspace(pSceneMgr, pRenderWin, pCamera, workspaceName, true);

	pSceneMgr->setSkyBox(true, "Examples/MorningSkyBox");*/

	/*pSceneMgr->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));
	Ogre::SceneNode *lightNode = pSceneMgr->getRootSceneNode()->createChildSceneNode();
	lightNode->setPosition(20, 80, 50);
	lightNode->attachObject(pSceneMgr->createLight());*/

	/*Ogre::Entity* head = pSceneMgr->createEntity("ogrehead.mesh");
	head->setName("Head");
	Ogre::SceneNode* headNode = pSceneMgr->getRootSceneNode()->createChildSceneNode();
	headNode->attachObject(head);*/

	/*for (int i = 0; i < 99; ++i)
	{
		Ogre::SceneNode* newNode = headNode->createChildSceneNode();
		Ogre::Entity* newEntity = pSceneMgr->createEntity("Cube.mesh");
		newEntity->setMaterialName("red");
		newNode->attachObject(newEntity);
	}*/

	createScene();

	Listener listener;
	Ogre::WindowEventUtilities::addWindowEventListener(window, &listener);
	Ogre::Root::getSingleton().addFrameListener(&listener);

	root->startRendering();

	return 0;
}