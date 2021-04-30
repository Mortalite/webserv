#pragma once

#include <algorithm>
#include <list>
#include <map>
#include <cstring>
#include <cstdio>
#include <csignal>
#include <cerrno>
#include <sys/socket.h>

#include "parser/Request.hpp"
#include "parser/Response.hpp"
#include "parser/Server.hpp"
#include "server/Client.hpp"
#include "utils/Data.hpp"
/*
** Пока порт задан макросом, надо будет читать из файла конфигурации
*/
#define PORT 9000

class Manager {

public:
	typedef void (Manager::*_func)(Client*);
	typedef std::map<int, _func> _funcType;

	Manager(Data *data);
	Manager(const Manager &other);
	~Manager();

	Manager& operator=(const Manager &other);

	static int& getSignal();
	int launchManager();


private:
	void initSet(Client *client);
	void recvHeaders(Client *client);
	void recvContentBody(Client *client);
	void recvChunkBody(Client *client);
	void sendResponse(Client *client);
	void closeConnection(Client::_clientIt &clientIt);

	_funcType _funcMap;
	struct sockaddr_in _address;
	fd_set _readSet;
	fd_set _writeSet;

	Data* _data;
	Request* _request;
	Response* _response;
	const Data::_serversType* _server;
	Client::_clientsType _clients;

};
