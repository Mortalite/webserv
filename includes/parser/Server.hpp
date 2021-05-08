#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "utils/utils.hpp"
#include "parser/Location.hpp"
#include "parser/Base.hpp"

struct Server:public Base {
	typedef void (Server::*_func)(std::vector<std::string>&);
	typedef std::map<std::string, _func> _svrFuncType;
	typedef std::vector<Server> _svrsType;
	typedef _svrsType::iterator _svrsIt;

	Server();
	Server(const Server& other);
	~Server();

	Server& operator=(const Server& other);
	friend std::ostream& operator<<(std::ostream& stream, const Server& server) {
		stream << RED << "Server" << RESET << std::endl;
		stream << "_host = " << server._host << std::endl;
		stream << "_clientMaxBodySize = " << server._clientMaxBodySize << std::endl;
		std::cout << "_listenPort = " << server._listenPort << std::endl;
		printContainer(stream, "_serverName", server._serverName);
		stream << "_root = " << server._root << std::endl;
		printContainer(stream, "_allowed_method", server._allowed_method);
		printContainer(stream, "_index", server._index);
		stream << "_autoindex = " << server._autoindex << std::endl;
		stream << RED << "Locations" << RESET << std::endl;

		size_t counter = 0;
		for (	Location::_locsType::const_iterator it = server._locations.begin();
				it != server._locations.end(); it++)
			stream << "Location" << "[" << counter++ << "]\n" << *it << std::endl;
		return (stream);
	}

	void parseHost(std::vector<std::string> &splitBuffer);
	void parseClientMaxBodySize(std::vector<std::string> &splitBuffer);
	void parseListenPorts(std::vector<std::string> &splitBuffer);
	void parseServerNames(std::vector<std::string> &splitBuffer);

	Server& parseServer(int fd);
	void setServerConfig();

	std::vector<std::string> 	_splitBuffer,
								_serverName;
	std::string _buffer,
				_host;
	long 	_clientMaxBodySize,
			_listenPort;
	Location::_locsType _locations;
	_svrFuncType _svrFuncMap;
};
