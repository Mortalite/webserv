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
#include <vector>

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
		e_chunked_body,
		e_multipart
	};

	enum CHUNKED {
		e_chunk_data,
		e_chunk_hex
	};

	struct sockaddr_in address;
	fd_set readSet;//, write_set;

	size_t BODY_BUFFER;

	std::list<Client*> _clients;
	typedef std::list<Client*> _clientsType;
	std::vector<char> _buffer;

public:
	Server();
	~Server();

	int recvHeaders(_clientsType::iterator& it);
	int recvBody(_clientsType::iterator& it);
	int recvChunkedBody(_clientsType::iterator& it);
	int runServer();

};

#endif
