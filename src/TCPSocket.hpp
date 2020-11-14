#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP

#include "Socket.hpp"
#include "Endpoint.hpp"

class TCPSocket : public Socket
{
public:

    TCPSocket() = default;

    TCPSocket(const IPAddress& addr, uint16_t port);

    TCPSocket(int socket, int family);

    virtual ~TCPSocket() = default;

    bool Connect(const Endpoint& endpoint);

    bool Bind(const Endpoint& endpoint);

    bool Listen();

    TCPSocket Accept(Endpoint * endpoint);

    ssize_t Send(uint8_t const * buffer, size_t length, int flags = 0);

    ssize_t Receive(uint8_t * buffer, size_t length, int flags = 0);

private:

}; // class TCPSocket

#endif // TCP_SOCKET_HPP