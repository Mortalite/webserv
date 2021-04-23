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
	static std::string begin[] = {"types", "{"};
	static std::string end[] = {"}"};
	static std::string delim(" \t");
	static std::vector<std::string> beginVec(begin, begin+sizeof(begin)/sizeof(begin[0]));
	static std::vector<std::string> endVec(end, end+sizeof(end)/sizeof(end[0]));
	static std::vector<std::string> splitBuffer;
	static int fd;

	fd = open(mimeTypes.c_str(), O_RDONLY);
	if (fd < 0) {
		std::cout << "Critical error - fd negative - parseMimeTypes" << std::endl;
		exit(1);
	}

	while (parseLine(fd, buffer) > 0) {
		splitBuffer = split(buffer, delim);
		if (splitBuffer == beginVec) {
			while (parseLine(fd, buffer) > 0 && splitBuffer != endVec) {
				splitBuffer = split(buffer, delim);
				for (size_t i = 1; i < splitBuffer.size(); i++)
					_mimeMap[splitBuffer[i]] = splitBuffer[0];
			}
		}
	}

//	for (_mimeMapIt it = _mimeMap.begin(); it != _mimeMap.end(); it++)
//		std::cout << "extension = " << it->first << ", type = " << it->second << std::endl;
}
