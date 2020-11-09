#include "IPAddress.hpp"

#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
    
IPAddress IPAddress::INVALID = IPAddress(AF_UNSPEC);

IPAddress IPAddress::Lookup(const std::string& hostname, int family /* = AF_INET */)
{
    struct hostent * host = gethostbyname2(hostname.c_str(), family);
    if (!host) {
        return IPAddress::INVALID;
    }

    return IPAddress((uint8_t *)host->h_addr_list[0], family);
}

IPAddress::IPAddress(const std::string& addr, int family /* = AF_INET */)
{
    _family = family;
    if (inet_pton(family, addr.c_str(), _address) <= 0) {
        _family = AF_UNSPEC;
    }
}

IPAddress::IPAddress(int family /* = AF_INET */)
    : _family(family)
{ }

IPAddress::IPAddress(uint8_t addr[], int family /* = AF_INET */)
    : _family(family)
{
    memcpy(_address, addr, sizeof(_address));
}

IPAddress::IPAddress(const IPAddress& other)
    : _family(other._family)
{
    memcpy(_address, other._address, sizeof(_address));
}

IPAddress& IPAddress::operator=(const IPAddress& other) {
    _family = other._family;
    memcpy(_address, other._address, sizeof(_address));
    return *this;
}

bool IPAddress::operator==(const IPAddress& other) const
{
    if (_family != other._family) {
        return false;
    }

    if (memcmp(_address, other._address, GetAddressSize()) == 0) {
        return true;
    }

    return false;
}

bool IPAddress::operator!=(const IPAddress& other) const
{
    return !(*this == other);
}

std::string IPAddress::ToString() const
{
    char buffer[INET6_ADDRSTRLEN];

    if (!inet_ntop(_family, _address, buffer, INET6_ADDRSTRLEN)) {
        return "(invalid)";
    }

    return std::string(buffer);
}

int IPAddress::GetAddressSize() const
{
    switch (_family)
    {
    case AF_INET:
        return 4;
    case AF_INET6:
        return 16;
    }

    return -1;
}

uint8_t * IPAddress::GetAddress() const
{
    return (uint8_t *)_address;
}

int IPAddress::GetFamily() const
{
    return _family;
}