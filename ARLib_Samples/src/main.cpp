// ----------------------------------------------------------------------
// Visualization of Patient data (Dicoms and surface models) using the
// Oculus Rift.
// ---------------------------------------------------------------------
// August 2014, Micha Pfeiffer
// Couples Ogre3D and the Oculus Rift SDK to render Meshes in a VR setting
//

#include <iostream>
#include "Globals.h"
#include "App.h"

extern "C" {

    int main(int argc, char *argv[])
    {
		for ( int i = 0; i < argc; i++ )
		{
			std::string arg( argv[i] );
			if( arg == "--rotate-view" )
			{
				ROTATE_VIEW = true;
			}
			if( arg == "--no-rift" )
			{
				NO_RIFT = true;
			}
			if( arg == "--no-debug" )
			{
				DEBUG_WINDOW = false;
			}
			if( arg == "--help" || arg == "-h" )
			{
				std::cout << "Available Commands:" << std::endl
					<< "\t--rotate-view\tChanges the orientation of the main render window. Useful when your computer can't rotate the screen." << std::endl
					<< "\t--no-rift\tFor debugging: disable the Oculus Rift." << std::endl
					<< "\t--no-debug\tDisables the debug window." << std::endl
					<< "\t--help,-h\tShow this help message." << std::endl;
				exit(0);	// show help and then close app.
			}
		}

		// Creates the main program and starts rendering. When a framelistener
		// returns false, this will return.
		App* app;

		try 
        {
			app = new App();
        } catch(Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
#endif
        }

		delete app;

        return 0;
    }
}
