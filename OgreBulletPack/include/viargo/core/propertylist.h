#ifndef VIARGO_PROPERTYLIST_H
#define VIARGO_PROPERTYLIST_H

#include <cstring>
#include <typeinfo>

#include "core/logger.h"

namespace viargo{

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// Service class for holding arbitrary property value
class PropertyValue {

public:
	// Constructor
	// @param value: the value to be saved
	template <typename T> PropertyValue(T value) {
		_valueptr = (void*)(new T(value));
		_typename = typeid(T).name();
		_size     = sizeof(T);
	}

	// Copy constructor
	PropertyValue(const PropertyValue& cp) {
		_typename = cp._typename;
		_size     = cp._size;
		_valueptr = (void*)(new unsigned char[_size]);
		memcpy(_valueptr, cp._valueptr, _size);
	}

	// Destructor
	virtual ~PropertyValue() {
		delete [] (unsigned char*)_valueptr;
	}

	// Assignment operator
	PropertyValue& operator=(const PropertyValue& cp) {
		_typename = cp._typename;
		_size     = cp._size;
		
		delete [] (unsigned char*)_valueptr;
		_valueptr = (void*)(new unsigned char[_size]);
		memcpy(_valueptr, cp._valueptr, _size);

		return *this;
	}

	// Gets the value saved in this instance
	// @param defaultValue: the value to be returned if the types don't match 
	// @NOTE: PropertyValue-s are strong typed, without implicit type conversions. The type of a PropertyValue 
	// is set by the initialisation and cannot be modified. All get(..) and set(..) calls will return an ERROR 
	// if the requestet type don't match the internal PropertyValue type.
	template <typename T> T get(T defaultValue) const {
		if (_typename != typeid(T).name()) {
			LogError("GET: Erroneous type specification: <" << typeid(T).name() 
				<< "> should be: <" << _typename << ">");
			return defaultValue;
		}
		else {
			return *((T*)_valueptr);
		}
	}

	// Modifies the value saved in this instance
	// @param value: the new value to be set 
	// @NOTE: @see: get(..)!
	template <typename T> void set(T value) {
		if (_typename != typeid(T).name()) {
			LogError("SET: Erroneous type specification: <" << typeid(T).name() 
				<< "> should be: <" << _typename << ">");
		}
		else {
			*((T*)_valueptr) = value;
		}
	}

	// Returns a string representing the type of the value saved in the instance.
	// NOTE: The returned value is a string as returned by typeid(value).name(), thus it may be
	// system specific and is not ment to be used as constant. For instance if (prop.type() == "float") 
	// is not guaraned to work. 
	// Use if (prop.type() == typeid(float).name()) instead.
	std::string type() const {
		return _typename;
	}

private:
	void*		_valueptr;
	int			_size;
	std::string _typename;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// Named list of arbitrary values to be used in the scene nodes.
class PropertyList {

public:
	// Constructor
	PropertyList() {}
	
	// Destructor
	~PropertyList() {}

	// Returns the Property's value by name
	// @param propertyName: The name of the property to be retreived.
	// @param defaultValue: The value to be reterned if a property with the name specified
	// was not found or an error has occured.
	// @NOTE: Properties are strong typed, without implicit conversions. The type of the 
	// value requested MUST match the type of the Property value!
	template <typename T> T    get(std::string propertyName, T defaultValue) const {
		PropertyMap::const_iterator it = _map.find(propertyName);
		if (it == _map.end()) {
			LogError("Could not find property with the name <" << propertyName 
						<< ">! Returning defaultValue!");
			return defaultValue;
		}

		return (it->second).get(defaultValue);
	}
	
	// Updates the value of the property specified by name. New property with the specified
	// name and value is created, if no property with this name exists. 
	// @param propertyName: The name of the property to be added/updated.
	// @param value:        The new value to be set.
	// @NOTE: Properties are strong typed, without implicit conversions. The type of the 
	// value requested MUST match the type of the Property value! If new propety is created
	// it takes the type of the value specified. No type modifications are allowed afterwards.
	template <typename T> void set(std::string propertyName, T value) {
		PropertyMap::iterator it = (_map.insert(std::make_pair(propertyName, PropertyValue(value)))).first;
		it->second.set(value);
	}
	
	// Removes property from the list
	// @param propertyName: name of the property to be removed.
	void remove(std::string propertyName) {
		PropertyMap::iterator it = _map.find(propertyName);
		if (it != _map.end()) {
			_map.erase(it);
		}
		else {
			LogWarning("Could not find property with the name <" << propertyName << ">!");
		}
	}
	
	// Checks for a Property with the given name
	// @param name: the name of the property we are searching for
	bool hasProperty(std::string propertyName) const {
		PropertyMap::const_iterator it = _map.find(propertyName);
		return (it != _map.end());
	}

private:
	// set of named properties
	typedef std::map<std::string, PropertyValue> PropertyMap;
	PropertyMap _map;
};	

} // namespace viargo
#endif // VIARGO_PROPERTYLIST_H

