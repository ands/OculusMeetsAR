#ifndef _OGREBULLET_HEIGHTMAP_
#define _OGREBULLET_HEIGHTMAP_


#include "OgreBulletCollisionsPreRequisites.h"

#include "OgreBulletCollisionsShape.h"
#include "Debug/OgreBulletCollisionsDebugLines.h"

#include "BulletCollision/CollisionShapes/btTriangleCallback.h"
#include "Utils/OgreBulletConverter.h"

namespace OgreBulletCollisions
{
	class DebugHelper : public btIDebugDraw
	{
	public:

        DebugHelper(DebugLines *pLines)
            : m_pLines(pLines)
        { }

        ~DebugHelper()
        { }

        void drawLine(const btVector3 &from,
                      const btVector3 &to,
                      const btVector3 &color)
		{
            m_pLines->addLine(convert(from), convert(to));
		}

        void draw3dText(const btVector3 &, const char *)
        { }

        void drawContactPoint(const btVector3 &PointOnB,
                              const btVector3 &normalOnB,
                              btScalar distance,
                              int lifeTime,
                              const btVector3 &color)
		{
		}

        void reportErrorWarning(const char* warningString)
		{
		}

        void setDebugMode(int debugMode)
		{
		}

        int getDebugMode() const
		{
			return -1;
		}

	private:
        DebugLines *m_pLines;
	};

	class DebugTriangleDrawCallback : public btTriangleCallback
	{
	private:
        DebugHelper *mDebugHelper;
        btTransform  mTransform;
        btVector3	 mColor;

	public:
        DebugTriangleDrawCallback(DebugHelper *db,
                                  btTransform &bt,
                                  const btVector3 &color)
            : btTriangleCallback(),
              mDebugHelper(db),
              mTransform(bt),
              mColor(color)
        { }

        void processTriangle(btVector3 *triangle,
                             int partId,
                             int triangleIndex)
		{
            mDebugHelper->drawLine(*triangle,       *(triangle + 1), mColor);
            mDebugHelper->drawLine(*(triangle + 1), *(triangle + 2), mColor);
            mDebugHelper->drawLine(*(triangle + 2), *triangle,       mColor);
		}						 
	};

	class HeightmapCollisionShape : public CollisionShape
	{
	public:
        HeightmapCollisionShape(int width,
                                int length,
                                const Ogre::Vector3 &scale,
                                Ogre::Real *pHeightData,
                                bool bFlip);

		virtual ~HeightmapCollisionShape()
        { }

        bool drawWireFrame(DebugLines *wire,
                           const Ogre::Vector3 &pos = Ogre::Vector3::ZERO,
                           const Ogre::Quaternion &quat = Ogre::Quaternion::IDENTITY) const;
	};
}


#endif // _OGREBULLET_HEIGHTMAP_
