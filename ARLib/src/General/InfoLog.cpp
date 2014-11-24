#include "InfoLog.h"

InfoLog::InfoLog(const std::string& filename){
	file.open(filename, std::ios::out);
}

InfoLog::~InfoLog(){
	file.close();
}

void InfoLog::log(const std::string& infoString){
	file << infoString.c_str();
}