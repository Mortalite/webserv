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
#include <ctime>
#include <sys/time.h>

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

int inSet(const char &character, const std::string &delim);
std::string trim(const std::string &string, const std::string &delim);
std::vector<std::string> split(const std::string &input, const std::string &delim);
std::string& toLower(std::string &string);
int isLastEqual(const std::string &string, const std::string &extension);
int readHeaderSize(const std::string& string);
int getNextLine(int fd, std::string &line);
std::string currentTime();
std::string readFile(const std::string &filename);
bool isValidFile(const std::string &filename);
int parseLine(int fd, std::string& buffer);

template <typename T, typename M, size_t arrayLength> void arrayToVec(const T (&array)[arrayLength], M& vec) {
    for (size_t i = 0; i < sizeof(array)/sizeof(array[0]); i++)
        vec.push_back(array[i]);
}

#endif
