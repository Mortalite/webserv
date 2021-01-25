#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

#include "utils/utils.hpp"
#include "parser/Request.hpp"

#define PORT 8080

class Server {

private:
	struct sockaddr_in address;
	fd_set read_set;//, write_set;
	int max_sd;

public:
	Server();
	~Server();

	int runServer();
};

#endif