#include "Endpoint.hpp"

#include <cstring>

Endpoint::Endpoint(const IPAddress& addr, uint16_t port)
    : _family(AF_UNSPEC)
{
    SetAddress(addr);
    SetPort(port);
}

void Endpoint::SetAddress(const IPAddress& addr)
{
    _family = addr.GetFamily();

    if (_family == AF_INET) {
        _sockaddr4.sin_family = _family;
        memcpy((uint8_t *)&_sockaddr4.sin_addr, addr.GetAddress(), sizeof(_sockaddr4.sin_addr));
    }
    else if (_family == AF_INET6) {
        _sockaddr6.sin6_family = _family;
        memcpy((uint8_t *)&_sockaddr6.sin6_addr, addr.GetAddress(), sizeof(_sockaddr6.sin6_addr));
    }
}

void Endpoint::SetPort(uint16_t port)
{
    _sockaddr4.sin_port = htons(port);
    _sockaddr6.sin6_port = htons(port);
}

IPAddress Endpoint::GetAddress() const
{
    if (_family == AF_INET) {
        return IPAddress((uint8_t *)&_sockaddr4.sin_addr, _family);
    }
    else if (_family == AF_INET6) {
        return IPAddress((uint8_t *)&_sockaddr6.sin6_addr, _family);
    }

    return IPAddress::INVALID;
}

uint16_t Endpoint::GetPort() const
{
    if (_family == AF_INET) {
        return ntohs(_sockaddr4.sin_port);
    }
    else if (_family == AF_INET6) {
        return ntohs(_sockaddr6.sin6_port);
    }

    return 0;
}

void Endpoint::SetFamily(int family)
{
    _family = family;
}

int Endpoint::GetFamily() const
{
    return _family;
}

std::tuple<sockaddr *, socklen_t> Endpoint::GetSocketAddress() const
{
    if (_family == AF_INET) {
        return { (sockaddr *)&_sockaddr4, sizeof(_sockaddr4) };
    }
    else if (_family == AF_INET6) {
        return { (sockaddr *)&_sockaddr6, sizeof(_sockaddr6) };
    }
    
    return { nullptr, 0 };
}