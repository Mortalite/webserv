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

#include "server/Client.hpp"
#include "utils/utils.hpp"
#include "parser/Request.hpp"
#include "utils/HttpStatusCode.hpp"
/*
** Пока порт задан макросом, надо будет читать из файла конфигурации
*/
#define PORT 8080

class Server {

	public:
		Server();
		~Server();

		static int& getSignal();
		void removeClient(Client::_clientsType::iterator& it);

		int recvHeaders(Client::_clientsType::iterator& it);
		int recvBody(Client::_clientsType::iterator& it);
		int recvChunkedBody(Client::_clientsType::iterator& it);
		int runServer();

	private:
		struct sockaddr_in _address;
		fd_set _readSet;//, write_set;

		size_t BODY_BUFFER;

		Client::_clientsType _clients;
		std::vector<char> _buffer;
};


#endif
