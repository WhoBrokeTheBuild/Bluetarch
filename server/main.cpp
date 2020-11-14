#include "UDPSocket.hpp"

#include <endian.h>

struct Header {
    uint32_t Task; 
};

struct Request {
    Header header;
    union
    {
        uint32_t modulus;

        struct {
            uint32_t GreetingType;
            union {
                // Fixed length
                char fname[32];

                // Variable Length
                struct {
                    uint32_t length;
                    char vname[1];
                };

                // Null Terminated
                char nname[1];
            };
        };
        
        char filename[1];

        char _[4096];
    };
};

struct CurrentTime {
    Header header;
    uint64_t time;
};

struct RandomNumber {
    Header header;
    uint32_t number;
};

enum GreetingType {
    GREETING_TYPE_INVALID = 0,
    GREETING_TYPE_FIXED_LENGTH = 1,
    GREETING_TYPE_VARIABLE_LENGTH = 2,
    GREETING_TYPE_NULL_TERMINATED = 3,
};

struct Greeting {
    Header header;
    char message[256];
};

struct File {
    Header header;
    uint32_t length;
    char fileContents[4096];
};

enum TaskType
{
    INVALID = 0,
    RANDOM = 1,
    CURRENT_TIME = 2,
    GREETING = 3,
    READ_FILE = 32,
};

/*
uint32_t
Task = Random, Current Time, Greeting, Read File

Random:
    uint32_t Modulus (Rand % mod)
    return uint32_t

Current Time:
    return uint64_t

Greeting:
    // Fixed Length
    string Name[32]

    // Variable Length
    uint8_t Length
    string Name[]

    // Null Terminated
    string Name[]
    NULL

    Print(Greeting)
    Return Greeting 256

Read File:
    string Filename
    return Data
*/

#define IP "0.0.0.0"
#define PORT 9000

int main(int argc, char * argv[])
{
    UDPSocket server;

    // Bind the server
    printf("Bind()\n");
    server.Bind(Endpoint(IPAddress(IP), PORT));
    if (server.HasError()) {
        fprintf(stderr, "%s\n", server.GetErrorString().c_str());
    }

    for (;;) {
        Endpoint ep;
        Request request;

        // Bigger buffer to read from
        // stuff head in
        // stuff in number

        // Recieve
        server.ReceiveFrom((uint8_t*)&request, sizeof(request), &ep);
        printf("RecieveFrom()\n");

        request.header.Task = htobe32(request.header.Task);
        printf("Task: %u\n", request.header.Task);

        switch (request.header.Task)
        {
            case INVALID:
                break;
            case RANDOM:
                RandomNumber rn;
                rn.header.Task = htobe32(request.header.Task);
                request.modulus = be32toh(request.modulus);
                rn.number = htobe32(rand() % request.modulus);
                printf("SendTo() with rand %u\n", be32toh(rn.number));

                server.SendTo((uint8_t*)&rn, sizeof(rn), ep);
                break;
            case CURRENT_TIME:
                CurrentTime t;
                t.header.Task = htobe32(request.header.Task);
                t.time = htobe64(time(0));
                printf("SendTo() with time: %lu\n", be64toh(t.time));

                server.SendTo((uint8_t*)&t, sizeof(t), ep);
                break;
            case GREETING:
                Greeting greeting;
                greeting.header.Task = htobe32(request.header.Task);

                switch (be32toh(request.GreetingType))
                {
                case INVALID:
                    break;

                case GREETING_TYPE_FIXED_LENGTH:
                    snprintf(greeting.message, sizeof(greeting.message), "Hello, %s", request.fname);
                    printf("%s\n", greeting.message);
                    break;

                case GREETING_TYPE_VARIABLE_LENGTH:
                    snprintf(greeting.message, sizeof(greeting.message), "Hello, %.*s", request.length, request.vname);

                    // This would cause buffer overflow and could cause bad things to happen.
                    //sprintf(greeting.message, "Hello, %.*s", request.length, request.vname);
                    printf("%s\n", greeting.message);
                    break;

                case GREETING_TYPE_NULL_TERMINATED:
                    snprintf(greeting.message, sizeof(greeting.message), "Hello, %s", request.nname);
                    printf("%s\n", greeting.message);
                    break;
                }

                server.SendTo((uint8_t*)&greeting, sizeof(greeting), ep);
                break;
            case READ_FILE:
                File file;
                file.header.Task = htobe32(request.header.Task);

                FILE * f = fopen(request.filename, "rt");

                if (!f){
                    break;
                }

                fseek(f, 0, SEEK_END);
                file.length = ftell(f);
                fseek(f, 0, SEEK_SET);

                if (file.length > sizeof(file.fileContents))
                {
                    file.length = sizeof(file.fileContents);
                }

                size_t bytesRead = fread(file.fileContents, 1, file.length, f);

                printf("%.*s \n%u\n", file.length, file.fileContents, file.length);

                fclose(f);

                file.length = htobe32(file.length);

                size_t bytes = sizeof(file.header) + sizeof(file.length) + bytesRead;

                printf("bytes: %zu\n", bytes);


                server.SendTo((uint8_t*)&file, bytes, ep);

                break;
        }
    }

    server.Close();
    return 0;
}