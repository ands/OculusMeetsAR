#include <ARLib/Webcam/videoplayer.hpp>

namespace webcam
{

	VideoPlayer::VideoPlayer(int camNum)
	{
		camNumber=camNum;
		CCapture::CreateInstance(&cap);
	}

	VideoPlayer::~VideoPlayer()
	{
		close();
	}

	void VideoPlayer::playVideo()
	{
		//cam selection
		IMFActivate *temp = NULL;
		DeviceList list = DeviceList();
		list.EnumerateDevices();
		int count = list.Count();
		//todo: select the two C310 in right order
		bool firstfound=false;
		for(unsigned int i=0;i<list.Count();i++){
			temp=list.m_ppDevices[i];
			WCHAR *name=NULL;
			temp->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,&name,NULL);
			std::wstring displayname = name;
			std::string helpname="Logitech HD Webcam C310";
			std::wstring loginame(helpname.begin(),helpname.end());
			if(displayname.compare(0,19,loginame,0,19)==0 && displayname.compare(loginame)>=0){//Accepts "Logitech HD Webcam Cx" with x>=310
				if(camNumber==0){
					break;
				}
				else if(!firstfound){
					firstfound=true;
				}
				else if(firstfound){
					break;
				}
			}

		}
		//Start capturing
		cap->StartCapture(temp);
		//Wait for first imagesample
		while (mTexture.isNull())
		{
			if(SUCCEEDED(update())){
				break;
			}
		}
	}

	HRESULT VideoPlayer::update()
	{
		HRESULT check=E_FAIL;
		if(cap->somebufferexist){
			BYTE* sample = cap->getLastImagesample(&check);
			if(SUCCEEDED(check)){
				if (mTexture.isNull())
				{
					static int i = 0;
					mTexture = Ogre::TextureManager::getSingleton().createManual(
						"ARLibWebcam/VideoTexture" + Ogre::StringConverter::toString(++i),
						Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
						Ogre::TEX_TYPE_2D,
						1280,960,
						0,
						Ogre::PF_BYTE_BGR,
						Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
				}
				Ogre::PixelBox pb(1280,960, 1, Ogre::PF_BYTE_BGR, sample);
				Ogre::HardwarePixelBufferSharedPtr buffer = mTexture->getBuffer();
				buffer->blitFromMemory(pb);
			}
		}
		return check;
	}

	std::string VideoPlayer::getTextureName()
	{
		std::string name;
		if (!mTexture.isNull())
			name = mTexture->getName();
		return name;
	}

	int VideoPlayer::getVideoWidth()
	{
		int width=0;
		if (!mTexture.isNull())
			width = mTexture->getWidth();
		return width;
	}

	int VideoPlayer::getVideoHeight()
	{
		int height=0;
		if (!mTexture.isNull())
			height = mTexture->getHeight();
		return height;
	}

	void VideoPlayer::close()
	{
	}

}
