#ifndef INFO_LOG_H
#define INFO_LOG_H

#include "..\Tracking\TrackingDll.h"
#include <fstream>

class TRACKINGDLL_API InfoLog{
public:
	InfoLog(const std::string& filename);
	~InfoLog();

	void log(const std::string& infoString);
private:
	std::ofstream file;
};

#endif
