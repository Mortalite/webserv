#include "server/Cgi.hpp"

Cgi::Cgi() {
	_client = NULL;
	_tgInfo = NULL;
	_envVar = NULL;
}

Cgi::Cgi(const Cgi &other) {
	_client = other._client;
	_tgInfo = other._tgInfo;
	_envVar = copy(other._envVar);
}

Cgi::~Cgi() {
	del(_envVar);
}

Cgi &Cgi::operator=(const Cgi &other) {
	if (this != &other) {
		del(_envVar);
		_envVar = copy(other._envVar);
	}
	return (*this);
}

size_t Cgi::size(char **array) {
	size_t size = 0;

	while (array[size])
		size++;
	return (size);
}

char** Cgi::copy(char **array) {
	size_t allocSize = size(array);

	char **result = new char*[allocSize + 1];
	for (size_t i = 0; i < allocSize; i++)
		result[i] = array[i];
	result[allocSize] = NULL;
	return (result);
}

void Cgi::del(char **&array) {
	if (array) {
		for (size_t i = 0; array[i]; i++)
			delete[] array[i];
		delete[] array;
		array = NULL;
	}
}

void Cgi::convertEnvVar() {
	size_t size = _envMap.size() + 1;

	_envVar = new char*[size];
	_envVar[size - 1] = NULL;
	size_t i = 0;
	for (	envMapType::const_iterator it = _envMap.begin();
			 it != _envMap.end();
			 it++, i++) {
		if ((_envVar[i] = strdup((it->first + "=" + it->second).c_str())) == NULL)
			TraceException("strdup failed");
	}
}


void Cgi::findTarget() {
	const std::vector<std::string> *cgi_extension = &(_client->_respLoc->_cgi_extension);

	ft::getFileInfo(_tgInfo->_path, *_tgInfo);
	if (_tgInfo->_type == e_directory && !_tgInfo->_path.empty()) {
		_tgInfo->_path += _tgInfo->_path[_tgInfo->_path.size() - 1] != '/' ? "/" : "";
		_tgInfo->_path += _client->_respLoc->_cgi_index;
	}
	std::string::size_type pos = _tgInfo->_path.find('.');
	if (pos != std::string::npos) {
		std::string extension = _tgInfo->_path.substr(pos+1);
		if (std::find(cgi_extension->begin(), cgi_extension->end(), extension) == cgi_extension->end())
			throw HttpStatusCode("404");
	}
	else
		throw HttpStatusCode("404");
}

/*
** Добавил указатель на отвечающий сервер и вывод его данных
*/
void Cgi::makeEnvVar() {
	std::vector<std::string> splitBuffer;

	if (_client->_hdrMap.find("authorization") != _client->_hdrMap.end()) {
		splitBuffer = ft::split(_client->_hdrMap.find("authorization")->second, delimConfig);
		std::string user = Base64::decode(splitBuffer[1]);

		_envMap["AUTH_TYPE"] = splitBuffer[0];
		_envMap["REMOTE_USER"] = user.substr(0, user.find(':'));
	}
	else {
		_envMap["AUTH_TYPE"] = "";
		_envMap["REMOTE_USER"] = "";
	}
	_envMap["CONTENT_LENGTH"] = ft::valueToString(_client->_body.size());
	_envMap["CONTENT_TYPE"] = _client->getHdrOrDflt("content-type", "");
	_envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
	_envMap["REQUEST_FILENAME"] = _client->_hdrMap["request_target"];
	_envMap["PATH_INFO"] = _client->_hdrMap["request_target"];
//	_envMap["PATH_INFO"] = _client->_respLoc->_uri;
//	_envMap["PATH_TRANSLATED"] = ft::getcwd()+_client->_respLoc->_uri;
	_envMap["PATH_TRANSLATED"] = ft::getcwd()+_client->_hdrMap["request_target"];
	_envMap["QUERY_STRING"] = _client->getHdrOrDflt("query-string", "");
//	_envMap["REMOTE_ADDR"] = inet_ntoa(_client->_acptAddr.sin_addr);
	_envMap["REMOTE_ADDR"] = "localhost";

	_envMap["REMOTE_IDENT"] = "";

	_envMap["REQUEST_METHOD"] = _client->_hdrMap["method"];
//	_envMap["REQUEST_URI"] = _client->_respLoc->_uri;
	_envMap["REQUEST_URI"] = _client->_hdrMap["request_target"];

	_envMap["SCRIPT_NAME"] = _envMap["SCRIPT_FILENAME"] = _tgInfo->_path;

	if (_client->_hdrMap.find("host") != _client->_hdrMap.end()) {
		splitBuffer = ft::split(_client->_hdrMap.find("host")->second, ":");
		_envMap["SERVER_NAME"] = ft::trim(splitBuffer[0], delimConfig);
		_envMap["SERVER_PORT"] = ft::trim(splitBuffer[1], delimConfig);
	}
	_envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
	_envMap["SERVER_SOFTWARE"] = "HTTP 1.1";

	std::string fieldName;
	for (	Client::_headersMapType::const_iterator it = _client->_hdrMap.begin();
			 it != _client->_hdrMap.end();
			 it++) {
		fieldName = ft::toupper(it->first);
		std::replace(fieldName.begin(), fieldName.end(), '-', '_');
		_envMap["HTTP_"+fieldName] = it->second;
	}
	_envMap.erase(_envMap.find("HTTP_HTTP_VERSION"));
	_envMap.erase(_envMap.find("HTTP_METHOD"));
	_envMap.erase(_envMap.find("HTTP_REQUEST_TARGET"));
	convertEnvVar();
}

