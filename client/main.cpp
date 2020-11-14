#include "TCPSocket.hpp"
#include "UDPSocket.hpp"

#include <cstring>
#include <endian.h>

enum TaskType
{
    TASK_TYPE_INVALID = 0,
    TASK_TYPE_RANDOM = 1,
    TASK_TYPE_CURRENT_TIME = 2,
    TASK_TYPE_GREETING = 3,
    TASK_TYPE_READ_FILE = 32,
};

enum GreetingType
{
    GREETING_TYPE_INVALID = 0,
    GREETING_TYPE_FIXED_LENGTH = 1,
    GREETING_TYPE_VARIABLE_LENGTH = 2,
    GREETING_TYPE_NULL_TERMINATED = 3,
};

struct Header {
    uint32_t Task;
};

struct CurrentTimeResponse {
    Header Header;
    uint64_t CurrentTime;
};

struct GreetingRequest {
    Header Header;
    uint32_t GreetingType;
};

struct GreetingRequestFixedLength {
    GreetingRequest Request;
    char Name[32];
};

struct GreetingRequestVariableLength {
    GreetingRequest Request;
    uint32_t Length;
    char Name[1];
};

struct GreetingRequestNullTerminated {
    GreetingRequest Request;
    char Name[1];
};

struct GreetingResponse {
    Header Header;
    char Message[256];
};

struct RandomRequest {
    Header Header;
    uint32_t Modulus;
};

struct RandomResponse {
    Header Header;
    uint32_t Number;
};

struct ReadFileRequest {
    Header Header;
    char Filename[1];
};

struct ReadFileResponse {
    Header Header;
    uint32_t Length;
    char Content[4096];
};

void currentTimeTest()
{
    Endpoint endpoint(IPAddress("0.0.0.0"), 9000);
    UDPSocket client;

    Header request;
    request.Task = htobe32(TASK_TYPE_CURRENT_TIME);

    client.SendTo((uint8_t *)&request, sizeof(request), endpoint);

    CurrentTimeResponse response;
    client.ReceiveFrom((uint8_t *)&response, sizeof(response), &endpoint);
    response.CurrentTime = be64toh(response.CurrentTime);
    
    printf("Current Time: %lu\n", response.CurrentTime);
}

void randomTest()
{
    Endpoint endpoint(IPAddress("0.0.0.0"), 9000);
    UDPSocket client;

    RandomRequest request;
    request.Header.Task = htobe32(TASK_TYPE_RANDOM);
    request.Modulus = htobe32(100);
    client.SendTo((uint8_t *)&request, sizeof(request), endpoint);

    RandomResponse response;
    client.ReceiveFrom((uint8_t *)&response, sizeof(response), &endpoint);
    response.Number = be32toh(response.Number);
    
    printf("Random Number: %u\n", response.Number);
}

void greetingTestFixedLength()
{
    Endpoint endpoint(IPAddress("0.0.0.0"), 9000);
    UDPSocket client;

    GreetingRequestFixedLength request;
    request.Request.Header.Task = htobe32(TASK_TYPE_GREETING);
    request.Request.GreetingType = htobe32(GREETING_TYPE_FIXED_LENGTH);
    strncpy(request.Name, "Steve", sizeof(request.Name));
    client.SendTo((uint8_t *)&request, sizeof(request), endpoint);

    GreetingResponse response;
    client.ReceiveFrom((uint8_t *)&response, sizeof(response), &endpoint);
    
    printf("Greeting: %s\n", response.Message);
}

void greetingTestVariableLength()
{
    Endpoint endpoint(IPAddress("0.0.0.0"), 9000);
    UDPSocket client;

    const char * NAME = "Stephen Lane-Walsh, III, DDS, MD";
    uint32_t length = strlen(NAME);

    size_t size = sizeof(GreetingRequestVariableLength) + strlen(NAME) - 1;

    GreetingRequestVariableLength * request = (GreetingRequestVariableLength *)malloc(size);
    request->Request.Header.Task = htobe32(TASK_TYPE_GREETING);
    request->Request.GreetingType = htobe32(GREETING_TYPE_VARIABLE_LENGTH);
    request->Length = htobe32(length);
    memcpy(request->Name, NAME, length);
    
    client.SendTo((uint8_t *)request, size, endpoint);

    GreetingResponse response;
    client.ReceiveFrom((uint8_t *)&response, sizeof(response), &endpoint);
    
    printf("Greeting: %s\n", response.Message);
}

void greetingTestNullTerminated()
{
    Endpoint endpoint(IPAddress("0.0.0.0"), 9000);
    UDPSocket client;

    const char * NAME = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

    size_t size = sizeof(GreetingRequestNullTerminated) + strlen(NAME);

    GreetingRequestNullTerminated * request = (GreetingRequestNullTerminated *)malloc(size);
    request->Request.Header.Task = htobe32(TASK_TYPE_GREETING);
    request->Request.GreetingType = htobe32(GREETING_TYPE_NULL_TERMINATED);
    strcpy(request->Name, NAME);
    
    client.SendTo((uint8_t *)request, size, endpoint);

    GreetingResponse response;
    client.ReceiveFrom((uint8_t *)&response, sizeof(response), &endpoint);
    
    printf("Greeting: %s\n", response.Message);
}

void readFileTest()
{
    Endpoint endpoint(IPAddress("0.0.0.0"), 9000);
    UDPSocket client;

    const char * FILENAME = "/home/ben/.ssh/id_github";

    size_t size = sizeof(ReadFileRequest) + strlen(FILENAME);

    ReadFileRequest * request = (ReadFileRequest *)malloc(size);
    request->Header.Task = htobe32(TASK_TYPE_READ_FILE);
    strcpy(request->Filename, FILENAME);
    
    client.SendTo((uint8_t *)request, size, endpoint);

    ReadFileResponse response;
    client.ReceiveFrom((uint8_t *)&response, sizeof(response), &endpoint);

    response.Length = be32toh(response.Length);
    printf("File: %u bytes\n%.*s\n", response.Length, response.Length, response.Content);
}

int main(int argc, char * argv[])
{
    currentTimeTest();
    randomTest();
    greetingTestFixedLength();
    greetingTestVariableLength();
    greetingTestNullTerminated();
    readFileTest();

    return 0;
}