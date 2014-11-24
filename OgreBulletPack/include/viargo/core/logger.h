
#ifndef VIARGO_LOGGER_H
#define VIARGO_LOGGER_H

#include <sstream>
#include <fstream>
#include <map>

#include "core/singleton.h"
#include "core/system.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace viargo{
// FIXME:dimi: the name of this class is too short to avoid naming clashes!!!
class Log{

public:

    enum LogSeverity {
        LS_None,
        LS_Info,
        LS_Message,
        LS_Warning,
        LS_Error,
        LS_Critical
    };
        
    virtual void addMessage(LogSeverity logSeverity, std::ostringstream& oss) = 0;

    Log(LogSeverity mls);
    virtual ~Log();

protected:
    LogSeverity _minSeverity;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class ConsoleLog : public Log{

public:
    ConsoleLog(LogSeverity minSeverity = LS_Warning);
	~ConsoleLog();
    void addMessage(LogSeverity logSeverity, std::ostringstream& oss);

private:
	System::MutexHandle _mutex;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class FileLog : public Log{

public:
    FileLog(std::string filename, LogSeverity minSeverity = LS_Warning);
    ~FileLog();

    void addMessage(LogSeverity logSeverity, std::ostringstream& oss);

private:
    std::ofstream _file;
	System::MutexHandle _mutex;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class Logger{

public:

    Logger();
    ~Logger();
    
    void enableConsoleLog(Log::LogSeverity severity = Log::LS_Warning);
    void disableConsoleLog();

    void addFileLog(std::string filename, Log::LogSeverity severity = Log::LS_Warning);
    void removeFileLog(std::string filename);

    void addMessage(Log::LogSeverity severity, std::ostringstream& oss);

private:

    std::map<std::string, Log*> _logmap;
    Log* _console;

};

} // namespace viargo


// macros
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef WIN32
    #define LOG_FUNCTION __FUNCTION__
#else
    #define LOG_FUNCTION __PRETTY_FUNCTION__
#endif // WIN32

#ifdef VIARGO_LOG_DESCRIPTION
	#define LOG_DESCRIPTION " at: \n"<<LOG_FUNCTION<<" in "<<__FILE__<<":"<<__LINE__<<"\n"
#else
	#define LOG_DESCRIPTION "\n"
#endif

#if defined(WITHOUT_LOGGER)
#define LogInfo(msg){}
    #define LogMessage(msg){}
    #define LogWarning(msg){}
    #define LogError(msg){}
    #define LogCritical(msg){}
#else // with Logger
    #define LogError(msg)\
    {\
        std::ostringstream oss;\
        oss << "ERROR: " << msg << LOG_DESCRIPTION;\
        viargo::Singleton<viargo::Logger>::reference().addMessage(viargo::Log::LS_Error, oss);\
    }

    #define LogCritical(msg)\
    {\
        std::ostringstream oss;\
		oss << "CRITICAL: " << msg << LOG_DESCRIPTION;\
        viargo::Singleton<viargo::Logger>::reference().addMessage(viargo::Log::LS_Critical, oss);\
    }
    
    #if defined(MINIMAL_LOGGER)
        #define LogInfo(msg){}
        #define LogMessage(msg){}
        #define LogWarning(msg){}
    #else // normal Logger
        #define LogInfo(msg)\
        {\
            std::ostringstream oss;\
            oss<<"INFO: "<<msg<<"\n";\
            viargo::Singleton<viargo::Logger>::reference().addMessage(viargo::Log::LS_Info, oss);\
        }

        #define LogMessage(msg)\
        {\
            std::ostringstream oss;\
            oss<<"MESSAGE: "<<msg<<"\n";\
            viargo::Singleton<viargo::Logger>::reference().addMessage(viargo::Log::LS_Message, oss);\
        }

        #define LogWarning(msg)\
        {\
            std::ostringstream oss;\
			oss << "WARNING: " << msg << LOG_DESCRIPTION;\
            viargo::Singleton<viargo::Logger>::reference().addMessage(viargo::Log::LS_Warning, oss);\
        }
    #endif
#endif

#endif // VIARGO_LOGGER_H