void Cgi::cgiExecve() {
	char filename[2][50];
	memset(filename[0],0,sizeof(char)*50);
	memset(filename[1],0,sizeof(char)*50);

	std::string templateFileName("/tmp/file-XXXXXX");
	strncpy(filename[0], templateFileName.c_str(), templateFileName.size());
	strncpy(filename[1], templateFileName.c_str(), templateFileName.size());

	int inputFD,
		outputFD;
	pid_t pid;

	if ((inputFD = mkstemp(filename[0])) == -1)
		TraceException("mkostemp failed");
	if (write(inputFD, _client->_body.c_str(), _client->_body.size()) == -1)
		TraceException("write failed");
	if ((outputFD = mkstemp(filename[1])) == -1)
		TraceException("mkostemp failed");
	if (close(inputFD) == -1)
		TraceException("close failed");
	if (close(outputFD) == -1)
		TraceException("close failed");

	char 	*cgi_path = NULL,
			*path = NULL;
	cgi_path = strdup(_client->_respLoc->_cgi_path.c_str());
	path = strdup(_tgInfo->_path.c_str());
	char *const args[3] = {	cgi_path,
							   path,
							   NULL};

	pid = fork();
	if (pid == -1)
		TraceException("fork failed");
	else if (pid == 0) {
		if ((inputFD = open(filename[0], O_RDONLY, S_IWUSR)) == -1)
			TraceException("mkostemp failed");
		if (dup2(inputFD, 0) == -1)
			TraceException("dup2 failed");
		if (close(inputFD) == -1)
			TraceException("close failed");

		if ((outputFD = open(filename[1], O_WRONLY, S_IWUSR)) == -1)
			TraceException("mkostemp failed");
		if (dup2(outputFD, 1) == -1)
			TraceException("dup2 failed");
		if (close(outputFD) == -1)
			TraceException("close failed");

		execve(args[0], args, _envVar);
		exit(EXIT_FAILURE);
	}
	else {
		int status;
		if (waitpid(-1, &status, 0) == -1)
			TraceException("waitpid failed");
		if (WIFEXITED(status))
			status = WEXITSTATUS(status);
		_cgiResp = ft::readFile(filename[1]);
		if (unlink(filename[0]) == -1)
			TraceException("unlink failed");
		if (unlink(filename[1]) == -1)
			TraceException("unlink failed");
	}
}

void Cgi::parseCgiResp() {
	std::string delim("\r\n\r\n");
	std::string::size_type	pos(_cgiResp.find(delim));

	if (pos != std::string::npos) {
		std::string fieldName;
		std::string fieldValue;
		std::string::size_type ptr;
		std::vector<std::string> hdr(ft::split(_cgiResp.substr(0, pos), delimHeaders));

		for (size_t i = 0; i < hdr.size(); i++) {
			if ((ptr = hdr[i].find(":")) != std::string::npos) {
				fieldName = ft::tolower(hdr[i].substr(0, ptr));
				fieldValue = ft::trim(hdr[i].substr(ptr + 1), delimConfig);
				if (fieldName == "status")
					_client->_httpStatusCode = ft::trim(ft::split(fieldValue, delimConfig)[0], delimConfig);
				else
					_client->_hdrMap[fieldName] = fieldValue;
			}
		}
		_tgInfo->_body = _cgiResp.substr(pos + 4);
	}
	else
		_tgInfo->_body = _cgiResp;
	_tgInfo->_size = ft::valueToString(_tgInfo->_body.size());
}

void Cgi::startCgi(Client *client, struct TargetInfo *tgInfo) {
	_client = client;
	_tgInfo = tgInfo;

	static int count = 0;
	std::cout << "count = " << count++ << std::endl;

	findTarget();
	makeEnvVar();
	cgiExecve();
	parseCgiResp();
}
