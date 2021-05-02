#pragma once

#include <iostream>
#include <list>
#include "utils/Data.hpp"
#include "utils/utils.hpp"

struct Client {
	typedef std::map<std::string, std::string> _headersType;
	typedef std::list<Client*> _clientsType;
	typedef _clientsType::iterator _clientIt;

	Client(const Server *acceptServer, int socket, int flag);
	Client(const Client& other);
	~Client();

	Client& operator=(const Client& other);

	friend std::ostream& operator<<(std::ostream &stream, const Client& client) {
		std::cout << BLUE_B << "CLIENT" << RESET << std::endl;
		if (client._acceptServer)
			stream << *client._acceptServer << std::endl;
		if (client._responseServer)
			stream << *client._responseServer << std::endl;
		if (client._responseLocation)
			stream << *client._responseLocation << std::endl;
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

	bool isKeepAlive();
	void responseSent();

	const Server *_acceptServer;
	const Server *_responseServer;
	const Location *_responseLocation;
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
