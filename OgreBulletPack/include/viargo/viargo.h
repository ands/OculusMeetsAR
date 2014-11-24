
#ifndef VIARGO_H
#define VIARGO_H

// Basic utils
#include "core/vrgmath.h"
#include "core/shape.h"
#include "core/singleton.h"
#include "core/system.h"
#include "core/logger.h"
#include "core/stringutils.h"
#include "core/timer.h"

// Core engine
#include "core/viargoengine.h"
#include "core/eventmanager.h"

// Event types
#include "event/sensorevent.h"
#include "event/sensorpositionevent.h"
#include "event/sensororientationevent.h"
#include "event/multitouchevent.h"
#include "event/wiimoteevent.h"
#include "event/timerevent.h"
#include "event/keyboardevent.h"
#include "event/mouseevent.h"

// Metaphors
#include "core/metaphor.h"

// Manipulators (AAL)
#include "core/camera.h"
#include "core/scene.h"

// Devices (HAL)
#include "device/tuiodevice.h"
#include "device/eosdevice.h"
#include "device/wiimotedevice.h"

// build
#define VIARGO_VERBOSE_BUILD

// library version
#define VIARGO_VERSION_MAJOR 0
#define VIARGO_VERSION_MINOR 4
#define VIARGO_VERSION_PATCH 3
#define VIARGO_VERSION_STRING "0.4.3"
#if defined(VIARGO_VERBOSE_BUILD)
	#pragma message("VIARGO >> VERSION " VIARGO_VERSION_STRING)
#endif

// bind the lib files (Visual Studio)
#if (_MSC_VER > 500 && !VIARGO_NO_AUTOLINK)
	#if defined(_DLL)
		#if defined(_DEBUG)
			#if defined(VIARGO_VERBOSE_BUILD)
				#pragma message("VIARGO >> linking to <viargod_mtdll.lib>")
			#endif
			#pragma comment (lib, "viargod_mtdll.lib")
		#else
			#if defined(VIARGO_VERBOSE_BUILD)
				#pragma message("VIARGO >> linking to <viargo_mtdll.lib>")
			#endif
			#pragma comment (lib, "viargo_mtdll.lib")
		#endif
	#else
		#if defined(_DEBUG)
			#if defined(VIARGO_VERBOSE_BUILD)
				#pragma message("VIARGO >> linking to <viargod_mt.lib>")
			#endif
			#pragma comment (lib, "viargod_mt.lib")
		#else
			#if defined(VIARGO_VERBOSE_BUILD)
				#pragma message("VIARGO >> linking to <viargo_mt.lib>")
			#endif
			#pragma comment (lib, "viargo_mt.lib")
		#endif
	#endif

	#pragma comment (lib, "ws2_32.lib")
	#pragma comment (lib, "Winmm.lib")
#endif

namespace viargo {
	void initialize(std::string filename = "./settings.xml");
	void initializeFromXmlString(std::string xmlString);
} // namespace viargo

#endif // VIARGO_H
