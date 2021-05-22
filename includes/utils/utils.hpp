#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <ctime>
#include <climits>
#include <cstring>
#include <sstream>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include "utils/TraceException.hpp"

#define RED		"\e[31;1m"
#define BLUE	"\e[34;1m"
#define BLUE_B	"\e[46;1m"
#define WHITE_B "\e[7;1m"
#define RESET	"\e[0m"

/*
** Global enum values
*/
enum FLAGS {
	e_listenSocket,
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
	e_mime,
	e_end,
};

enum FILE_TYPE {
	e_valid,
	e_invalid,
	e_directory,
	e_file_type_error
};

/*
** Global structure
*/
struct TargetInfo {
	std::string _path;
	int			_type;
	struct stat	_stat;
	std::string _size;
	std::string _body;
	std::string _lstMod;
};

/*
** Global values
*/
static std::string mimeTypesConfig("config/mime.types");
static std::string webserverConfig("config/webserv.conf");
static std::string errorsDirectory("config/errors");
static std::string delimConfig(" \t");
static std::string delimHeaders("\r\n");
static std::string defaultAllowedMethod[] = {"GET", "HEAD", "POST", "PUT", "CONNECT", "OPTIONS", "TRACE"};
static long defaultClientTimeout = 3600;
static size_t defaultAllowedMethodSize = sizeof(defaultAllowedMethod)/sizeof(defaultAllowedMethod[0]);
static size_t bufferSize = 100 * 1000 * 1000;
static long defaultClientMaxBodySize = 10 * 1000 * 1000;

namespace ft {
	/*
	** libft functions
	*/
	std::string tolower(std::string string);
	std::string toupper(std::string string);

	/*
	** C++ functions
	*/
	std::string trim(const std::string &string, const std::string &delim);
	std::vector<std::string> split(const std::string &input, const std::string &delim);
	bool isStartWith(const std::string &string, const std::string &extension);
	bool isEndWith(const std::string &string, const std::string &extension);
	int readHeaderSize(const std::string &string);
	int getNextLine(int fd, std::string &line);
	std::string convertTime(const time_t &time);
	std::string currentTime();
	std::string readFile(int fd);
	std::string readFile(const std::string &filename);
	int parseLine(int fd, std::string &buffer);
	bool matchPattern(int flag, std::vector<std::string> vec);
	long strToLong(const std::string &string, int base);
	int &getDebug();
	void getFileInfo(const std::string &filename, TargetInfo &targetInfo);
	void getStringInfo(const std::string &string, TargetInfo &targetInfo);
	std::string getcwd();
	void exit_fatal(std::string msg);

	/*
	** Templates
	*/
	template<typename T, typename M>
	bool isInSet(const T &set, const M &value) {
		static std::string::size_type i;

		for (i = 0; i < set.size(); i++)
			if (value == set[i])
				return (true);
		return (false);
	}

	template<typename T, typename M, size_t arrayLength>
	bool isEqual(const T (&array)[arrayLength], M &vec) {
		static size_t patternSize;

		patternSize = sizeof(array) / sizeof(array[0]);
		if (patternSize == vec.size()) {
			for (size_t i = 0; i != vec.size(); i++) {
				if (vec[i] != array[i] && array[i] != "*")
					return (false);
			}
			return (true);
		}
		return (false);
	}

	template<typename T>
	std::string valueToString(T value) {
		std::ostringstream ss;

		ss << value;
		return (ss.str());
	}

	template<typename T>
	void printContainer(std::ostream &stream, std::string containerName, const T &container) {
		size_t counter = 0;

		for (typename T::const_iterator it = container.begin(); it != container.end(); it++)
			stream << containerName << "[" << counter++ << "] = " << *it << std::endl;
	}

	template<typename T>
	void printContainerPair(std::ostream &stream, std::string containerName, const T &container) {
		size_t counter = 0;

		for (typename T::const_iterator it = container.begin(); it != container.end(); it++)
			stream << containerName << "[" << counter++ << "] = (" << (*it).first << ", " << (*it).second << ")"
				   << std::endl;
	}


	template<typename T>
	void printContainerMap(std::ostream &stream, std::string containerName, const T &container) {
		size_t counter = 0;

		for (typename T::const_iterator it = container.begin(); it != container.end(); it++)
			stream << containerName << "[" << counter++ << "] = (" << (*it).first << ", " << (*it).second << ")"
				   << std::endl;
	}
}
