#include "utils/Data.hpp"

Data::Data() {
	/*
	** Коды состояния
	 */
	_httpMap["200"] = new Node(e_success, "OK");
	_httpMap["201"] = new Node(e_success, "Created");
	_httpMap["204"] = new Node(e_success, "No Content");
	_httpMap["301"] = new Node(e_redirection, "Moved Permanently");
	_httpMap["302"] = new Node(e_redirection, "Found");
	_httpMap["400"] = new Node(e_clientError, "Bad Request");
	_httpMap["401"] = new Node(e_clientError, "Unauthorized");
	_httpMap["403"] = new Node(e_clientError, "Forbidden");
	_httpMap["404"] = new Node(e_clientError, "Not Found");
	_httpMap["405"] = new Node(e_clientError, "Method Not Allowed");
	_httpMap["413"] = new Node(e_clientError, "Request Entity Too Large");
	_httpMap["500"] = new Node(e_clientError, "Internal server error");

	for (_httpMapIt httpMapIt = _httpMap.begin(); httpMapIt != _httpMap.end(); httpMapIt++) {
		if (isErrorStatus(httpMapIt))
			httpMapIt->second->setPath(errorsDirectory+"/"+httpMapIt->first+".html");
	}
}

Data::Data(const Data &other):  _buffer(other._buffer),
								_splitBuffer(other._splitBuffer),
								_fd(other._fd),
								_mimeMap(other._mimeMap),
								_httpMap(other._httpMap) {}

Data::~Data() {
	for (_httpMapIt httpMapIt = _httpMap.begin(); httpMapIt != _httpMap.end();) {
		delete (*httpMapIt).second;
		_httpMap.erase(httpMapIt++);
	}
}

Data &Data::operator=(const Data &other) {
	if (this != &other) {
		_buffer = other._buffer;
		_splitBuffer = other._splitBuffer;
		_fd = other._fd;
		_mimeMap = other._mimeMap;
		_httpMap = other._httpMap;
	}
	return (*this);
}

const Data::_mimeMapType& Data::getMimeMap() const {
	return (_mimeMap);
}

std::string Data::getMessage(const HttpStatusCode &httpStatusCode) const {
	return (_httpMap.find(httpStatusCode.getStatusCode())->second->getName());
}

std::string Data::getErrorPath(const Client *client) const {
	std::vector<std::pair<std::string, std::string> >::const_iterator customErrorIt;

	for (	customErrorIt = client->_respLoc->_error_page.begin();
			 customErrorIt != client->_respLoc->_error_page.end();
			 customErrorIt++) {
		if ((*customErrorIt).first == client->_httpStatusCode.getStatusCode())
			return ((*customErrorIt).second);
	}
	return (_httpMap.find(client->_httpStatusCode.getStatusCode())->second->getPath());
}

const std::vector<Server> &Data:: getServers() const {
	return (_servers);
}

bool Data::isErrorStatus(const HttpStatusCode *httpStatusCode) const {
	static int type;

	type = _httpMap.find(httpStatusCode->getStatusCode())->second->getType();
	return (type == e_clientError || type == e_serverError);
}

bool Data::isErrorStatus(const Data::_httpMapIt &httpMapIt) const {
	static int type;

	type = httpMapIt->second->getType();
	return (type == e_clientError || type == e_serverError);
}

bool Data::isErrorStatus(const Client *client) const {
	return (isErrorStatus(&client->_httpStatusCode));
}

void Data::parseMimeTypes(const std::string& mimeTypes) {
	try {
		if ((_fd = open(mimeTypes.c_str(), O_RDONLY)) == -1)
			TraceException("open failed");

		while (ft::parseLine(_fd, _buffer) > 0) {
			_splitBuffer = ft::split(_buffer, delimConfig);
			if (ft::matchPattern(e_mime, _splitBuffer)) {
				while (ft::parseLine(_fd, _buffer) > 0) {
					if (ft::matchPattern(e_end, _splitBuffer))
						break;
					_splitBuffer = ft::split(_buffer, delimConfig);
					for (size_t i = 1; i < _splitBuffer.size(); i++)
						_mimeMap[_splitBuffer[i]] = _splitBuffer[0];
				}
			}
		}
	}
	catch (std::runtime_error& runtimeError) {
		std::cerr << "criticalError: " << runtimeError.what();
		exit(EXIT_FAILURE);
	}
}

void Data::parseConfiguration(const std::string &configuration) {
	try {
		if ((_fd = open(configuration.c_str(), O_RDONLY)) == -1)
			TraceException("open failed");

		while (ft::parseLine(_fd, _buffer) > 0) {
			_splitBuffer = ft::split(_buffer, delimConfig);
			if (ft::matchPattern(e_server, _splitBuffer))
				_servers.push_back(Server().parseServer(_fd));
		}
		for (Server::_svrsIt serverIt = _servers.begin(); serverIt != _servers.end(); serverIt++)
			(*serverIt).setServerConfig();
		for (Server::_svrsIt it = _servers.begin(); it != _servers.end(); it++)
			std::cout << *it << std::endl;
	}
	catch (std::runtime_error& runtimeError) {
		std::cerr << "criticalError: " << runtimeError.what();
		exit(EXIT_FAILURE);
	}
}
