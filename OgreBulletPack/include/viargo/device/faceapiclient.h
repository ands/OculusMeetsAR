#ifndef FACE_API_CLIENT
#define FACE_API_CLIENT

#include "tgt/event/eventhandler.h"
#include "tgt/modules/comm/datareceiver.h"
#include "trackingevent.h"

#include <vector>

namespace tgt{

//abstract
class FaceAPIClient : public Client{

public:

    explicit FaceAPIClient(EventHandler* eventHandler, float jitterEpsilon = 0.0005) //meter 
        : eventHandler_(eventHandler), jitterEpsilon_(jitterEpsilon){
    }

    //Client interface implementation
    void processData(const char* data, int size){
        TrackingEvent* event = parseData(data, size);
        
        if(event != 0)
            eventHandler_->broadcast(event);
    }

    //parse data
    TrackingEvent* parseData(const char* data, int size);
    
    //swaps the data (input in little endian) if the machine is big-endian
    template <typename T>
    inline static T swapIfBigEndian(T value){
	    
        int size = sizeof(T);
        if (size == 1 || !isBigEndian()) 
            return value;

	    unsigned char* data = (unsigned char*) &value;

		for (int i = 0; i < size/2; ++i){
			unsigned char tmp = data[i];
			data[i] = data[size-1 - i];
			data[size-1 - i] = tmp;
		}
        return (T)*data;
    }
    
    //checks if the machine is big-endian
    inline static bool isBigEndian(){
        volatile int check = 1;
        return !(*(char*)(&check));
    }

private:
    EventHandler* eventHandler_;
    const float jitterEpsilon_;
    vec3 lastPosition_;

};

}//namespace tgt

#endif //FACE_API_CLIENT

