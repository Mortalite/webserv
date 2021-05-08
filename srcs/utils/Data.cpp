#include "utils/Data.hpp"

Data::Data() {
	/*
	** Коды состояния
	 */
	_httpMap["200"] = new Node(e_success, "OK");
	_httpMap["201"] = new Node(e_success, "Created");
	_httpMap["301"] = new Node(e_redirection, "Moved Permanently");
	_httpMap["400"] = new Node(e_clientError, "Bad Request");
	_httpMap["403"] = new Node(e_clientError, "Forbidden");
	_httpMap["404"] = new Node(e_clientError, "Not Found");
	_httpMap["405"] = new Node(e_clientError, "Method Not Allowed");
	_httpMap["413"] = new Node(e_clientError, "Request Entity Too Large");

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

const Data::_httpMapType &Data::getHttpMap() const {
	return (_httpMap);
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

std::string Data::getErrorPath(const HttpStatusCode *httpStatusCode) const {
	return (_httpMap.find(httpStatusCode->getStatusCode())->second->getPath());
}

const std::vector<Server> &Data::getServers() const {
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

void Data::parseMimeTypes(const std::string& mimeTypes) {
	_fd = open(mimeTypes.c_str(), O_RDONLY);
	if (_fd < 0) {
		std::cout << "Critical error - fd negative - parseMimeTypes" << std::endl;
		exit(1);
	}

	while (parseLine(_fd, _buffer) > 0) {
		_splitBuffer = split(_buffer, delimConfig);
		if (matchPattern(e_mime, _splitBuffer)) {
			while (parseLine(_fd, _buffer) > 0) {
				if (matchPattern(e_end, _splitBuffer))
					break;
				_splitBuffer = split(_buffer, delimConfig);
				for (size_t i = 1; i < _splitBuffer.size(); i++)
					_mimeMap[_splitBuffer[i]] = _splitBuffer[0];
			}
		}
	}
}

void Data::parseConfiguration(const std::string &configuration) {
	_fd = open(configuration.c_str(), O_RDONLY);
	if (_fd < 0) {
		std::cerr << "Critical error - fd negative - parseConfiguration" << std::endl;
		exit(1);
	}

	while (parseLine(_fd, _buffer) > 0) {
		_splitBuffer = split(_buffer, delimConfig);
		if (matchPattern(e_server, _splitBuffer))
			_servers.push_back(Server().parseServer(_fd));
	}
	for (Server::_svrsIt serverIt = _servers.begin(); serverIt != _servers.end(); serverIt++)
		(*serverIt).setServerConfig();
//	for (Server::_svrsIt it = _servers.begin(); it != _servers.end(); it++)
//		std::cout << *it << std::endl;

}
