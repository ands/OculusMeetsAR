#ifndef GLOWMATERIALLISTENER_H
#define GLOWMATERIALLISTENER_H

#include "OGRE/Ogre.h"
#include "OGRE/OgreMaterialManager.h"

class GlowMaterialListener : public Ogre::MaterialManager::Listener{
public:
    GlowMaterialListener(){
        mBlackMat = Ogre::MaterialManager::getSingleton().create("mGlowBlack", "Internal");
        mWhiteMat = Ogre::MaterialManager::getSingleton().create("mGlowWhite", "Internal");
        mBlackMat->getTechnique(0)->getPass(0)->setDiffuse(0,0,0,0);
        mBlackMat->getTechnique(0)->getPass(0)->setSpecular(0,0,0,0);
        mBlackMat->getTechnique(0)->getPass(0)->setAmbient(0,0,0);
        mBlackMat->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,0);
        mWhiteMat->getTechnique(0)->getPass(0)->setDiffuse(0.3f,0.3f,0.3f,0.3f);
        mWhiteMat->getTechnique(0)->getPass(0)->setSpecular(0.3f,0.3f,0.3f,0.3f);
        mWhiteMat->getTechnique(0)->getPass(0)->setAmbient(0.3f,0.3f,0.3f);
        mWhiteMat->getTechnique(0)->getPass(0)->setSelfIllumination(0.3f,0.3f,0.3f);
    }

    Ogre::Technique *handleSchemeNotFound(unsigned short, const Ogre::String& schemeName, Ogre::Material *mat, unsigned short, const Ogre::Renderable*){
        if(schemeName == "glowbig"){
            return mBlackMat->getTechnique(0);
        }
		if(schemeName == "glowsmall"){
            return mBlackMat->getTechnique(0);
		}
        if(schemeName == "geom"){
            return mBlackMat->getTechnique(0);
        }
        return nullptr;
    }

protected:
    Ogre::MaterialPtr mBlackMat;
    Ogre::MaterialPtr mWhiteMat;
};

#endif
