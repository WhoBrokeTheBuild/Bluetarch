#include <cstdio>
#include <cstring>
#include <string>
#include <cerrno>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define INVALID_SOCKET (~0)

#include "IPAddress.hpp"
#include "TCPSocket.hpp"
#include "UDPSocket.hpp"

int tcpTest()
{
    int result = -1;

    const int FAMILY = AF_INET;
    const char * HOSTNAME = "stephenlw.com";
    const uint16_t PORT = 80;

    printf("Resolving %s\n", HOSTNAME);

    IPAddress ipaddr = IPAddress::Lookup(HOSTNAME, FAMILY);
    if (ipaddr == IPAddress::INVALID) {
        fprintf(stderr, "Failed to resolve %s %s\n", HOSTNAME, strerror(errno));
    }

    printf("%s resolved to %s\n", HOSTNAME, ipaddr.ToString().c_str());

    TCPSocket sock(ipaddr, PORT);
    if (sock.HasError()) {
        fprintf(stderr, "Failed to open socket: %s\n", sock.GetErrorString().c_str());
        return 1;
    }

    uint8_t request[] = "HEAD / HTTP/1.1\n\n";

    printf("[REQUEST]\n%s\n[/REQUEST]\n", request);

    uint8_t buffer[4096];

    result = sock.Send(request, sizeof(request));
    if (result < 0) {
        fprintf(stderr, "send() failed %s\n", strerror(errno));
        return 1;
    }

    printf("[RESPONSE]\n");
    for (; result > 0;) {
        result = sock.Receive(buffer, sizeof(buffer));
        if (result < 0) {
            fprintf(stderr, "recv() failed %s\n", strerror(errno));
            return 1;
        }

        printf("%*s", result, buffer);
    }

    printf("\n[/RESPONSE]\n");

    return 0;
}

int udpTest()
{
    int result = -1;

    uint8_t request[] = 
        "\x10\x32\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00\x06\x67" \
        "\x6f\x6f\x67\x6c\x65\x03\x63\x6f\x6d\x00\x00\x10\x00\x01";

    Endpoint endpoint(IPAddress("127.0.0.53"), 53);

    UDPSocket sock;
    result = sock.SendTo(request, sizeof(request), endpoint);

    uint8_t buffer[4096];
    for (; result > 0;) {
        Endpoint src;

        result = sock.ReceiveFrom(buffer, sizeof(buffer), &src);
        if (result < 0) {
            return 1;
        }

        printf("Received %d bytes from %s:%d\n", result, src.GetAddress().ToString().c_str(), src.GetPort());

        for (int i = 0; i < 32; ++i) {
            printf("%02x '%c' ", buffer[i], buffer[i]);
        }
        printf("\n");

        break;
    }
    
    return 0;
}

int tcpServerTest()
{
    TCPSocket socket(IPAddress("192.168.1.73"), 9000);

    const char * name = "Steve";
    socket.Send((uint8_t *)name, sizeof(name));

    uint8_t buffer[4096];
    socket.Receive(buffer, sizeof(buffer));

    printf("%s\n", buffer);

    socket.Close();

    return 0;
}

int udpServerTest()
{
    UDPSocket socket;
    Endpoint endpoint(IPAddress("0.0.0.0"), 9000);

    const char * name = "Steve";
    socket.SendTo((uint8_t *)name, sizeof(name), endpoint);
    printf("SendTo()\n");

    uint8_t buffer[4096];
    socket.ReceiveFrom(buffer, sizeof(buffer), nullptr);
    printf("ReceiveFrom()\n");

    printf("%s\n", buffer);

    socket.Close();

    return 0;
}

int main(int argc, char * argv[])
{
    // if (tcpTest() > 0) {
    //     return 1;
    // }

    // if (udpTest() > 0) {
    //     return 1;
    // }

    tcpServerTest();

    //udpServerTest();

    return 0;
}