#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <string.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
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

enum PATTERN_FLAG {
	e_server,
	e_location,
	e_end
};

int inSet(const char &character, const std::string &delim);
std::string trim(const std::string &string, const std::string &delim);
std::vector<std::string> split(const std::string &input, const std::string &delim);
std::string& toLower(std::string &string);
int isLastEqual(const std::string &string, const std::string &extension);
int readHeaderSize(const std::string& string);
int getNextLine(int fd, std::string &line);
std::string convertTime(const time_t& time);
std::string currentTime();
std::string readFile(const std::string &filename);
int parseLine(int fd, std::string& buffer);
bool matchPattern(int flag, std::vector<std::string> vec);

template <typename T, typename M, size_t arrayLength>
bool isEqual(const T (&array)[arrayLength], M& vec) {
	static size_t patternSize;

	patternSize = sizeof(array)/sizeof(array[0]);
	if (patternSize == vec.size()) {
		for (size_t i = 0; i != vec.size(); i++) {
			if (vec[i] != array[i] && array[i] != "*")
				return (false);
		}
		return (true);
	}
	return (false);
}

#endif
