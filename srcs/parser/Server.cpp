#include "parser/Server.hpp"

Server::Server() {
	_delim = "\t ";
}

Server::~Server() {
	_serverFuncMap.insert(std::make_pair("client_max_body_size", &Server::parseClientMaxBodySize));
	_serverFuncMap.insert(std::make_pair("listen", &Server::parseListenPorts));
	_serverFuncMap.insert(std::make_pair("server_name", &Server::parseServerNames));
	_serverFuncMap.insert(std::make_pair("root", &Server::parseRoot));
	_serverFuncMap.insert(std::make_pair("autoindex", &Server::parseAutoindex));
}

Server::Server(const Server &other):_splitBuffer(other._splitBuffer),
									_buffer(other._buffer),
									_clientMaxBodySize(other._clientMaxBodySize),
									_listenPorts(other._listenPorts),
									_serverNames(other._serverNames),
									_root(other._root),
									_autoindex(other._autoindex) {}

Server &Server::operator=(const Server &other) {
	if (this != &other) {
		_splitBuffer = other._splitBuffer;
		_buffer = other._buffer;
		_clientMaxBodySize = other._clientMaxBodySize;
		_listenPorts = other._listenPorts;
		_serverNames = other._serverNames;
		_root = other._root;
		_autoindex = other._autoindex;
	}
	return (*this);
}

long Server::getClientMaxBodySize() const {
	return (_clientMaxBodySize);
}

void Server::setClientMaxBodySize(long clientMaxBodySize) {
	_clientMaxBodySize = clientMaxBodySize;
}

const std::vector<long> &Server::getListenPorts() const {
	return (_listenPorts);
}

void Server::setListenPorts(const std::vector<long> &listenPorts) {
	_listenPorts = listenPorts;
}

const std::vector<std::string> &Server::getServerNames() const {
	return (_serverNames);
}

void Server::setServerNames(const std::vector<std::string> &serverNames) {
	_serverNames = serverNames;
}

const std::string &Server::getRoot() const {
	return (_root);
}

void Server::setRoot(const std::string &root) {
	_root = root;
}

const std::string &Server::getAutoindex() const {
	return (_autoindex);
}

void Server::setAutoindex(const std::string &autoindex) {
	_autoindex = autoindex;
}

const Server::_locationsType &Server::getLocations() const {
	return (_locations);
}

void Server::setLocations(const Server::_locationsType &locations) {
	_locations = locations;
}

void Server::parseClientMaxBodySize(std::vector<std::string> &splitBuffer) {
    _clientMaxBodySize = strToLong(trim(splitBuffer[1], "m"));
}

void Server::parseListenPorts(std::vector<std::string> &splitBuffer) {
    for (int i = 1; i < splitBuffer.size(); ++i)
        _listenPorts.push_back(strToLong(splitBuffer[i]));
}

void Server::parseServerNames(std::vector<std::string> &splitBuffer) {
    for (int i = 1; i < splitBuffer.size(); ++i)
        _serverNames.push_back(splitBuffer[i]);
}

void Server::parseRoot(std::vector<std::string> &splitBuffer) {
    _root = splitBuffer[1];
}

void Server::parseAutoindex(std::vector<std::string> &splitBuffer) {
    _autoindex = splitBuffer[1] == "on";
}

std::vector<Server> Server::parseConfiguration(const std::string &config) {
	std::vector<Server> servers;
	static int fd;

	fd = open(config.c_str(), O_RDONLY);
	if (fd < 0) {
		std::cerr << "Critical error - fd negative - parseMimeTypes" << std::endl;
		exit(1);
	}

    while (parseLine(fd, _buffer) > 0) {
        _splitBuffer = split(_buffer, _delim);
		if (matchPattern(e_server, _splitBuffer))
			servers.push_back(parseServer(fd));
	}
	return (servers);
}

Server Server::parseServer(int fd) {
	Server server;

	while (parseLine(fd, _buffer) > 0) {
		_splitBuffer = split(_buffer, _delim);
        if (matchPattern(e_end, _splitBuffer))
            break;
        if (matchPattern(e_location, _splitBuffer))
            server._locations.push_back(Location().parseLocation(fd, _splitBuffer));
        if (this->_serverFuncMap.find(_splitBuffer[0]) != _serverFuncMap.end())
            (this->*_serverFuncMap.find(_splitBuffer[0])->second)(_splitBuffer);
	}
	std::cout << "Server" << std::endl;
	std::cout << server << std::endl;
	return (server);
}


