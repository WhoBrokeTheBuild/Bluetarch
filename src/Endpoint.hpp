#ifndef ENDPOINT_HPP
#define ENDPOINT_HPP

#include <netinet/in.h>

#include <memory>

#include "IPAddress.hpp"

class Endpoint
{
public:

    Endpoint() = default;
    Endpoint(const IPAddress& addr, uint16_t port);

    void SetAddress(const IPAddress& addr);
    void SetPort(uint16_t port);

    IPAddress GetAddress() const;
    uint16_t GetPort() const;

    void SetFamily(int family);
    int GetFamily() const;

    std::tuple<sockaddr *, socklen_t> GetSocketAddress() const;

private:

    int _family;
    sockaddr_in _sockaddr4;
    sockaddr_in6 _sockaddr6;

}; // class Endpoint

#endif // ENDPOINT_HPP