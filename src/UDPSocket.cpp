#include "UDPSocket.hpp"

#include <cerrno>
#include <cstring>

UDPSocket::UDPSocket()
{

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
