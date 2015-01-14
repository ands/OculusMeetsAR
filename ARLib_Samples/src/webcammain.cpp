#include <iostream>
#include "WebcamApp.h"

extern "C" int main(int argc, char *argv[])
{
	bool showDebugWindow = true;
	for (int i = 0; i < argc; i++)
	{
		std::string arg(argv[i]);
		if (arg == "--no-debug")
			showDebugWindow = false;
		if (arg == "--help" || arg == "-h")
		{
			std::cout << "Available Commands:" << std::endl
				<< "\t--no-debug\tDisables the debug window." << std::endl
				<< "\t--help,-h\tShow this help message." << std::endl;
			exit(0);
		}
	}

	WebcamApp* app;
	try 
    {
		app = new WebcamApp(showDebugWindow);
    } 
	catch(Ogre::Exception& e) 
	{
        MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
    }
	delete app;

    return 0;
}
