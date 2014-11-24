
#ifndef VIARGO_BUFFERED_QUEUE_H
#define VIARGO_BUFFERED_QUEUE_H

#include <deque>
#include "core/system.h"

namespace viargo{

template <typename T>
class BufferedQueue{

private:
    std::deque<T> _q1;
    std::deque<T> _q2;

    std::deque<T>* _in;
    std::deque<T>* _out;

	System::MutexHandle _lock;

public:
    BufferedQueue()
		:_in(&_q1), 
		_out(&_q2) 
	{
		_lock = System::createMutexHandle();
    }

    ~BufferedQueue(){
		System::freeMutexHandle(_lock);
    }

    inline void enqueue(T& elem){
		System::lockMutex(_lock);
        _in->push_back(elem);
		System::unlockMutex(_lock);
    }
    
    inline void dequeue(){
        _out->pop_front();
    }

    inline T&   front(){
        return _out->front();
    }

    inline bool hasNext(){
        return _out->size() > 0;
    }

	inline std::vector<T> getOutQueue() const{
		std::vector<T> result;
		result.insert(result.begin(), _out->begin(), _out->end());
		return result;
	} 

    inline void clearIn(){
        System::lockMutex(_lock);
        _in->clear();
		System::unlockMutex(_lock);
    }

    inline void clearOut(){
        _out->clear();
    }

    inline void swap(){
        System::lockMutex(_lock);
        std::deque<T>* temp = _in;
        _in = _out;
        _out = temp;
		System::unlockMutex(_lock);
    }
};

} // namespace viargo

#endif // VIARGO_BUFFERED_QUEUE_H


