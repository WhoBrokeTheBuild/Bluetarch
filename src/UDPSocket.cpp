#include "UDPSocket.hpp"

#include <cerrno>
#include <cstring>

UDPSocket::UDPSocket()
{

}

bool UDPSocket::Bind(const Endpoint& endpoint)
{
    Close();

    if (!Open(endpoint.GetFamily(), SOCK_DGRAM)) {
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

ssize_t UDPSocket::SendTo(uint8_t const * buffer, size_t length, const Endpoint& endpoint, int flags /* = 0 */)
{
    if (_socket == INVALID_SOCKET) {
        if (!Open(endpoint.GetFamily(), SOCK_DGRAM)) {
            return -1;
        }
    }

    auto[saddr, slen] = endpoint.GetSocketAddress();
    
    ssize_t result = sendto(_socket, buffer, length, flags, saddr, slen);
    
    if (result < 0 
        && result != EAGAIN 
        && result != EWOULDBLOCK) {
        _error = "sendto() " + std::string(strerror(errno));
        Close();
    }

    return result;
}

ssize_t UDPSocket::ReceiveFrom(uint8_t * buffer, size_t length, Endpoint * endpoint, int flags /* = 0 */)
{
    if (_socket == INVALID_SOCKET) {
        return -1;
    }

    ssize_t result;
    if (endpoint) {
        endpoint->SetFamily(_family);
        auto[saddr, slen] = endpoint->GetSocketAddress();
        result = recvfrom(_socket, buffer, length, flags, saddr, &slen);
    }
    else {
        result = recvfrom(_socket, buffer, length, flags, nullptr, nullptr);
    }
    
    if (result < 0 
        && result != EAGAIN 
        && result != EWOULDBLOCK) {
        _error = "recvfrom() " + std::string(strerror(errno));
        Close();
    }

    return result;
}
