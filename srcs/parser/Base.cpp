#include "parser/Base.hpp"

Base::Base() {
	_baseFuncMap.insert(std::make_pair("root", &Base::parseRoot));
	_baseFuncMap.insert(std::make_pair("allowed_method", &Base::parseAllowedMethod));
	_baseFuncMap.insert(std::make_pair("index", &Base::parseIndex));
	_baseFuncMap.insert(std::make_pair("autoindex", &Base::parseAutoindex));
}

Base::Base(std::string root,
		   std::vector<std::string> allowed_method,
		   std::vector<std::string> index,
		   bool autoindex) {
	_root = root;
	_allowed_method = allowed_method;
	_index = index;
	_autoindex = autoindex;
	_baseFuncMap.insert(std::make_pair("root", &Base::parseRoot));
	_baseFuncMap.insert(std::make_pair("allowed_method", &Base::parseAllowedMethod));
	_baseFuncMap.insert(std::make_pair("index", &Base::parseIndex));
	_baseFuncMap.insert(std::make_pair("autoindex", &Base::parseAutoindex));
}

Base::Base(const Base &other):	_root(other._root),
								_allowed_method(other._allowed_method),
								_index(other._allowed_method),
								_autoindex(other._autoindex) {}

Base::~Base() {}

Base &Base::operator=(const Base &other) {
	if (this != &other) {
		_root = other._root;
		_allowed_method = other._allowed_method;
		_index = other._index;
		_autoindex = other._autoindex;
	}
	return (*this);
}

void Base::parseRoot(std::vector<std::string> &splitBuffer) {
	_root = splitBuffer[1];

}

void Base::parseAllowedMethod(std::vector<std::string> &splitBuffer) {
	for (size_t i = 1; i < splitBuffer.size(); i++)
		_allowed_method.push_back(splitBuffer[i]);
}

void Base::parseIndex(std::vector<std::string> &splitBuffer) {
	for (size_t i = 1; i < splitBuffer.size(); i++)
		_index.push_back(splitBuffer[i]);
}

void Base::parseAutoindex(std::vector<std::string> &splitBuffer) {
	_autoindex = splitBuffer[1] == "on";
}
