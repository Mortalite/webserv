#include "server/Cgi.hpp"

Cgi::Cgi() {
	_client = NULL;
	_tgInfo = NULL;
	_cgi_path = NULL;
	_path = NULL;
	_envVar = NULL;
}

Cgi::Cgi(const Cgi &other) {
	_client = other._client;
	_tgInfo = other._tgInfo;
	_cgi_path = other._cgi_path;
	_path = other._path;
	_envVar = copy(other._envVar);
}

Cgi::~Cgi() {
	clear();
}

Cgi &Cgi::operator=(const Cgi &other) {
	if (this != &other) {
		_client = other._client;
		_tgInfo = other._tgInfo;
		clear();
		_cgi_path = copy(other._cgi_path);
		_path = copy(other._path);
		_envVar = copy(other._envVar);
	}
	return (*this);
}

void Cgi::clear() {
	freeArray(_cgi_path);
	freeArray(_path);
	freeArray(_envVar);
}

size_t Cgi::size(char **array) {
	size_t size = 0;

	while (array[size])
		size++;
	return (size);
}

char *Cgi::copy(char *array) {
	return (strdup(array));
}

char** Cgi::copy(char **array) {
	size_t allocSize = size(array);

	char **result = (char**)malloc(sizeof(*result)*allocSize);
	for (size_t i = 0; i < allocSize; i++)
		result[i] = array[i];
	result[allocSize] = NULL;
	return (result);
}

void Cgi::freeArray(char *&array) {
	free(array);
	array = NULL;
}

void Cgi::freeArray(char **&array) {
	if (array) {
		for (size_t i = 0; array[i]; i++)
			free(array[i]);
		free(array);
		array = NULL;
	}
}

void Cgi::convertEnvVar() {
	size_t size = _envMap.size() + 1;
	_envVar = (char**)malloc(sizeof(*_envVar)*size);

	for (size_t i = 0; i < size; i++)
		_envVar[i] = NULL;

	size_t i = 0;
	for (	envMapType::const_iterator it = _envMap.begin();
			 it != _envMap.end();
			 it++, i++) {
		if (!(_envVar[i] = strdup((it->first + "=" + it->second).c_str())))
			TraceException("strdup failed");
	}
}

void Cgi::findTarget() {
	const std::vector<std::string> *cgi_extension = &(_client->_respLoc->_cgi_extension);

	ft::getFileInfo(_tgInfo->_path, *_tgInfo);
	if (_tgInfo->_type == filetype::e_directory && !_tgInfo->_path.empty()) {
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

void Cgi::makeEnvVar() {
	std::vector<std::string> splitBuffer;

	if (_client->_hdrMap.find("authorization") != _client->_hdrMap.end()) {
		splitBuffer = ft::split(_client->_hdrMap.find("authorization")->second, ft::delimConfig);
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
	_envMap["PATH_TRANSLATED"] = ft::getcwd()+_client->_hdrMap["request_target"];
	_envMap["QUERY_STRING"] = _client->getHdrOrDflt("query-string", "");
	_envMap["REMOTE_ADDR"] = "localhost";
	_envMap["REMOTE_IDENT"] = "";
	_envMap["REQUEST_METHOD"] = _client->_hdrMap["method"];
	_envMap["REQUEST_URI"] = _client->_hdrMap["request_target"];
	_envMap["SCRIPT_NAME"] = _envMap["SCRIPT_FILENAME"] = _tgInfo->_path;

	if (_client->_hdrMap.find("host") != _client->_hdrMap.end()) {
		splitBuffer = ft::split(_client->_hdrMap.find("host")->second, ":");
		_envMap["SERVER_NAME"] = ft::trim(splitBuffer[0], ft::delimConfig);
		_envMap["SERVER_PORT"] = ft::trim(splitBuffer[1], ft::delimConfig);
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
	convertEnvVar();
}

void Cgi::cgiExecve() {
	char filename[2][50];
	memset(filename,0,sizeof(char)*100);

	std::string templateFileName("/tmp/file-XXXXXX");
	strncpy(filename[0], templateFileName.c_str(), templateFileName.size());
	strncpy(filename[1], templateFileName.c_str(), templateFileName.size());

	int inputFD,
		outputFD;
	pid_t pid;

	if ((inputFD = mkstemp(filename[0])) == -1)
		TraceException("mkstemp failed");
	if (write(inputFD, _client->_body.c_str(), _client->_body.size()) == -1)
		TraceException("write failed");
	if ((outputFD = mkstemp(filename[1])) == -1)
		TraceException("mkstemp failed");
	if (close(inputFD) == -1)
		TraceException("close failed");
	if (close(outputFD) == -1)
		TraceException("close failed");

	if (!(_cgi_path = strdup(_client->_respLoc->_cgi_path.c_str())))
		TraceException("strdup failed");
	if (!(_path = strdup(_tgInfo->_path.c_str())))
		TraceException("strdup failed");
	char *const args[3] = {	_cgi_path,
						 	_path,
						 	NULL};

	pid = fork();
	if (pid == -1)
		TraceException("fork failed");
	else if (pid == 0) {
		if ((inputFD = open(filename[0], O_RDONLY, S_IRUSR)) == -1)
			TraceException("open failed");
		if (dup2(inputFD, 0) == -1)
			TraceException("dup2 failed");
		if (close(inputFD) == -1)
			TraceException("close failed");

		if ((outputFD = open(filename[1], O_WRONLY, S_IWUSR)) == -1)
			TraceException("open failed");
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
		clear();
	}
}

void Cgi::parseCgiResp() {
	std::string delim("\r\n\r\n");
	std::string::size_type	pos(_cgiResp.find(delim));

	if (pos != std::string::npos) {
		std::string fieldName;
		std::string fieldValue;
		std::string::size_type ptr;
		std::vector<std::string> hdr(ft::split(_cgiResp.substr(0, pos), ft::delimHeaders));

		for (size_t i = 0; i < hdr.size(); i++) {
			if ((ptr = hdr[i].find(":")) != std::string::npos) {
				fieldName = ft::tolower(hdr[i].substr(0, ptr));
				fieldValue = ft::trim(hdr[i].substr(ptr + 1), ft::delimConfig);
				if (fieldName == "status")
					_client->_httpStatusCode = ft::trim(ft::split(fieldValue, ft::delimConfig)[0], ft::delimConfig);
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

void Cgi::startCgi(Client *client, struct ft::TargetInfo *tgInfo) {
	_client = client;
	_tgInfo = tgInfo;

	findTarget();
	makeEnvVar();
	cgiExecve();
	parseCgiResp();
}
