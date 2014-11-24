
#ifndef VIARGO_UDP_DEVICE_H
#define VIARGO_UDP_DEVICE_H

#include <vector>
#include <string>

#include "core/device.h"

namespace viargo {

// forward declarations
class UdpListener;
class TcpListener;

/**
* Base Network device Class
*/
class NetworkDevice : public Device {

public:

	// Specifies the type of connection
	enum ConnectionType {
		CT_TCP,
		CT_UDP
	};

	// Constructor
	NetworkDevice(std::string name, ConnectionType ct, std::string host, int port);

	// Destructor
	virtual ~NetworkDevice();

	std::string		host()			 const { return _host;}
	int				port()			 const { return _port;}
	ConnectionType	connectionType() const { return _connectionType; }
	bool			connected()		 const { return _connected; }

	// Interface to be implemented by subclasses
	virtual void parseData(const char* data, int size) = 0;

protected:
	std::string	   _host;
	int			   _port;

private:
	static std::vector<UdpListener*> _udpListeners;
	static std::vector<TcpListener*> _tcpListeners;

	ConnectionType _connectionType;
    bool _connected;
};

} // namespace viargo
#endif // VIARGO_UDP_DEVICE_H




