cmake_minimum_required(VERSION 3.17)
project(webserv)

include_directories(includes)
set(CMAKE_CXX_STANDARD 98)

add_executable(webserv  srcs/main.cpp
                        srcs/utils/utils.cpp
                        includes/utils/utils.hpp
                        srcs/server/Manager.cpp
                        includes/server/Manager.hpp
                        srcs/parser/Response.cpp
                        includes/parser/Response.hpp
                        srcs/server/Client.cpp
                        includes/server/Client.hpp
                        srcs/utils/Data.cpp
                        includes/utils/Data.hpp
                        srcs/utils/HttpStatusCode.cpp
                        includes/utils/HttpStatusCode.hpp
                        srcs/parser/Request.cpp
                        includes/parser/Request.hpp
        srcs/parser/Server.cpp
        includes/parser/Server.hpp srcs/parser/Location.cpp includes/parser/Location.hpp srcs/parser/Base.cpp includes/parser/Base.hpp)