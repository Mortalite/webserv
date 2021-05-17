#include "parser/Server.hpp"

Server::Server():Base() {
	_clientMaxBodySize = bodyBufferSize;
	_listenPort = 8080;

	_svrFuncMap.insert(std::make_pair("host", &Server::parseHost));
	_svrFuncMap.insert(std::make_pair("client_max_body_size", &Server::parseClientMaxBodySize));
	_svrFuncMap.insert(std::make_pair("listen", &Server::parseListenPorts));
	_svrFuncMap.insert(std::make_pair("server_name", &Server::parseServerNames));

	_svrFuncMap.insert(std::make_pair("root", &Server::parseRoot));
	_svrFuncMap.insert(std::make_pair("auth_basic", &Server::parseAuthBasic));
	_svrFuncMap.insert(std::make_pair("auth_basic_user_file", &Server::parseAuthBasicUserFile));
	_svrFuncMap.insert(std::make_pair("error_page", &Server::parseCustomErrors));
	_svrFuncMap.insert(std::make_pair("allowed_method", &Server::parseAllowedMethod));
	_svrFuncMap.insert(std::make_pair("index", &Server::parseIndex));
	_svrFuncMap.insert(std::make_pair("autoindex", &Server::parseAutoindex));
	_svrFuncMap.insert(std::make_pair("cgi_path", &Server::parseCgiPath));
	_svrFuncMap.insert(std::make_pair("cgi_index", &Server::parseCgiIndex));
	_svrFuncMap.insert(std::make_pair("cgi_extension", &Server::parseCgiExtension));
}

Server::~Server() {}

Server::Server(const Server &other): Base(	other._root,
										 	other._auth_basic,
										 	other._auth_basic_user_file,
										 	other._error_page,
										  	other._allowed_method,
										 	other._index,
										 	other._autoindex,
										 	other._cgi_path,
											other._cgi_index,
										 	other._cgi_extension),
									 _splitBuffer(other._splitBuffer),
									 _buffer(other._buffer),
									 _host(other._host),
									 _clientMaxBodySize(other._clientMaxBodySize),
									 _listenPort(other._listenPort),
									 _serverName(other._serverName),
									 _locations(other._locations),
									 _svrFuncMap(other._svrFuncMap) {}

Server &Server::operator=(const Server &other) {
	if (this != &other) {
		Base::operator=(other);
		_splitBuffer = other._splitBuffer;
		_buffer = other._buffer;
		_host = other._host;
		_clientMaxBodySize = other._clientMaxBodySize;
		_listenPort = other._listenPort;
		_serverName = other._serverName;
		_locations = other._locations;
		_svrFuncMap = other._svrFuncMap;
	}
	return (*this);
}

std::ostream &Server::print(std::ostream &out) const {
	out << RED << "Server" << RESET << std::endl;
	out << "_host = " << _host << std::endl;
	out << "_clientMaxBodySize = " << _clientMaxBodySize << std::endl;
	std::cout << "_listenPort = " << _listenPort << std::endl;
	Base::print(out);
	ft::printContainer(out, "_serverName", _serverName);

	size_t counter = 0;
	for (	Location::_locsType::const_iterator it = _locations.begin();
			 it != _locations.end(); it++)
		out << "Location" << "[" << counter++ << "]\n" << *it << std::endl;
	return (out);
}

void Server::parseHost(std::vector<std::string> &splitBuffer) {
	_host = splitBuffer[1];
}

void Server::parseClientMaxBodySize(std::vector<std::string> &splitBuffer) {
	_clientMaxBodySize = ft::strToLong(ft::trim(splitBuffer[1], "m"))*1000*1000;
}

void Server::parseListenPorts(std::vector<std::string> &splitBuffer) {
	_listenPort = ft::strToLong(splitBuffer[1]);
}

void Server::parseServerNames(std::vector<std::string> &splitBuffer) {
	for (size_t i = 1; i < splitBuffer.size(); ++i)
		_serverName.push_back(splitBuffer[i]);
}

Server& Server::parseServer(int fd) {
	while (ft::parseLine(fd, _buffer) > 0) {
		_splitBuffer = ft::split(_buffer, delimConfig);
		if (ft::matchPattern(e_end, _splitBuffer))
			break;
		if (ft::matchPattern(e_location, _splitBuffer))
			_locations.push_back(Location().parseLocation(fd, _splitBuffer));
		if (!_splitBuffer.empty() && this->_svrFuncMap.find(_splitBuffer[0]) != _svrFuncMap.end())
			(this->*_svrFuncMap.find(_splitBuffer[0])->second)(_splitBuffer);
	}
	return (*this);
}

void Server::setServerConfig() {
	if (_host.empty())
		_host = "localhost";
	for (	Location::_locsIt locationsIt = _locations.begin();
			locationsIt != _locations.end(); locationsIt++) {
		if ((*locationsIt)._root.empty())
			(*locationsIt)._root = _root;
		if ((*locationsIt)._auth_basic.empty())
			(*locationsIt)._auth_basic = _auth_basic;
		if ((*locationsIt)._auth_basic_user_file.empty())
			(*locationsIt)._auth_basic_user_file = _auth_basic_user_file;
		if ((*locationsIt)._error_page.empty())
			(*locationsIt)._error_page = _error_page;
		if ((*locationsIt)._allowed_method.empty())
			(*locationsIt)._allowed_method = _allowed_method;
		if ((*locationsIt)._index.empty())
			(*locationsIt)._index = _index;
		if (!(*locationsIt)._autoindex)
			(*locationsIt)._autoindex = _autoindex;
		if ((*locationsIt)._cgi_path.empty())
			(*locationsIt)._cgi_path = _cgi_path;
		if ((*locationsIt)._cgi_index.empty())
			(*locationsIt)._cgi_index = _cgi_index;
		if ((*locationsIt)._cgi_extension.empty())
			(*locationsIt)._cgi_extension = _cgi_extension;

	}
}
