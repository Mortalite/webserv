cmake_minimum_required(VERSION 3.17)
project(webserv)

include_directories(includes)
set(CMAKE_CXX_STANDARD 98)

add_executable(webserv srcs/main.cpp srcs/utils/utils.cpp includes/utils/utils.hpp srcs/server/Server.cpp includes/server/Server.hpp srcs/parser/Request.cpp includes/parser/Request.hpp srcs/server/Client.cpp includes/server/Client.hpp srcs/utils/Data.cpp includes/utils/Data.hpp srcs/parser/Response.cpp includes/parser/Response.hpp includes/utils/HttpStatusCode.hpp includes/utils/StatusCodeData.hpp srcs/utils/HttpStatusCode.cpp srcs/utils/StatusCodeData.cpp)