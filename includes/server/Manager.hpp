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
	static void launchManager();

private:
	static int& getSignal();
	static void	signalCtrlC(int signal);
	static void signalMain();
	void recvHeaders(Client *client);
	void recvContentBody(Client *client);
	void recvChunkBody(Client *client);
	void sendResponse(Client *client);
	void closeConnection(Client::_clientIt &clientIt);
	void initSet(Client *client);
	int runWebserver();

	Data* _data;
	Cgi* _cgi;
	Request* _request;
	Response* _response;
	const Server::_svrsType* _server;
	Client::_clientsType _clients;
	fd_set 	_readSet,
			_writeSet;
	_mgrFuncType _funcMap;
};
