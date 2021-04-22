#include "parser/Server.hpp"

Server::Server() {
	static std::string serverBegin[] = {"server", "{"};
	static std::string locationBegin[] = {"location", "{"};
	static std::string end[] = {"}"};

	arrayToVec(serverBegin, _serverVecBegin);
	arrayToVec(locationBegin, _locationVecBegin);
	arrayToVec(end, _endVec);
}

Server::~Server() {}

Server::Server(const Server &other):_serverVecBegin(other._serverVecBegin),
                                    _locationVecBegin(other._locationVecBegin),
                                    _endVec(other._endVec),
                                    _splitBuffer(other._splitBuffer),
                                    _buffer(other._buffer),
                                    _maxFileSize(other._maxFileSize),
                                    _host(other._host),
                                    _serverName(other._serverName),
                                    _root(other._root),
                                    _autoindex(other._autoindex),
                                    _serverMap(other._serverMap) {}

Server &Server::operator=(const Server &other) {
    if (this != &other) {
        _serverVecBegin = other._serverVecBegin;
        _locationVecBegin = other._locationVecBegin;
        _endVec = other._endVec;
        _splitBuffer = other._splitBuffer;
        _buffer = other._buffer;
        _maxFileSize = other._maxFileSize;
        _host = other._host;
        _serverName = other._serverName;
        _root = other._root;
        _autoindex = other._autoindex;
        _serverMap = other._serverMap;
    }
    return (*this);
}


Server Server::parseServer(int fd) {
	Server server;

/*    static std::string end[] = {"}"};
	static std::vector<std::string> _endVec(end, end + sizeof(end) / sizeof(end[0]));

	while (parseLine(fd, _buffer) > 0) {
		_splitBuffer = split(_buffer, delim);
		if (_splitBuffer == _serverVecBegin) {
			while (parseLine(fd, _buffer) > 0 && _splitBuffer != _endVec) {
				_splitBuffer = split(_buffer, delim);
				for (size_t i = 1; i < _splitBuffer.size(); i++)
					_serverMap[_splitBuffer[0]] = _splitBuffer[i];
			}
		}

	}*/
	return (server);
}

std::vector<Server> Server::parseConfiguration(const std::string &config) {
	std::vector<Server> servers;
	static std::string delim(" \t");
	static int fd;

	fd = open(config.c_str(), O_RDONLY);
	if (fd < 0) {
		std::cerr << "Critical error - fd negative - parseMimeTypes" << std::endl;
		exit(1);
	}

	while (parseLine(fd, _buffer) > 0) {
		_splitBuffer = split(_buffer, delim);
		if (_splitBuffer == _serverVecBegin) {
			while (parseLine(fd, _buffer) > 0 && _splitBuffer != _endVec) {
				_splitBuffer = split(_buffer, delim);
				for (size_t i = 1; i < _splitBuffer.size(); i++)
					_serverMap[_splitBuffer[0]] = _splitBuffer[i];
			}
		}
	}

	for (_serverMapIt it = _serverMap.begin(); it != _serverMap.end(); it++)
		std::cout << "first = " << it->first << ", second = " << it->second << std::endl;

	return (servers);
}

