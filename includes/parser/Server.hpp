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
	typedef void (Server::*_func)(std::vector<std::string>&);
	typedef std::map<std::string, _func> _serverFuncType;
	typedef std::list<Location> _locationsType;
	typedef _locationsType::iterator _locationsIt;

	Server();
	Server(const Server& other);
	~Server();

	Server& operator=(const Server& other);
	friend std::ostream& operator<<(std::ostream& stream, const Server& server) {
		stream << RED << "Server" << RESET << std::endl;
		stream << "_host = " << server._host << std::endl;
		stream << "_clientMaxBodySize = " << server._clientMaxBodySize << std::endl;
		std::cout << "_listenPort = " << server._listenPort << std::endl;
		printContainer(stream, "_index", server._index);
		printContainer(stream, "_serverName", server._serverName);
		stream << "_root = " << server._root << std::endl;
		stream << "_autoindex = " << server._autoindex << std::endl;
		stream << RED << "Locations" << RESET << std::endl;

		size_t counter = 0;
		for (_locationsType::const_iterator it = server._locations.begin(); it != server._locations.end(); it++)
			stream << "Location" << "[" << counter++ << "]\n" << *it << std::endl;
		return (stream);
	}

	void parseHost(std::vector<std::string> &splitBuffer);
	void parseClientMaxBodySize(std::vector<std::string> &splitBuffer);
	void parseListenPorts(std::vector<std::string> &splitBuffer);
	void parseServerNames(std::vector<std::string> &splitBuffer);
	void parseRoot(std::vector<std::string> &splitBuffer);
	void parseIndex(std::vector<std::string> &splitBuffer);
	void parseAutoindex(std::vector<std::string> &splitBuffer);

	Server& parseServer(int fd);
	void setServerConfig();

	std::vector<std::string> _splitBuffer;
	std::string _buffer;
	std::string _delim;

	std::string _host;
	long _clientMaxBodySize;
	long _listenPort;
	std::vector<std::string> _serverName;
	std::string _root;
	std::vector<std::string> _index;
	bool _autoindex;
	_locationsType _locations;
	_serverFuncType _serverFuncMap;
};

#endif