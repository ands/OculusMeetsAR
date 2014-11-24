
#ifndef VIARGO_SINGLETON_H
#define VIARGO_SINGLETON_H

namespace viargo{

template <typename T>
class Singleton{

public:

    inline static void initialize(T* instance){
        delete _instance;
        _instance = instance;
    }

    inline static void deinitialize(){
        delete _instance;
        _instance = 0;
    }

    inline static T* pointer() {
        return _instance;
    }

	inline static T* ptr() {
        return pointer();
    }

    inline static T& reference() {
        return *_instance;
    }

	inline static T& ref() {
        return reference();
    }

protected:
    static T* _instance;
};

//init the static instance
template <typename T> T* Singleton<T>::_instance = 0;

} // namespace viargo

#endif // VIARGO_SINGLETON_H

