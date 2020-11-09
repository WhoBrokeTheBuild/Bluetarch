#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <tuple>
#include <memory>
#include <string>

#define INVALID_SOCKET (~0)

class Socket
{
public:

    Socket();

    Socket(int socket, int family);

    virtual ~Socket();

    bool Open(int family, int type);

    void Close();

    bool HasError() const;

    std::string GetErrorString() const;

    operator bool() const;

protected:

    int _family;

    int _socket;

    std::string _error;

}; // class Socket

#endif // SOCKET_HPP