cmake_minimum_required(VERSION 3.17)
project(webserv)

include_directories(includes)
set(CMAKE_CXX_STANDARD 98)

add_executable(webserv srcs/main.cpp srcs/utils/utils.cpp includes/utils/utils.hpp srcs/server/Server.cpp includes/server/Server.hpp)