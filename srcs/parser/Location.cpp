#include "parser/Location.hpp"

Location::Location():Base() {
	_locationFuncMap.insert(std::make_pair("location", &Location::parseURI));
	_locationFuncMap.insert(std::make_pair("root", &Location::parseRoot));
	_locationFuncMap.insert(std::make_pair("allowed_method", &Location::parseAllowedMethod));
	_locationFuncMap.insert(std::make_pair("index", &Location::parseIndex));
	_locationFuncMap.insert(std::make_pair("autoindex", &Location::parseAutoindex));
}

Location::Location(const Location &other):	Base(	other._root,
													other._allowed_method,
													other._index,
													other._autoindex),
											_uri(other._uri),
										   	_locationFuncMap(other._locationFuncMap) {}

Location::~Location() {}

Location &Location::operator=(const Location &other) {
	Base::operator=(other);
	_uri = other._uri;
	_locationFuncMap = other._locationFuncMap;
	return (*this);
}

void Location::parseURI(std::vector<std::string> &splitBuffer) {
	_uri = splitBuffer[1];
}

Location& Location::parseLocation(int fd, std::vector<std::string> &splitBuffer) {
	static std::string buffer;
	static std::string delim(" \t");

    while (!matchPattern(e_end, splitBuffer)) {
		if (this->_locationFuncMap.find(splitBuffer[0]) != _locationFuncMap.end())
			(this->*_locationFuncMap.find(splitBuffer[0])->second)(splitBuffer);
		if (parseLine(fd, buffer) <= 0)
			break;
		splitBuffer = split(buffer, delim);
	}
	return (*this);
}

