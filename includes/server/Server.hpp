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

/*
** Пока порт задан макросом, надо будет читать из файла конфигурации
*/
#define PORT 8080

class Server {

private:
	struct sockaddr_in _address;
	fd_set _readSet;//, write_set;

	size_t BODY_BUFFER;

	std::list<Client*> _clients;
	typedef std::list<Client*> _clientsType;
	std::vector<char> _buffer;
	static int _signal;

public:
	Server();
	~Server();

	int getSignal();
	static void setSignal(int signal);
	void removeClient(_clientsType::iterator& it);

	int recvHeaders(_clientsType::iterator& it);
	int recvBody(_clientsType::iterator& it);
	int recvChunkedBody(_clientsType::iterator& it);
	int runServer();


};


#endif
