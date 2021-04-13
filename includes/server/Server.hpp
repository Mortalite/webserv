#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <algorithm>
#include <list>
#include <map>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

#include "parser/Request.hpp"
#include "server/Client.hpp"
#include "utils/Data.hpp"
#include "utils/HttpStatusCode.hpp"
#include "utils/utils.hpp"
/*
** Пока порт задан макросом, надо будет читать из файла конфигурации
*/
#define PORT 9000

class Server {

	private:
		typedef void (Server::*_func)(Client::_clientsType::iterator&);
		typedef std::map<int, _func> _funcType;

	public:
		Server(Data* data);
		~Server();

		static int& getSignal();
		Request* getRequest();
		void setData(Data* data);

		void initSet(Client::_clientIt &clientIt);
		void recvHeaders(Client::_clientIt &clientIt);
		void recvContentBody(Client::_clientIt &clientIt);
		void recvChunkBody(Client::_clientIt &clientIt);
		void sendResponse(Client::_clientIt &clientIt);
		void closeConnection(Client::_clientIt &clientIt);
		int runServer();

	private:

		_funcType _funcMap;

		struct sockaddr_in _address;
		fd_set _readSet;
		fd_set _writeSet;

		long BODY_BUFFER;

		Data* _data;
		Request* _request;
		Client::_clientsType _clients;
		std::vector<char> _buffer;

};


#endif
