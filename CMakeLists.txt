CMAKE_MINIMUM_REQUIRED(VERSION 3.15 FATAL_ERROR)

PROJECT(Bluetarch)

FILE(
    GLOB_RECURSE
    sources 
    src/*.hpp
    src/*.cpp
)


ADD_EXECUTABLE(
    client
    ${sources}
    client/main.cpp
)

SET_TARGET_PROPERTIES(
    client
    PROPERTIES
        CXX_STANDARD 17
)

TARGET_INCLUDE_DIRECTORIES(
    client
    PUBLIC
        src
)


ADD_EXECUTABLE(
    server
    ${sources}
    server/main.cpp
)

SET_TARGET_PROPERTIES(
    server
    PROPERTIES
        CXX_STANDARD 17
)

TARGET_INCLUDE_DIRECTORIES(
    server
    PUBLIC
        src
)


ADD_EXECUTABLE(
    dnsclient
    ${sources}
    dnsclient/main.cpp
)

SET_TARGET_PROPERTIES(
    dnsclient
    PROPERTIES
        CXX_STANDARD 17
)

TARGET_INCLUDE_DIRECTORIES(
    dnsclient
    PUBLIC
        src
)


ADD_CUSTOM_TARGET(
    run-client
    COMMAND $<TARGET_FILE:client>
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_CUSTOM_TARGET(
    valgrind-client
    COMMAND valgrind $<TARGET_FILE:client>
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_CUSTOM_TARGET(
    run-server
    COMMAND $<TARGET_FILE:server>
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)