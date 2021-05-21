#include "parser/Base.hpp"

Base::Base() {
	_root = ".";
	_client_max_body_size = defaultClientMaxBodySize;
	_autoindex = false;
	initBaseFuncMap();
}

Base::Base(const std::string &root,
		   const std::string &auth_basic,
		   const std::string &auth_basic_user_file,
		   const std::string &cgi_index,
		   const std::string &cgi_path,
		   const std::vector<std::pair<std::string, std::string> > &error_page,
		   const std::vector<std::string> &allowed_method,
		   const std::vector<std::string> &index,
		   const std::vector<std::string> &cgi_extension,
		   long clientMaxBodySize,
		   bool autoindex) {
	_root = root;
	_auth_basic = auth_basic;
	_auth_basic_user_file = auth_basic_user_file;
	_cgi_index = cgi_index;
	_cgi_path = cgi_path;
	_error_page = error_page;
	_allowed_method = allowed_method;
	_index = index;
	_cgi_extension = cgi_extension;
	_client_max_body_size = clientMaxBodySize;
	_autoindex = autoindex;
	initBaseFuncMap();
}

Base::Base(const Base &other): _root(other._root),
							   _auth_basic(other._auth_basic),
							   _auth_basic_user_file(other._auth_basic_user_file),
							   _cgi_index(other._cgi_index),
							   _cgi_path(other._cgi_path),
							   _error_page(other._error_page),
							   _allowed_method(other._allowed_method),
							   _index(other._allowed_method),
							   _cgi_extension(other._cgi_extension),
							   _client_max_body_size(other._client_max_body_size),
							   _autoindex(other._autoindex),
							   _baseFuncMap(other._baseFuncMap) {}

Base::~Base() {}

Base &Base::operator=(const Base &other) {
	if (this != &other) {
		_root = other._root;
		_auth_basic = other._auth_basic;
		_auth_basic_user_file = other._auth_basic_user_file;
		_cgi_index = other._cgi_index;
		_cgi_path = other._cgi_path;
		_error_page = other._error_page;
		_allowed_method = other._allowed_method;
		_index = other._index;
		_cgi_extension = other._cgi_extension;
		_client_max_body_size = other._client_max_body_size;
		_autoindex = other._autoindex;
		_baseFuncMap = other._baseFuncMap;
	}
	return (*this);
}

std::ostream &Base::print(std::ostream &out) const {
	out << "_root = " << _root << std::endl;
	out << "_auth_basic = " << _auth_basic << std::endl;
	out << "_auth_basic_user_file = " << _auth_basic_user_file << std::endl;
	out << "_cgi_index = " << _cgi_index << std::endl;
	out << "_cgi_path = " << _cgi_path << std::endl;
	ft::printContainer(out, "_allowed_method", _allowed_method);
	ft::printContainer(out, "_index", _index);
	out << "_clientMaxBodySize = " << _client_max_body_size << std::endl;
	out << "_autoindex = " << _autoindex << std::endl;
	ft::printContainer(out, "_cgi_extension", _cgi_extension);
	return (out);
}

void Base::initBaseFuncMap() {
	_baseFuncMap.insert(std::make_pair("root", &Base::parseRoot));
	_baseFuncMap.insert(std::make_pair("auth_basic", &Base::parseAuthBasic));
	_baseFuncMap.insert(std::make_pair("auth_basic_user_file", &Base::parseAuthBasicUserFile));
	_baseFuncMap.insert(std::make_pair("cgi_index", &Base::parseCgiIndex));
	_baseFuncMap.insert(std::make_pair("cgi_path", &Base::parseCgiPath));
	_baseFuncMap.insert(std::make_pair("error_page", &Base::parseErrorPage));
	_baseFuncMap.insert(std::make_pair("allowed_method", &Base::parseAllowedMethod));
	_baseFuncMap.insert(std::make_pair("index", &Base::parseIndex));
	_baseFuncMap.insert(std::make_pair("cgi_extension", &Base::parseCgiExtension));
	_baseFuncMap.insert(std::make_pair("client_max_body_size", &Base::parseClientMaxBodySize));
	_baseFuncMap.insert(std::make_pair("autoindex", &Base::parseAutoindex));
}

void Base::parseRoot(std::vector<std::string> &splitBuffer) {
	_root = splitBuffer[1];
}

void Base::parseAuthBasic(std::vector<std::string> &splitBuffer) {
	_auth_basic = ft::trim(splitBuffer[1], "\"");
}

void Base::parseAuthBasicUserFile(std::vector<std::string> &splitBuffer) {
	_auth_basic_user_file = splitBuffer[1];
}

void Base::parseCgiIndex(std::vector<std::string> &splitBuffer) {
	_cgi_index = splitBuffer[1];
}

void Base::parseCgiPath(std::vector<std::string> &splitBuffer) {
	_cgi_path = splitBuffer[1];
}

void Base::parseErrorPage(std::vector<std::string> &splitBuffer) {
	_error_page.push_back(std::make_pair(splitBuffer[1], splitBuffer[2]));
}

void Base::parseAllowedMethod(std::vector<std::string> &splitBuffer) {
	for (size_t i = 1; i < splitBuffer.size(); i++)
		_allowed_method.push_back(splitBuffer[i]);
}

void Base::parseCgiExtension(std::vector<std::string> &splitBuffer) {
	for (size_t i = 1; i < splitBuffer.size(); i++)
		_cgi_extension.push_back(splitBuffer[i]);
}

void Base::parseIndex(std::vector<std::string> &splitBuffer) {
	for (size_t i = 1; i < splitBuffer.size(); i++)
		_index.push_back(splitBuffer[i]);
}

void Base::parseClientMaxBodySize(std::vector<std::string> &splitBuffer) {
	long multiplier;

	if (ft::isEndWith(splitBuffer[1], "b"))
		multiplier = 1;
	else
		multiplier = 1000*1000;
	_client_max_body_size = ft::strToLong(ft::trim(splitBuffer[1], "bm"), 10) * multiplier;
}


void Base::parseAutoindex(std::vector<std::string> &splitBuffer) {
	_autoindex = splitBuffer[1] == "on";
}
