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

class Manager {

public:
	typedef void (Manager::*_func)(Client*);
	typedef std::map<int, _func> _mgrFuncType;

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

	Data* _data;
	Cgi* _cgi;
	Request* _request;
	Response* _response;
	const Server::_svrsType* _server;
	_mgrFuncType _funcMap;
	fd_set 	_readSet,
			_writeSet;
	Client::_clientsType _clients;
};
