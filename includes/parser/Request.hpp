#pragma once

#include "server/Client.hpp"
#include "utils/HttpStatusCode.hpp"

class Request {

public:
	Request(const Data* data);
	Request(const Request& other);
	~Request();

	Request& operator=(const Request& other);

	void recvHeaders(Client *client);
	void recvContentBody(Client *client);
	void recvChunkBody(Client *client);
	void parseHeaders(Client *client);

	std::pair<int, long> getBodyType(Client *client);

private:
	const Data *_data;
	const Server::_serversType *_servers;
	long _bodyBuffer;
	std::vector<char> _buffer;
};
