#include "UDPSocket.hpp"

#include <ctime>
#include <cstring>
#include <random>
#include <sstream>

struct DNSQueryHeader
{
    uint16_t TransactionID;
    union {
        uint16_t Flags;
        struct {
            uint8_t RecursionDesired:1;
            uint8_t Truncation:1;
            uint8_t AuthoritativeAnswer:1;
            uint8_t Opcode:4;
            uint8_t QueryOrResponse:1;

            uint8_t ResponseCode:4;
            uint8_t Reserved:3;
            uint8_t RecursionAvailable:1;
        };
    };
    uint16_t Questions;
    uint16_t Answers;
    uint16_t Authorities;
    uint16_t Additional;
};

uint8_t * getName(uint8_t * ptr, std::string& name)
{
    while (ptr[0] > 0) {
        name += std::string((char *)ptr + 1, ptr[0]) + '.';
        ptr += ptr[0];
        ++ptr;
    }
    ++ptr;

    return ptr;
}

const char * getType(uint16_t value)
{
    switch (value) {
    case 0x0001:
        return "A";
    case 0x0010:
        return "TXT";
    }

    return "?";
}

const char * getClass(uint16_t value)
{
    switch (value) {
    case 0x0001:
        return "IN";
    }

    return "?";
}

int main(int argc, char * argv[])
{
    srand(time(0));

    if (argc == 1) {
        printf("Usage: dnsclient DOMAIN\n");
        return 1;
    }

    Endpoint endpoint(IPAddress("127.0.0.53"), 53);
    UDPSocket socket;

    DNSQueryHeader header;
    memset(&header, 0, sizeof(header));
    header.TransactionID = rand() % UINT16_MAX;
    header.RecursionDesired = 1;
    header.Questions = htons(1);
    header.QueryOrResponse = 0;
    header.Opcode = 0;

    uint8_t buffer[4096];
    uint8_t * ptr = buffer;

    memcpy(ptr, &header, sizeof(header));
    ptr += sizeof(header);

    std::stringstream ss(argv[1]);
    std::string token;
    while (std::getline(ss, token, '.')) {
        *ptr = token.length();
        ++ptr;

        strncpy((char *)ptr, token.c_str(), token.length());
        ptr += token.length();
    }

    *ptr = 0;
    ++ptr;

    uint16_t Type = htons(1);
    uint16_t Class = htons(1);
    uint32_t TTL;
    uint16_t DataLength;

    memcpy(ptr, &Type, sizeof(Type));
    ptr += 2;
    memcpy(ptr, &Class, sizeof(Class));
    ptr += 2;

    socket.SendTo(buffer, ptr - buffer + 1, endpoint);

    socket.ReceiveFrom(buffer, sizeof(buffer), &endpoint);
    memcpy(&header, buffer, sizeof(header));

    ptr = buffer + sizeof(header);

    printf("Questions:\n");
    for (int i = 0; i < ntohs(header.Questions); ++i) {
        std::string name;
        ptr = getName(ptr, name);
        
        memcpy(&Type, ptr, sizeof(Type));
        ptr += 2;
        Type = ntohs(Type);
        
        memcpy(&Class, ptr, sizeof(Class));
        ptr += 2;
        Class = ntohs(Class);

        printf("%30s\t%4s\t%4s\n", name.c_str(), getType(Type), getClass(Class));
    }
    printf("\n");

    printf("Answers:\n");
    for (int i = 0; i < ntohs(header.Answers); ++i) {
        uint16_t NameOffset;
        memcpy(&NameOffset, ptr, sizeof(NameOffset));
        ptr += 2;

        std::string name;

        NameOffset = ntohs(NameOffset);
        if ((NameOffset & 0xF000) == 0xC000) {
            NameOffset &= 0x0FFF;

            getName(buffer + NameOffset, name);
        }
        
        memcpy(&Type, ptr, sizeof(Type));
        ptr += 2;
        Type = ntohs(Type);
        
        memcpy(&Class, ptr, sizeof(Class));
        ptr += 2;
        Class = ntohs(Class);
        
        memcpy(&TTL, ptr, sizeof(TTL));
        ptr += 4;
        TTL = ntohl(TTL);
        
        memcpy(&DataLength, ptr, sizeof(DataLength));
        ptr += 2;
        DataLength = ntohs(DataLength);
        
        if (DataLength == 4) {
            printf("%30s\t%4d\t%4s\t%s\t%d.%d.%d.%d\n", 
                name.c_str(), TTL, getType(Type), getClass(Class), 
                ptr[0], ptr[1], ptr[2], ptr[3]);
        }

        ptr += DataLength;
    }

    return 0;
}