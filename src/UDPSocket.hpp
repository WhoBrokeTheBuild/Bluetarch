#ifndef UDP_SOCKET_HPP
#define UDP_SOCKET_HPP

#include "Socket.hpp"

#include "Endpoint.hpp"

class UDPSocket : public Socket
{
public:

    UDPSocket();

    bool Bind(const Endpoint& endpoint);

    ssize_t SendTo(uint8_t const * buffer, size_t length, const Endpoint& endpoint, int flags = 0);
    ssize_t ReceiveFrom(uint8_t * buffer, size_t length, Endpoint * endpoint, int flags = 0);

}; // class UDPSocket

#endif // UDP_SOCKET_HPP