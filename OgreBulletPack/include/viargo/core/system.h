
#ifndef VIARGO_SYSTEM_H
#define VIARGO_SYSTEM_H

#if defined(WIN32) // TODO: do we really need all socket headers here?
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <winsock.h>
#else // UNIX
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <sys/types.h>

namespace viargo{

//TODO:dimi: comment
class System{

public:
    inline static bool is64bit(){
        return (sizeof(int*) == 8);
    }

    inline static bool isBigEndian(){
        volatile int check = 1;
        return !(*(char*)(&check));
    }

	inline static void sleep(int ms){
		#ifdef WIN32
		Sleep((DWORD)ms);
		#else //unix
		sleep(ms);
		#endif
	}

    template <typename T>
    inline static T swapIfBigEndian(T value){

        int size = sizeof(T);
        if (size == 1 || !isBigEndian())
            return value;

	    uint8* data = (uint8*) &value;

		for (int i = 0; i < size/2; ++i){
			uint8 tmp = data[i];
			data[i] = data[size-1 - i];
			data[size-1 - i] = tmp;
		}
        return (T)*data;
    }

    //types
    #if defined(WIN32)
        typedef unsigned __int8     uint8;
        typedef unsigned __int16    uint16;
        typedef unsigned __int32    uint32;
        typedef unsigned __int64    uint64;

        typedef signed __int8       int8;
        typedef signed __int16      int16;
        typedef signed __int32      int32;
        typedef signed __int64      int64;

        typedef float               ieee754float;
        typedef double              ieee754double;
        typedef long double         ieee754ldouble;

    #else
        //TODO: check for 64 bit system
        typedef uint8_t      uint8;
        typedef uint16_t     uint16;
        typedef uint32_t     uint32;
        typedef uint64_t	 uint64;

        typedef int8_t	     int8;
        typedef int16_t      int16;
        typedef int32_t      int32;
        typedef int64_t      int64;

        typedef float               ieee754float;
        typedef double              ieee754double;
        typedef long double         ieee754ldouble;
    #endif //WIN32

    typedef uint8   byte;
    typedef uint16  word;
    typedef uint32  dword;
    typedef uint64  qword;

	inline static word makeWord(const char* string){
		byte wrd[2];
		wrd[0] = string[0]; wrd[1] = string[1];
		return *((word*)wrd);
	}

	// threading
	#ifdef WIN32

		class WIN32_ThreadFunctionWrapper{
			public:
				WIN32_ThreadFunctionWrapper(void* (*func)(void*), void* param)
					:_func(func),
					_param(param)
				{
				}

				~WIN32_ThreadFunctionWrapper(){}

				DWORD run(){
					#if defined(_MSC_VER)
						#pragma warning(push)
						#pragma warning(disable : 4311)
					#endif

					return reinterpret_cast<DWORD>(_func(_param));

					#if defined(_MSC_VER)
						#pragma warning(pop)
					#endif
				}

			private:
				void* (*_func)(void*);
				void* _param;
		};

		typedef CRITICAL_SECTION*   MutexHandle;
		typedef struct __ThreadHandle_struct__{
			__ThreadHandle_struct__(int /*unused*/)
				: threadHandle(0),
				threadId(0),
				threadWrapper(0)
			{
			}
			__ThreadHandle_struct__() {	}
			bool operator !() const {return threadHandle == 0;}
			HANDLE threadHandle;
			DWORD threadId;
			WIN32_ThreadFunctionWrapper* threadWrapper;
		} ThreadHandle;

		/// starts a thread
		static ThreadHandle createThread(void* (*threadFunction)(void*), void* parameter);
		/// kills the specified thread
		static void destroyThread(ThreadHandle&);

		/// creates and initializes a handle to a recursive mutex
		static inline MutexHandle createMutexHandle(){
			MutexHandle mutex = new CRITICAL_SECTION;
			InitializeCriticalSection(mutex);
			return mutex;
		}

		static inline void freeMutexHandle(MutexHandle mutex){
			DeleteCriticalSection(mutex);
			delete mutex;
		}

		static inline void lockMutex(MutexHandle mutex){
			EnterCriticalSection(mutex);
		}

		static inline void unlockMutex(MutexHandle mutex){
			LeaveCriticalSection(mutex);
		}
	#else // UNIX
		typedef pthread_mutex_t*   MutexHandle;
		typedef pthread_t ThreadHandle;

		/// starts a thread
		static ThreadHandle createThread(void* (*threadFunction)(void*), void* parameter);
		/// kills the specified thread
		static void destroyThread(ThreadHandle&);

		/// creates and initializes a handle to a recursive mutex
		static inline MutexHandle createMutexHandle(){

			MutexHandle mutex = new pthread_mutex_t;

			pthread_mutexattr_t attrib;
			pthread_mutexattr_settype(&attrib, PTHREAD_MUTEX_RECURSIVE);

			pthread_mutex_init(mutex, &attrib);

			return mutex;
		}

		static inline void freeMutexHandle(MutexHandle mutex){
			 pthread_mutex_destroy(mutex);
			 delete mutex;
		}

		static inline void lockMutex(MutexHandle mutex){
			 pthread_mutex_lock(mutex);
		}

		static inline void unlockMutex(MutexHandle mutex){
			pthread_mutex_unlock(mutex);
		}
	#endif // ifdef WIN32
};

} // namespace viargo

#endif // VIARGO_SYSTEM_H

