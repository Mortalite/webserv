#include "parser/Location.hpp"

Location::Location() {
	_funcMap.insert(std::make_pair("location", &Location::parseURI));
	_funcMap.insert(std::make_pair("root", &Location::parseRoot));
	_funcMap.insert(std::make_pair("index", &Location::parseIndex));
	_funcMap.insert(std::make_pair("autoindex", &Location::parseAutoindex));
}

Location::Location(const Location &other):	_URI(other._URI),
											_root(other._root),
											_index(other._index),
											_autoindex(other._autoindex),
											_funcMap(other._funcMap) {}

Location::~Location() {}

const std::string &Location::getUri() const {
	return _URI;
}

void Location::setUri(const std::string &uri) {
	_URI = uri;
}

const std::string &Location::getRoot() const {
	return _root;
}

void Location::setRoot(const std::string &root) {
	_root = root;
}

const std::vector<std::string> &Location::getIndex() const {
	return _index;
}

void Location::setIndex(const std::vector<std::string> &index) {
	_index = index;
}

bool Location::isAutoindex() const {
	return _autoindex;
}

void Location::setAutoindex(bool autoindex) {
	_autoindex = autoindex;
}

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

Location& Location::parseLocation(int fd, std::vector<std::string>& splitBuffer) {
	static std::string buffer;
	static std::string delim(" \t");

	(this->*_funcMap.find(splitBuffer[0])->second)(splitBuffer);
	while (parseLine(fd, buffer) > 0) {
		splitBuffer = split(buffer, delim);
		if (parser::matchPattern(parser::e_end, splitBuffer))
			break ;
		(this->*_funcMap.find(splitBuffer[0])->second)(splitBuffer);
	}
	return (*this);
}

Location &Location::operator=(const Location &other) {
	_URI = other._URI;
	_root = other._root;
	_index = other._index;
	_autoindex = other._autoindex;
	_funcMap = other._funcMap;
	return (*this);
}
