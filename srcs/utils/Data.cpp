#include "utils/Data.hpp"

Data::Data() {
	_errorsDirectory = "./config/errors/";
	/*
	** Коды состояния
	 */
	_httpMap["200"] = new Node(e_success, "OK");
	_httpMap["201"] = new Node(e_success, "Created");
	_httpMap["400"] = new Node(e_clientError, "Bad Request");
	_httpMap["404"] = new Node(e_clientError, "Not Found");

	for (_httpMapIt httpMapIt = _httpMap.begin(); httpMapIt != _httpMap.end(); httpMapIt++) {
		if (isErrorStatus(httpMapIt))
			httpMapIt->second->setPath(_errorsDirectory + httpMapIt->first + ".html");
	}
}

Data::Data(const Data &other):  _mimeMap(other._mimeMap),
								_httpMap(other._httpMap),
								_errorsDirectory(other._errorsDirectory) {}

Data::~Data() {
	for (_httpMapIt httpMapIt = _httpMap.begin(); httpMapIt != _httpMap.end();) {
		delete (*httpMapIt).second;
		_httpMap.erase(httpMapIt++);
	}
}

Data &Data::operator=(const Data &other) {
	if (this != &other) {
		_mimeMap = other._mimeMap;
		_httpMap = other._httpMap;
		_errorsDirectory = other._errorsDirectory;
	}
	return (*this);
}

const Data::_mimeMapType& Data::getMimeMap() const {
	return (_mimeMap);
}

const Data::_httpMapType &Data::getHttpMap() const {
	return (_httpMap);
}

const std::string &Data::getErrorsDirectory() const {
	return (_errorsDirectory);
}

std::string Data::getMessage(const HttpStatusCode &httpStatusCode) const {
	return (_httpMap.find(httpStatusCode.getStatusCode())->second->getName());
}

std::string Data::getErrorPath(const HttpStatusCode &httpStatusCode) const {
	return (_httpMap.find(httpStatusCode.getStatusCode())->second->getPath());
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
	static std::string buffer;
	static std::string delim(" \t");
	static std::vector<std::string> splitBuffer;
	static int fd;

	fd = open(mimeTypes.c_str(), O_RDONLY);
	if (fd < 0) {
		std::cout << "Critical error - fd negative - parseMimeTypes" << std::endl;
		exit(1);
	}

	while (parseLine(fd, buffer) > 0) {
		splitBuffer = split(buffer, delim);
		if (matchPattern(e_mime, splitBuffer)) {
			while (parseLine(fd, buffer) > 0 && matchPattern(e_end, splitBuffer)) {
				splitBuffer = split(buffer, delim);
				for (size_t i = 1; i < splitBuffer.size(); i++)
					_mimeMap[splitBuffer[i]] = splitBuffer[0];
			}
		}
	}
}

void Data::parseConfiguration(const std::string &configuration) {
	static std::string buffer;
	static std::string delim(" \t");
	static std::vector<std::string> splitBuffer;
	static int fd;

	fd = open(configuration.c_str(), O_RDONLY);
	if (fd < 0) {
		std::cerr << "Critical error - fd negative - parseMimeTypes" << std::endl;
		exit(1);
	}

	while (parseLine(fd, buffer) > 0) {
		splitBuffer = split(buffer, delim);
		if (matchPattern(e_server, splitBuffer))
			_servers.push_back(Server().parseServer(fd));
	}
}



