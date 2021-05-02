#include "parser/Server.hpp"

Server::Server():Base() {
	_delim = "\t ";
	_serverFuncMap.insert(std::make_pair("host", &Server::parseHost));
	_serverFuncMap.insert(std::make_pair("client_max_body_size", &Server::parseClientMaxBodySize));
	_serverFuncMap.insert(std::make_pair("listen", &Server::parseListenPorts));
	_serverFuncMap.insert(std::make_pair("server_name", &Server::parseServerNames));
	_serverFuncMap.insert(std::make_pair("root", &Server::parseRoot));
	_serverFuncMap.insert(std::make_pair("allowed_method", &Server::parseAllowedMethod));
	_serverFuncMap.insert(std::make_pair("index", &Server::parseIndex));
	_serverFuncMap.insert(std::make_pair("autoindex", &Server::parseAutoindex));
}

Server::~Server() {}

Server::Server(const Server &other):Base(other._root,
										 other._allowed_method,
										 other._index,
										 other._autoindex),
									_splitBuffer(other._splitBuffer),
									_buffer(other._buffer),
								 	_delim("\t "),
								 	_host(other._host),
								 	_clientMaxBodySize(other._clientMaxBodySize),
								 	_listenPort(other._listenPort),
								 	_serverName(other._serverName),
								 	_locations(other._locations) {}

Server &Server::operator=(const Server &other) {
	if (this != &other) {
		Base::operator=(other);
		_splitBuffer = other._splitBuffer;
		_buffer = other._buffer;
		_delim = other._delim;
		_host = other._host;
		_clientMaxBodySize = other._clientMaxBodySize;
		_listenPort = other._listenPort;
		_serverName = other._serverName;
		_locations = other._locations;
	}
	return (*this);
}

void Server::parseHost(std::vector<std::string> &splitBuffer) {
	_host = splitBuffer[1];
}

void Server::parseClientMaxBodySize(std::vector<std::string> &splitBuffer) {
	_clientMaxBodySize = strToLong(trim(splitBuffer[1], "m"))*1000*1000;
}

void Server::parseListenPorts(std::vector<std::string> &splitBuffer) {
	_listenPort = strToLong(splitBuffer[1]);
}

void Server::parseServerNames(std::vector<std::string> &splitBuffer) {
	for (size_t i = 1; i < splitBuffer.size(); ++i)
		_serverName.push_back(splitBuffer[i]);
}

Server& Server::parseServer(int fd) {
	while (parseLine(fd, _buffer) > 0) {
		_splitBuffer = split(_buffer, _delim);
		if (matchPattern(e_end, _splitBuffer))
			break;
		if (matchPattern(e_location, _splitBuffer))
			_locations.push_back(Location().parseLocation(fd, _splitBuffer));
		if (!_splitBuffer.empty() && this->_serverFuncMap.find(_splitBuffer[0]) != _serverFuncMap.end())
			(this->*_serverFuncMap.find(_splitBuffer[0])->second)(_splitBuffer);
	}
	return (*this);
}

void Server::setServerConfig() {
	if (_host.empty())
		_host = "localhost";
	for (	Location::_locationsIt locationsIt = _locations.begin();
			locationsIt != _locations.end(); locationsIt++) {
		if ((*locationsIt)._root.empty())
			(*locationsIt)._root = _root;
		if ((*locationsIt)._allowed_method.empty())
			(*locationsIt)._allowed_method = _allowed_method;
		if ((*locationsIt)._index.empty())
			(*locationsIt)._index = _index;
		if ((*locationsIt)._autoindex)
			(*locationsIt)._autoindex = _autoindex;
	}
}


