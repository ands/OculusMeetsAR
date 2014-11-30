#include "General/InfoLog.h"

namespace ARLib{

InfoLog::InfoLog(const std::string& filename){
	mFile = new std::ofstream(filename, std::ios::out);
}

InfoLog::~InfoLog(){
	mFile->close();
	delete mFile;
}

void InfoLog::log(const std::string& infoString){
	(*mFile) << infoString.c_str();
}

};