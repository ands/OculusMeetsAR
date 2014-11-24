
#ifndef VIARGO_DATA_PARSER_H
#define VIARGO_DATA_PARSER_H

#include <vector>

#include "core/system.h"

namespace viargo {

class Device;

/**
 * Abstract base class for parsing of packets sent by devices. For Viargo
 * it can be seen as a black box that transforms a device-dependant packet
 * into one or more events, which are then broadcasted via the EventProvider.
 * 
 * Notes:
 * - Packet types are distinguished via a magic number in a byte- or word-header
 * - Each DataParser implementation can handle one or more magic numbers
 */
class DataParser {
public:
	// Constructor
	DataParser() {}

	// Destructor
	virtual ~DataParser() {}

	// Returns a vector of all byte-sized magic numbers recognized 
	// by the parser
	virtual std::vector<System::byte> byteHeaders() {
		return std::vector<System::byte>();
	}

	// The same as byteHeaders, but for word-sized headers
	virtual std::vector<System::word> wordHeaders() {
		return std::vector<System::word>();
	}

	// Parses a data string and sends events through the EventProvider
	virtual void parse(const System::byte* data, int size, Device* provider) = 0;
};

} // namespace viargo
#endif // VIARGO_DATA_PARSER_H
