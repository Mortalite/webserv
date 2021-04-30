#pragma once

#include <iostream>
#include <list>
#include "utils/Data.hpp"
#include "utils/utils.hpp"

class Client {

public:
	typedef std::map<std::string, std::string> _headersType;
	typedef std::list<Client*> _clientsType;
	typedef _clientsType::iterator _clientIt;

	Client(const Server *server, int socket, int flag);
	Client(const Client& other);
	~Client();

	Client& operator=(const Client& other);

	friend std::ostream& operator<<(std::ostream &stream, const Client& client) {
		stream << "_socket = " << client._socket << std::endl;
		stream << "_flag = " << client._flag << std::endl;
		stream << "_chunkMod = " << client._chunkMod << std::endl;
		stream << "_size = " << client._size << std::endl;
		stream << "_headers = " << client._headers << std::endl;
		printContainerMap(stream, "_headerMap", client._headersMap);
		stream << "_body = " << client._body << std::endl;
		stream << "_hexNum = " << client._hexNum << std::endl;
		stream << "_httpStatusCode = " << client._httpStatusCode << std::endl;
		return (stream);
	}

	void wipeData();

	const Server *_server;
	int _socket;
	int _flag;
	int _chunkMod;
	long _size;
	std::string _headers;
	_headersType _headersMap;
	std::string _body;
	std::string _hexNum;
	HttpStatusCode _httpStatusCode;
};
