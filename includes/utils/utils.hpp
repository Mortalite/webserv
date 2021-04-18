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

#define RED		"\e[31;1m"
#define BLUE	"\e[34;1m"
#define BLUE_B	"\e[46;1m"
#define WHITE_B "\e[7;1m"
#define RESET	"\e[0m"

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

int	ft_strcmp(const char *s1, const char *s2);
int inSet(const char &character, const std::string &delim);
std::string trim(const std::string &string, const std::string &delim);
std::vector<std::string> split(const std::string &input, const std::string &delim);
std::string& toLower(std::string &string);
int isLastEqual(const std::string &string, const std::string &extension);
int readHeaderSize(const std::string& string);
std::string readFile(const std::string &filename);
bool isValidFile(const std::string &filename);
struct tm *ft_gmtime(const time_t *timer);
std::string ft_ctime(const time_t* timer);
std::string currentTime();

#endif
