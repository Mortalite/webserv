#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <cerrno>
#include <algorithm>
#include <list>
#include <map>

#include "server/Client.hpp"
#include "utils/utils.hpp"
#include "parser/Request.hpp"

/*
** Пока порт задан макросом, надо будет читать из файла конфигурации
*/
#define PORT 8080

class Server {

private:

	/*
	** Флаги чтения запроса
	*/
	enum FLAGS {
		e_headers,
		e_content_body,
		e_chunked_body
	};

	struct sockaddr_in address;
	fd_set read_set;//, write_set;

	static const int BODY_BUFFER = 4096;

	std::list<Client*> _clients;
	typedef std::list<Client*> clients_type;

public:
	Server();
	~Server();

	int recv_headers(clients_type::iterator& i);
	int recv_body(clients_type::iterator& i);
	int run_server();

};

#endif
