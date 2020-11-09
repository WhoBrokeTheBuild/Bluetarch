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
)

SET_TARGET_PROPERTIES(
    client
    PROPERTIES
        CXX_STANDARD 17
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