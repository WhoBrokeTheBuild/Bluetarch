#ifndef IP_ADDRESS_HPP
#define IP_ADDRESS_HPP

#include <cstdint>
#include <string>
#include <netinet/in.h>

class IPAddress
{
public:

    static IPAddress INVALID;

    static IPAddress Lookup(const std::string& hostname, int family = AF_INET);

    IPAddress(int family = AF_INET);

    IPAddress(const std::string& addr, int family = AF_INET);

    IPAddress(uint8_t addr[], int family = AF_INET);

    IPAddress(const IPAddress& other);

    virtual ~IPAddress() = default;

    IPAddress& operator=(const IPAddress& other);

    bool operator==(const IPAddress& other) const;
    bool operator!=(const IPAddress& other) const;

    std::string ToString() const;

    uint8_t * GetAddress() const;

    int GetFamily() const;

private:

    int GetAddressSize() const;

    int _family;

    uint8_t _address[16];

};

#endif // IP_ADDRESS_HPP