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
	virtual std::ostream& print(std::ostream &out) const;

	void parseHost(std::vector<std::string> &splitBuffer);
	void parseListenPorts(std::vector<std::string> &splitBuffer);
	void parseServerNames(std::vector<std::string> &splitBuffer);

	Server& parseServer(int fd);
	void setServerConfig();

	std::vector<std::string> 	_splitBuffer,
								_serverName;
	std::string _buffer,
				_host;
	long 	_listenPort;
	Location::_locsType _locations;
	_svrFuncType _svrFuncMap;
};
