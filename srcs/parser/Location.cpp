#include "parser/Location.hpp"

Location::Location() {
	_locationFuncMap.insert(std::make_pair("location", &Location::parseURI));
	_locationFuncMap.insert(std::make_pair("root", &Location::parseRoot));
	_locationFuncMap.insert(std::make_pair("index", &Location::parseIndex));
	_locationFuncMap.insert(std::make_pair("autoindex", &Location::parseAutoindex));
}

Location::Location(const Location &other): _URI(other._URI),
										   _root(other._root),
										   _index(other._index),
										   _autoindex(other._autoindex),
										   _locationFuncMap(other._locationFuncMap) {}

Location::~Location() {}

void Location::parseURI(std::vector<std::string> &splitBuffer) {
	_URI = splitBuffer[1];
}

void Location::parseRoot(std::vector<std::string> &splitBuffer) {
	_root = splitBuffer[1];
}

void Location::parseIndex(std::vector<std::string> &splitBuffer) {
	for (size_t i = 1; i < splitBuffer.size(); i++)
		_index.push_back(splitBuffer[i]);
}

void Location::parseAutoindex(std::vector<std::string> &splitBuffer) {
	_autoindex = splitBuffer[1] == "on";
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

Location &Location::operator=(const Location &other) {
	_URI = other._URI;
	_root = other._root;
	_index = other._index;
	_autoindex = other._autoindex;
	_locationFuncMap = other._locationFuncMap;
	return (*this);
}
