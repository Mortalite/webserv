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
#define PORT 8080

class Server {

	public:
		Server(Data* data);
		~Server();

		static int& getSignal();
		void setData(Data* data);

		void closeConnection(Client::_clientsType::iterator& client_it);
		void recvHeaders(Client::_clientsType::iterator& it);
		void recvContentBody(Client::_clientsType::iterator& it);
		void recvChunkBody(Client::_clientsType::iterator& it);
		void sendResponse(Client::_clientsType::iterator& it);
		int runServer();

		void initSet(Client::_clientsType::iterator& client_it);


	private:
		typedef void (Server::*_func)(Client::_clientsType::iterator&);
		typedef std::map<int, _func> _funcType;

		_funcType _funcMap;

		struct sockaddr_in _address;
		fd_set _readSet;
		fd_set _writeSet;

		long BODY_BUFFER;

		Client::_clientsType _clients;
		std::vector<char> _buffer;
		Data* _data;

};


#endif
