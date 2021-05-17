#include "server/Cgi.hpp"

Cgi::Cgi() {
	_envVar = NULL;
}

Cgi::Cgi(const Cgi &other) {
	copy(other._envVar);
}

Cgi::~Cgi() {
	del(_envVar);
}

Cgi &Cgi::operator=(const Cgi &other) {
	if (this != &other) {
		copy(other._envVar);
	}
	return (*this);
}

size_t Cgi::size(char **array) {
	size_t size = 0;

	while (array[size])
		size++;
	return (size);
}

void Cgi::copy(char **array) {
	size_t allocSize = size(array);

	del(_envVar);
	_envVar = new char*[allocSize];
	for (size_t i = 0; i < allocSize; i++)
		_envVar[i] = array[i];
}

void Cgi::del(char **array) {
	if (array) {
		for (size_t i = 0; _envVar[i]; i++)
			delete[] _envVar[i];
		delete[] _envVar;
	}
}

void Cgi::findTarget() {
	struct TargetInfo *tgInfo = _resp->getTargetInfo();
	const std::vector<std::string> *cgi_extension = &(_resp->getClient()->_respLoc->_cgi_extension);

	ft::getFileInfo(tgInfo->_path, *tgInfo);
	if (tgInfo->_type == e_directory && !tgInfo->_path.empty()) {
		tgInfo->_path += tgInfo->_path[tgInfo->_path.size() - 1] != '/' ? "/" : "";
		tgInfo->_path += _resp->getClient()->_respLoc->_cgi_index;
	}
	std::string::size_type pos = tgInfo->_path.find('.');
	if (pos != std::string::npos) {
		std::string extension = tgInfo->_path.substr(pos);
		if (std::find(cgi_extension->begin(), cgi_extension->end(), extension) == cgi_extension->end())
			_resp->initErrorFile(HttpStatusCode("404"));
	}
	else
		_resp->initErrorFile(HttpStatusCode("404"));
}

/*
** Добавил указатель на отвечающий сервер и вывод его данных
*/
void Cgi::makeEnvVar(){
	std::vector<std::string> tmp;

	std::cout << *_server << std::endl;

	_envMap["CONTENT_LENGTH"] = _client->_body.size();
	_envMap["CONTENT_TYPE"] = _client->getHdrOrDflt("content-type", "");
	_envMap["QUERY_STRING"] = _client->getHdrOrDflt("query-string", "");
	_envMap["REQUEST_METHOD"] = _resp->getMethod();
	_envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
	_envMap["SERVER_SOFTWARE"] = "Webserv";
	_envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
	/*
	** SERVER_PORT и SERVER_NAME добавляются в Request::parseHeaders();
	*/
//	_envMap["SERVER_PORT"] = valueToString(server->_listenPort);
//	_envMap["SERVER_NAME"] = serv[0]._serverName[0];
	_envMap["SCRIPT_NAME"] = _tgInfo->_path;
	_envMap["SCRIPT_FILENAME"] = _tgInfo->_path;
	_envMap["REQUEST_URI"] = _client->_respLoc->_uri;

	if (_client->_hdrMap.find("authorization") != _client->_hdrMap.end()) {
		tmp = ft::split(_client->_hdrMap.find("authorization")->second, delimConfig);
		_envMap["AUTH_TYPE"] = tmp[0];
	}
	_envMap["PATH_INFO"] = _envMap["PATH_TRANSLATED"] = _client->_hdrMap.find("request_target")->second;

	/*
	** Надо все заголовки загнать в map в виде (HTTP_+replace(toupper(_hdrMap->first), "-", "_"), _hdrMap->second),
	** то есть привести каждый заголовок в заглавный вид: content-type -> CONTENT-TYPE, поменять дефис на нижнее,
	** подчеркивание CONTENT-TYPE -> CONTENT_TYPE и прибавить в начало HTTP_, в итоге HTTP_CONTENT_TYPE.
	** Потом перегнать в char**.
	*/
	for (	Client::_headersType::const_iterator it = _client->_hdrMap.begin();
			it != _client->_hdrMap.end();
			it++) {
		std::string fieldName = ft::toupper(it->second);
		std::replace(fieldName.begin(), fieldName.end(), '-', '_');
		fieldName = "HTTP_"+fieldName;
	}
}


void Cgi::startCgi(Response *resp) {
	_resp = resp;
	_data = resp->getData();
	_client = resp->getClient();
	_server = _client->_respSvr;
	_tgInfo = _resp->getTargetInfo();

	findTarget();
	makeEnvVar();

	free(_envVar);
	_envMap.clear();
}

