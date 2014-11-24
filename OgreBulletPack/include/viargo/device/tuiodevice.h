
#ifndef VIARGO_TUIO_PARSER_H
#define VIARGO_TUIO_PARSER_H

#include <string>
#include <map>

#include "device/networkdevice.h"
#include "event/multitouchevent.h"
#include "core/system.h"
#include "core/tinyxml/tinyxml.h"

namespace osc {
class ReceivedBundle;
class ReceivedMessage;
class ReceivedMessageArgumentStream;
} // namespace osc

namespace viargo {

/**
 * Class for parsing of TUIO packets. For Viargo it can be seen as a black
 * box that broadcasts events via the EventProvider, which originate in Multi-
 * touch enabled devices.
 *
 * Notes:
 * - One parsed packet may result in multiple events
 */
class TuioDevice : public NetworkDevice {
public:
    /// Constructor
	/// @param: name - name of the multi-touch source
	/// @param: sendSysnchro - if the frame-Event should be send
	TuioDevice(std::string name, NetworkDevice::ConnectionType ct,
		std::string host, int port, bool flosc = false, bool sendSynchro = true);

    /// Destructor
	virtual ~TuioDevice();

	/// Implementation of NetworkDevice interface
	void parseData(const char* data, int size);

private:
    typedef std::map<int, bool> TouchIdMap;

	void parseUdp(const char* data, int size);
	void parseTcp(const char* data, int size);
	void parseFlosc(const char* data, int size);

    bool valid() const;
    void parseBundle(const osc::ReceivedBundle& bundle);
    void parseMessage(const osc::ReceivedMessage& message);
	void parseXMLMessage(TiXmlElement* elem);
	bool binBufferHasPacket();
	void parseBinaryMessage();
	void parseSetCommand(TouchIdMap* objectMap, RawMultiTouchEvent::ObjectType objectType,
		osc::ReceivedMessageArgumentStream& rmas);
	void parseXMLSetCommand(TouchIdMap* objectMap, RawMultiTouchEvent::ObjectType objectType, 
		TiXmlElement* elem);
	void parseAliveCommand(TouchIdMap* objectMap, RawMultiTouchEvent::ObjectType objectType,
		osc::ReceivedMessageArgumentStream& rmas);
	void parseXMLAliveCommand(TouchIdMap* objectMap, RawMultiTouchEvent::ObjectType objectType,
		TiXmlElement* elem);
	void parseFseqCommand(RawMultiTouchEvent::ObjectType objectType,
		osc::ReceivedMessageArgumentStream& rmas);
	void parseXMLFseqCommand(RawMultiTouchEvent::ObjectType objectType, TiXmlElement* elem);

	bool _flosc;
    bool _sendSynchro;

	// Use string buffer for xml packets and binBuffer for binaary packets
	std::string _stringBuffer;
	std::deque<viargo::System::byte> binBuffer;

	// magic string to identify the sender on his name
	std::string _magicString;

    TouchIdMap _objects;
    TouchIdMap _cursors;

    System::int64 _currentFrame;
    System::int64 _lastFrame;
};

} // namespace viargo
#endif // VIARGO_TUIO_PARSER_H
