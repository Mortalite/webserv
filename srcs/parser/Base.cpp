#include "parser/Base.hpp"

Base::Base() {
	_root = ".";
	_autoindex = false;
	initBaseFuncMap();
}

Base::Base(std::string root,
		   std::string auth_basic,
		   std::string auth_basic_user_file,
		   std::vector<std::pair<std::string, std::string> > customErrors,
		   std::vector<std::string> allowed_method,
		   std::vector<std::string> index,
		   bool autoindex,
		   std::string cgi_path,
		   std::string cgi_index,
		   std::vector<std::string> cgi_extension) {
	_root = root;
	_auth_basic = auth_basic;
	_auth_basic_user_file = auth_basic_user_file;
	_cgi_path = cgi_path;
	_error_page = customErrors;
	_allowed_method = allowed_method;
	_index = index;
	_cgi_extension = cgi_extension;
	_autoindex = autoindex;
	initBaseFuncMap();
}

Base::Base(const Base &other):	_root(other._root),
								_auth_basic(other._auth_basic),
								_auth_basic_user_file(other._auth_basic_user_file),
								_error_page(other._error_page),
							  	_allowed_method(other._allowed_method),
							  	_index(other._allowed_method),
							  	_autoindex(other._autoindex),
							  	_cgi_path(other._cgi_path),
							  	_cgi_index(other._cgi_index),
							  	_cgi_extension(other._cgi_extension),
								_baseFuncMap(other._baseFuncMap) {}

Base::~Base() {}

Base &Base::operator=(const Base &other) {
	if (this != &other) {
		_root = other._root;
		_auth_basic = other._auth_basic;
		_auth_basic_user_file = other._auth_basic_user_file;
		_error_page = other._error_page;
		_allowed_method = other._allowed_method;
		_index = other._index;
		_autoindex = other._autoindex;
		_cgi_path = other._cgi_path;
		_cgi_index = other._cgi_index;
		_cgi_extension = other._cgi_extension;
		_baseFuncMap = other._baseFuncMap;
	}
	return (*this);
}

std::ostream &Base::print(std::ostream &out) const {
	out << "_root = " << _root << std::endl;
	out << "_auth_basic = " << _auth_basic << std::endl;
	out << "_auth_basic_user_file = " << _auth_basic_user_file << std::endl;
	ft::printContainer(out, "_allowed_method", _allowed_method);
	ft::printContainer(out, "_index", _index);
	out << "_autoindex = " << _autoindex << std::endl;
	out << "_cgi_path = " << _cgi_path << std::endl;
	out << "_cgi_index = " << _cgi_index << std::endl;
	ft::printContainer(out, "_cgi_extension", _cgi_extension);
	return (out);
}

void Base::initBaseFuncMap() {
	_baseFuncMap.insert(std::make_pair("root", &Base::parseRoot));
	_baseFuncMap.insert(std::make_pair("auth_basic", &Base::parseAuthBasic));
	_baseFuncMap.insert(std::make_pair("auth_basic_user_file", &Base::parseAuthBasicUserFile));
	_baseFuncMap.insert(std::make_pair("error_page", &Base::parseCustomErrors));
	_baseFuncMap.insert(std::make_pair("allowed_method", &Base::parseAllowedMethod));
	_baseFuncMap.insert(std::make_pair("index", &Base::parseIndex));
	_baseFuncMap.insert(std::make_pair("autoindex", &Base::parseAutoindex));
	_baseFuncMap.insert(std::make_pair("cgi_path", &Base::parseCgiPath));
	_baseFuncMap.insert(std::make_pair("cgi_index", &Base::parseCgiIndex));
	_baseFuncMap.insert(std::make_pair("cgi_extension", &Base::parseCgiExtension));
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

void Base::parseCustomErrors(std::vector<std::string> &splitBuffer) {
	_error_page.push_back(std::make_pair(splitBuffer[1], splitBuffer[2]));
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

void Base::parseCgiPath(std::vector<std::string> &splitBuffer) {
	_cgi_path = splitBuffer[1];
}

void Base::parseCgiIndex(std::vector<std::string> &splitBuffer) {
	_cgi_index = splitBuffer[1];
}

void Base::parseCgiExtension(std::vector<std::string> &splitBuffer) {
	for (size_t i = 1; i < splitBuffer.size(); i++)
		_cgi_extension.push_back(splitBuffer[i]);
}
