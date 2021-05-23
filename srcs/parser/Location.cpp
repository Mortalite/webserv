#include "parser/Location.hpp"

Location::Location():Base() {
	_locFuncMap.insert(std::make_pair("location", &Location::parseURI));

	_locFuncMap.insert(std::make_pair("root", &Location::parseRoot));
	_locFuncMap.insert(std::make_pair("auth_basic", &Location::parseAuthBasic));
	_locFuncMap.insert(std::make_pair("auth_basic_user_file", &Location::parseAuthBasicUserFile));
	_locFuncMap.insert(std::make_pair("cgi_index", &Location::parseCgiIndex));
	_locFuncMap.insert(std::make_pair("cgi_path", &Location::parseCgiPath));
	_locFuncMap.insert(std::make_pair("error_page", &Location::parseErrorPage));
	_locFuncMap.insert(std::make_pair("allowed_method", &Location::parseAllowedMethod));
	_locFuncMap.insert(std::make_pair("index", &Location::parseIndex));
	_locFuncMap.insert(std::make_pair("cgi_extension", &Location::parseCgiExtension));
	_locFuncMap.insert(std::make_pair("client_max_body_size", &Location::parseClientMaxBodySize));
	_locFuncMap.insert(std::make_pair("autoindex", &Location::parseAutoindex));
}

Location::Location(const Location &other): Base(	other._root,
													other._auth_basic,
													other._auth_basic_user_file,
													other._cgi_index,
													other._cgi_path,
													other._error_page,
													other._allowed_method,
													other._index,
													other._cgi_extension,
													other._client_max_body_size,
													other._autoindex),
										   _uri(other._uri),
										   _locFuncMap(other._locFuncMap) {}

Location::~Location() {}

Location &Location::operator=(const Location &other) {
	if (this != &other) {
		Base::operator=(other);
		_uri = other._uri;
		_locFuncMap = other._locFuncMap;
	}
	return (*this);
}

std::ostream &Location::print(std::ostream &out) const {
	out << "URI = " << _uri << std::endl;
	Base::print(out);
	return (out);
}

void Location::parseURI(std::vector<std::string> &splitBuffer) {
	_uri = splitBuffer[1];
}

Location& Location::parseLocation(int fd, std::vector<std::string> &splitBuffer) {
	static std::string buffer;

	(this->*_locFuncMap.find(splitBuffer[0])->second)(splitBuffer);
	while (ft::parseLine(fd, buffer) > 0) {
		splitBuffer = ft::split(buffer, ft::delimConfig);
		if (ft::matchPattern(pattern::e_end, splitBuffer))
			break;
		if (!splitBuffer.empty() && this->_locFuncMap.find(splitBuffer[0]) != _locFuncMap.end())
			(this->*_locFuncMap.find(splitBuffer[0])->second)(splitBuffer);
	}
	return (*this);
}
