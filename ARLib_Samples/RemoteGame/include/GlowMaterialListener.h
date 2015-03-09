#ifndef GLOWMATERIALLISTENER_H
#define GLOWMATERIALLISTENER_H

#include "OGRE/OgreMaterialManager.h"

namespace Ogre
{
	typedef _StringBase String;
	class Renderable;
};

class GlowMaterialListener : public Ogre::MaterialManager::Listener
{
public:
    GlowMaterialListener();
    Ogre::Technique *handleSchemeNotFound(unsigned short, const Ogre::String& schemeName, Ogre::Material *mat, unsigned short, const Ogre::Renderable*);

protected:
    Ogre::MaterialPtr mBlackMat;
    Ogre::MaterialPtr mWhiteMat;
};

#endif
