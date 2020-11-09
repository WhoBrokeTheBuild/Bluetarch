#include "TCPSocket.hpp"

#include <cerrno>
#include <cstring>

TCPSocket::TCPSocket(const IPAddress& addr, uint16_t port)
{
    Connect(Endpoint(addr, port));
}

TCPSocket::TCPSocket(int socket, int family)
    : Socket(socket, family)
{ }

bool TCPSocket::Connect(const Endpoint& endpoint)
{
    Close();

    if (!Open(endpoint.GetFamily(), SOCK_STREAM)) {
        return false;
    }

    auto[saddr, slen] = endpoint.GetSocketAddress();
    if (!saddr) {
        _error = "GetSocketAddress() failed to get sockaddr";
        Close();
        return false;
    }

    int result = connect(_socket, saddr, slen);
    if (result < 0) {
        _error = "connect() " + std::string(strerror(errno));
        Close();
        return false;
    }

    return true;
}

bool TCPSocket::Bind(const Endpoint& endpoint)
{
    Close();

    if (!Open(endpoint.GetFamily(), SOCK_STREAM)) {
        return false;
    }

    int tmp = 1;
    setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&tmp, sizeof(int));
    setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, (const char*)&tmp, sizeof(int));

    auto[saddr, slen] = endpoint.GetSocketAddress();

    int result = bind(_socket, saddr, slen);
    if (result < 0) {
        _error = "bind() " + std::string(strerror(errno));
        Close();
        return false;
    }

    return true;
}

bool TCPSocket::Listen()
{
    int result = listen(_socket, SOMAXCONN);
    if (result < 0) {
        _error = "listen() " + std::string(strerror(errno));
        Close();
        return false;
    }

    return true;
}

TCPSocket TCPSocket::Accept()
{
    Endpoint endpoint;

    auto[saddr, slen] = endpoint.GetSocketAddress();

    int newSocket = accept(_socket, saddr, &slen);
    if (newSocket < 0) {
        _error = "accept() " + std::string(strerror(errno));
        Close();
        return TCPSocket();
    }

    return TCPSocket(newSocket, endpoint.GetFamily());
}

ssize_t TCPSocket::Send(uint8_t const * buffer, size_t length, int flags /* = 0 */)
{
    ssize_t result = send(_socket, buffer, length, flags);
    
    if (result < 0 
        && result != EAGAIN 
        && result != EWOULDBLOCK) {
        _error = "send() " + std::string(strerror(errno));
        Close();
    }

    return result;
}

ssize_t TCPSocket::Receive(uint8_t * buffer, size_t length, int flags /* = 0 */)
{
    ssize_t result = recv(_socket, buffer, length, flags);
    
    if (result < 0 
        && result != EAGAIN 
        && result != EWOULDBLOCK) {
        _error = "recv() " + std::string(strerror(errno));
        Close();
    }

    return result;
}