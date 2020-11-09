#include "Socket.hpp"

#if defined(WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#define closesocket close
#endif

#include <cstring>
#include <cerrno>

#include <netinet/in.h>

Socket::Socket()
    : _family(AF_UNSPEC)
    , _socket(INVALID_SOCKET)
{ }

Socket::Socket(int socket, int family)
    : _family(family)
    , _socket(socket)
{ }

Socket::~Socket()
{
    Close();
}

bool Socket::Open(int family, int type)
{
    _family = family;
    _socket = socket(_family, type, 0);
    if (_socket == INVALID_SOCKET) {
        _error = "socket() " + std::string(strerror(errno));
        return false;
    }

    return true;
}

void Socket::Close()
{
    if (_socket != INVALID_SOCKET) {
        closesocket(_socket);
    }
    _socket = INVALID_SOCKET;
}

bool Socket::HasError() const
{
    return (_socket == INVALID_SOCKET);
}

std::string Socket::GetErrorString() const
{
    return _error;
}

Socket::operator bool() const
{
    return (_socket != INVALID_SOCKET);
}

// std::tuple<std::unique_ptr<sockaddr>, socklen_t> 
//     Socket::GetSocketAddress(const IPAddress& addr, uint16_t port)
// {
//     if (_family == AF_INET) {
//         sockaddr_in * saddr = new sockaddr_in();
//         saddr->sin_family = _family;
//         saddr->sin_port = htons(port);
//         memcpy(&saddr->sin_addr, addr.GetAddress(), 4);
//         memset(saddr->sin_zero, 0, sizeof(sockaddr_in::sin_zero));

//         return { 
//             std::unique_ptr<sockaddr>((sockaddr *)saddr),
//             sizeof(sockaddr_in)
//         };
//     }
//     else if (_family == AF_INET6) {
//         sockaddr_in6 * saddr = new sockaddr_in6();
//         saddr->sin6_family = _family;
//         saddr->sin6_port = htons(port);
//         memcpy(&saddr->sin6_addr, addr.GetAddress(), 16);
//         // saddr->sin6_flowinfo = 0; // ??
//         // saddr->sin6_scope_id = 0; // ??

//         return { 
//             std::unique_ptr<sockaddr>((sockaddr *)saddr),
//             sizeof(sockaddr_in6)
//         };
//     }
    
//     return { std::unique_ptr<sockaddr>(), 0 };
// }
