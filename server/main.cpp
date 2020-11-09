#include "TCPSocket.hpp"
#include "UDPSocket.hpp"

#include <cstring>

int tcpServer() 
{
    TCPSocket server;

    printf("Bind()\n");
    server.Bind(Endpoint(IPAddress("0.0.0.0"), 9000));
    if (server.HasError()) {
        fprintf(stderr, "%s\n", server.GetErrorString().c_str());
    }

    printf("Listen()\n");
    server.Listen();
    if (server.HasError()) {
        fprintf(stderr, "%s\n", server.GetErrorString().c_str());
    }

    for (;;) {
        printf("Accept()\n");
        TCPSocket sock = server.Accept();
        if (server.HasError()) {
            fprintf(stderr, "%s\n", server.GetErrorString().c_str());
        }

        uint8_t name[32];
        sock.Receive(name, sizeof(name));

        printf("%s\n", name);

        uint8_t buffer[4096];
        int length = snprintf((char *)buffer, sizeof(buffer), "Hello, %s!\n", name);
        sock.Send(buffer, length);

        sock.Close();
    }

    server.Close();
}

int udpServer()
{
    UDPSocket server;

    printf("Bind()\n");
    server.Bind(Endpoint(IPAddress("0.0.0.0"), 9000));
    if (server.HasError()) {
        fprintf(stderr, "%s\n", server.GetErrorString().c_str());
    }

    for (;;) {
        uint8_t name[32];
        Endpoint src;
        server.ReceiveFrom(name, sizeof(name), &src);

        printf("%s from %s:%d\n", name, src.GetAddress().ToString().c_str(), src.GetPort());

        uint8_t buffer[4096];
        int length = snprintf((char *)buffer, sizeof(buffer), "Hello, %s!\n", name);
        server.SendTo(buffer, length, src);
    }

    server.Close();

    return 0;
}

int main(int argc, char * argv[])
{
    // tcpServer();
    udpServer();

    return 0;
}