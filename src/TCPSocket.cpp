#include "TCPSocket.hpp"

#include <cerrno>
#include <cstring>

TCPSocket::TCPSocket(const IPAddress& addr, uint16_t port)
{
    Connect(addr, port);
}

bool TCPSocket::Connect(const Endpoint& endpoint)
{
    Close();

    if (!Open(endpoint.GetFamily(), SOCK_STREAM)) {
        return false;
    }

    if (_family != endpoint.GetFamily()) {
        _error = "Mismatched family";
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

bool TCPSocket::Connect(const IPAddress& addr, uint16_t port)
{
    return Connect(Endpoint(addr, port));
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