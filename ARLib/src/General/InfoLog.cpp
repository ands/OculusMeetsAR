#include "InfoLog.h"

InfoLog::InfoLog(const std::string& filename){
	file = new std::ofstream(filename, std::ios::out);
}

InfoLog::~InfoLog(){
	file->close();
	delete file;
}

void InfoLog::log(const std::string& infoString){
	(*file) << infoString.c_str();
}