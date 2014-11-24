#ifndef VIARGO_BINARY_DATA_READER_H
#define VIARGO_BINARY_DATA_READER_H

#include <string.h>
#include "core/system.h"
#include "endiannessconvertor.h"

namespace viargo{

class BinaryDataReader{

public:

	BinaryDataReader(const System::byte* source, int size, 
        EndiannessConvertor::Endianness endianness = EndiannessConvertor::ED_LITTLE_ENDIAN)
        : size_(size), readError_(false), ec_(endianness){
        
			cursor_ = buffer_ = new System::byte[size];
            memcpy(buffer_, source, size);
    }
    
    ~BinaryDataReader(){
        delete [] buffer_;
    }

    bool good() const{
        return cursor_ < buffer_ + size_;
    }

    bool readError() const{
        return readError_;
    }

    int size() const{
        return size_;
    }

    void read(char* destination, int size){
        
        memcpy(destination, cursor_, size);
        cursor_ += size;
    }
    
    inline bool   readBool8(){
		System::byte result;
		read((char*)&result, sizeof(System::byte));
        return result != 0;
    }
    
#define READIMPL(type){ \
    type result;\
    read((char*)&result, sizeof(type)); \
    return ec_.convertValueToSystemEndianness(result); \
    }

    inline char   readChar()   READIMPL(char);
    inline int    readInt()    READIMPL(int);
    inline long   readLong()   READIMPL(long);
    inline float  readFloat()  READIMPL(float);
    inline double readDouble() READIMPL(double);

#undef READIMPL

private:

    int size_;
    bool readError_;
    EndiannessConvertor ec_;

    System::byte* buffer_;
    System::byte* cursor_;
};

} // namespace viargo
#endif // VIARGO_BINARY_DATA_READER_H

