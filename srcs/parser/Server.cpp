#include "parser/Server.hpp"

Server::Server():Base() {
	_svrFuncMap.insert(std::make_pair("host", &Server::parseHost));
	_svrFuncMap.insert(std::make_pair("client_max_body_size", &Server::parseClientMaxBodySize));
	_svrFuncMap.insert(std::make_pair("listen", &Server::parseListenPorts));
	_svrFuncMap.insert(std::make_pair("server_name", &Server::parseServerNames));
	_svrFuncMap.insert(std::make_pair("root", &Server::parseRoot));
	_svrFuncMap.insert(std::make_pair("error_page", &Server::parseCustomErrors));
	_svrFuncMap.insert(std::make_pair("allowed_method", &Server::parseAllowedMethod));
	_svrFuncMap.insert(std::make_pair("index", &Server::parseIndex));
	_svrFuncMap.insert(std::make_pair("autoindex", &Server::parseAutoindex));
}

Server::~Server() {}

Server::Server(const Server &other): Base(other._root,
										 other._error_page,
										 other._allowed_method,
										 other._index,
										 other._autoindex),
									 _splitBuffer(other._splitBuffer),
									 _buffer(other._buffer),
									 _host(other._host),
									 _clientMaxBodySize(other._clientMaxBodySize),
									 _listenPort(other._listenPort),
									 _serverName(other._serverName),
									 _locations(other._locations),
									 _svrFuncMap(other._svrFuncMap) {}

Server &Server::operator=(const Server &other) {
	if (this != &other) {
		Base::operator=(other);
		_splitBuffer = other._splitBuffer;
		_buffer = other._buffer;
		_host = other._host;
		_clientMaxBodySize = other._clientMaxBodySize;
		_listenPort = other._listenPort;
		_serverName = other._serverName;
		_locations = other._locations;
		_svrFuncMap = other._svrFuncMap;
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
		_splitBuffer = split(_buffer, delimConfig);
		if (matchPattern(e_end, _splitBuffer))
			break;
		if (matchPattern(e_location, _splitBuffer))
			_locations.push_back(Location().parseLocation(fd, _splitBuffer));
		if (!_splitBuffer.empty() && this->_svrFuncMap.find(_splitBuffer[0]) != _svrFuncMap.end())
			(this->*_svrFuncMap.find(_splitBuffer[0])->second)(_splitBuffer);
	}
	return (*this);
}

void Server::setServerConfig() {
	if (_host.empty())
		_host = "localhost";
	for (	Location::_locsIt locationsIt = _locations.begin();
			locationsIt != _locations.end(); locationsIt++) {
		if ((*locationsIt)._root.empty())
			(*locationsIt)._root = _root;
		if ((*locationsIt)._error_page.empty())
			(*locationsIt)._error_page = _error_page;
		if ((*locationsIt)._allowed_method.empty())
			(*locationsIt)._allowed_method = _allowed_method;
		if ((*locationsIt)._index.empty())
			(*locationsIt)._index = _index;
		if (!(*locationsIt)._autoindex)
			(*locationsIt)._autoindex = _autoindex;
	}
}
