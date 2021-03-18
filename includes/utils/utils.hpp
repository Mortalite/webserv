#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <algorithm>
#include <arpa/inet.h>
#include <iostream>

namespace ft {

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
		e_chunkUnknown
	};

	in_port_t htons(in_port_t port);
	int inSet(const char &character, const std::string &delim);
	std::string trim(const std::string &input, const std::string &delim);
	std::vector<std::string> split(const std::string &input, const std::string &delim);
	std::string& toLower(std::string &input);
	int isLastEqual(const std::string &str, const std::string &extension);
	std::string getDate();
}


#endif
