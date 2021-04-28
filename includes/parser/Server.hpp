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

	Server();
	Server(const Server& other);
	~Server();

	Server& operator=(const Server& other);
	friend std::ostream& operator<<(std::ostream& stream, const Server& server) {
		size_t counter = 0;

		std::cout << "_clientMaxBodySize = " << server._clientMaxBodySize << std::endl;
		printContainer("_listenPorts", server._listenPorts);
		printContainer("_serverNames", server._serverNames);

		std::cout << "_clientMaxBodySize = " << server._clientMaxBodySize << std::endl;
		std::cout << "_clientMaxBodySize = " << server._clientMaxBodySize << std::endl;
		std::cout << "_clientMaxBodySize = " << server._clientMaxBodySize << std::endl;

		for (_locationsType::const_iterator it = server._locations.begin(); it != server._locations.end(); it++)
			stream << "Location[" << counter++ << "]\n" << *it << std::endl;
		return (stream);
	}

	long getClientMaxBodySize() const;
	void setClientMaxBodySize(long clientMaxBodySize);
	const std::vector<long> &getListenPorts() const;
	void setListenPorts(const std::vector<long> &listenPorts);
	const std::vector<std::string> &getServerNames() const;
	void setServerNames(const std::vector<std::string> &serverNames);
	const std::string &getRoot() const;
	void setRoot(const std::string &root);
	const std::string &getAutoindex() const;
	void setAutoindex(const std::string &autoindex);
	const _locationsType &getLocations() const;
	void setLocations(const _locationsType &locations);

	void parseClientMaxBodySize(std::vector<std::string> &splitBuffer);
	void parseListenPorts(std::vector<std::string> &splitBuffer);
	void parseServerNames(std::vector<std::string> &splitBuffer);
	void parseRoot(std::vector<std::string> &splitBuffer);
	void parseAutoindex(std::vector<std::string> &splitBuffer);

	std::vector<Server> parseConfiguration(const std::string& config = "config/webserv.conf");

private:
	Server parseServer(int fd);

	std::vector<std::string> _splitBuffer;
	std::string _buffer;
	std::string _delim;

	long _clientMaxBodySize;
	std::vector<long> _listenPorts;
	std::vector<std::string> _serverNames;
	std::string _root;
	std::string _autoindex;
	_locationsType _locations;
	_serverFuncType _serverFuncMap;
};

#endif