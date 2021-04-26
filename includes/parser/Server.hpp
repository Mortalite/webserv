#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <cstdlib>
#include <map>
#include <fcntl.h>
#include "utils/utils.hpp"
#include "parser/Location.hpp"

class Server {

public:
	typedef std::list<Location> _locationsType;

	Server();
	Server(const Server& other);
	~Server();

	Server& operator=(const Server& other);

	Server parseServer(int fd);
	std::vector<Server> parseConfiguration(const std::string& config = "config/webserv.conf");

	friend std::ostream& operator<<(std::ostream& stream, const Server& server) {
		size_t counter = 0;

		for (_locationsType::const_iterator it = server._locations.begin(); it != server._locations.end(); it++)
			stream << "Location[" << counter++ << "]\n" << *it << std::endl;
		return (stream);
	}

private:
	std::vector<std::string> _splitBuffer;
	std::string _buffer;
	std::string _delim;

	long _maxFileSize;
	std::string _host;
	std::vector<std::string> _serverName;
	std::string _root;
	std::string _autoindex;
	_locationsType _locations;
};

#endif