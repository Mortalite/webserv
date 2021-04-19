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
		~Server();

		void parseConfiguration(const std::string& config = "config/webserv.conf");

	private:
		long _maxFileSize;
		std::string _host;
		std::vector<std::string> _serverName;
		std::string _root;
		std::string _autoindex;
		_serverMapType _serverMap;

};

#endif