#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>
#include <fcntl.h>
#include "utils/utils.hpp"

class Server {

	public:
		typedef std::map<std::string, std::string> _serverMapType;
		typedef _serverMapType::const_iterator _serverMapIt;

		Server();
		Server(const Server& other);
		~Server();

        Server& operator=(const Server& other);

        Server parseServer(int fd);
        std::vector<Server> parseConfiguration(const std::string& config = "config/webserv.conf");


	private:
        std::vector<std::string> _serverVecBegin;
        std::vector<std::string> _locationVecBegin;
        std::vector<std::string> _endVec;
        std::vector<std::string> _splitBuffer;
        std::string _buffer;

        long _maxFileSize;
		std::string _host;
		std::vector<std::string> _serverName;
		std::string _root;
		std::string _autoindex;
		_serverMapType _serverMap;

};

#endif