#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <memory>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>

#include <boost/locale/encoding_utf.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#define LOG_THREAD "LoggerThread"
#define BOOST_SERVICE_THREAD "boost_service_thread"
#define FILESWATCHER_THREAD "FilesThread"

#define PEER_ID_SIZE 20

using namespace std;

typedef boost::asio::ip::address IpAddress;
typedef boost::asio::ip::address_v4 Ipv4Address;

typedef boost::asio::ip::tcp tcp;
typedef tcp::socket tcp_socket;
typedef tcp::endpoint tcp_endpoint;
typedef tcp::acceptor tcp_acceptor;

typedef boost::asio::local::stream_protocol stream_protocol;
typedef stream_protocol::socket unix_socket;
typedef stream_protocol::endpoint unix_endpoint;
typedef stream_protocol::acceptor unix_acceptor;

typedef boost::thread Thread;

typedef boost::system::error_code ErrorCode;

typedef const char* cc_string;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long uint64;

// Global variables
static uint16 Port = 54321;
static string Seed = "127.0.0.1";
static uint16 SeedPort = 54321;
static map<string, shared_ptr<Thread>> ThreadsMap;

string to_string(const string& wstr);

#endif // UTILS_H
