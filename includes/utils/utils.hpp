#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include "utils/HttpStatusCode.hpp"

/*
** Флаги чтения запроса
*/
enum FLAGS {
	e_recvHeaders,
	e_recvContentBody,
	e_recvChunkBody,
	e_sendResponse,
	e_closeConnection,
};

enum CHUNK {
	e_recvChunkData,
	e_recvChunkHex,
};

in_port_t htons(in_port_t port);
int inSet(const char &character, const std::string &delim);
std::string trim(const std::string &string, const std::string &delim);
std::vector<std::string> split(const std::string &input, const std::string &delim);
std::string& toLower(std::string &string);
int isLastEqual(const std::string &string, const std::string &extension);
int readHeaderSize(const std::string& string);
std::string readFile(const std::string &filename);
bool isValidFile(const std::string &filename);
struct tm *ft_gmtime(const time_t *timer);
std::string ctimes(const time_t* timer);
std::string currentTime();

#endif
