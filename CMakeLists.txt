
cmake_minimum_required(VERSION 3.9)
PROJECT(peer_client)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-pthread")

SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREAD ON)
FIND_PACKAGE(Boost REQUIRED COMPONENTS system thread regex)
IF(Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)

SET(USED_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY})

ADD_EXECUTABLE(peer_client main.cpp peer_client.cpp peer_client.h handlers.cpp msg_formats.h msg_builder.cpp msg_builder.h
        peer.cpp peer.h utils.h)
TARGET_LINK_LIBRARIES(peer_client ${USED_LIBS})

