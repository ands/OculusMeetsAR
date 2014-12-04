#ifndef ARLIB_INFO_LOG_H
#define ARLIB_INFO_LOG_H

#include <fstream>

namespace ARLib{

class InfoLog{
public:
	InfoLog(const std::string& filename);
	~InfoLog();

	void log(const std::string& infoString);
private:
	std::ofstream *mFile;
};

};
#endif
