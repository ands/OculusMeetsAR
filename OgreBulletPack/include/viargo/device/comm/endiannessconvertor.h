
#ifndef VIARGO_ENDIANNESS_CONVERTOR
#define VIARGO_ENDIANNESS_CONVERTOR

#include "core/system.h"

namespace viargo{

class EndiannessConvertor{

public:

    enum Endianness{
        ED_LITTLE_ENDIAN,
        ED_BIG_ENDIAN
    };

    EndiannessConvertor(Endianness im)
        : inputEndianness_(im){
        systemEndianness_ = systemEndianness();
    }
    
    ~EndiannessConvertor(){
        // nothing to do here
    }

    inline Endianness systemEndianness(){
		return(System::isBigEndian() ? ED_BIG_ENDIAN : ED_LITTLE_ENDIAN);
    }

    template <typename T>
    static T swapBytes(T value){
	    
        int size = sizeof(T);
        if (size == 1) 
            return value;

	    unsigned char* data = (unsigned char*) &value;
        unsigned char tmp;

		for (int i = 0; i < (int)(size/2); ++i){
			tmp = data[i];
			data[i] = data[size - 1 - i];
			data[size - 1 - i] = tmp;
		}
        return (T)*data;
    }

    template <typename T>
    T convertValueToSystemEndianness(T value){
        
        if(inputEndianness_ != systemEndianness_)
            return swapBytes(value);

        return value;
    }

private:
    Endianness inputEndianness_;
    Endianness systemEndianness_;
};

} // namespace viargo
#endif // VIARGO_ENDIANNESS_CONVERTOR
//


