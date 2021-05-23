#include "parser/Response.hpp"

Response::Response(Data *data, Cgi *cgi):	_data(data),
											_client(NULL),
											_cgi(cgi) {
	_funcMap.insert(std::make_pair("GET", &Response::methodGET));
	_funcMap.insert(std::make_pair("HEAD", &Response::methodHEAD));
	_funcMap.insert(std::make_pair("POST", &Response::methodPOST));
	_funcMap.insert(std::make_pair("PUT", &Response::methodPUT));
	_funcMap.insert(std::make_pair("CONNECT", &Response::methodCONNECT));
	_funcMap.insert(std::make_pair("OPTIONS", &Response::methodOPTIONS));
	_funcMap.insert(std::make_pair("TRACE", &Response::methodTRACE));
}

Response::Response(const Response &other): 	_data(other._data),
										  	_client(other._client),
										  	_cgi(other._cgi),
										   	_tgInfo(other._tgInfo),
										   	_method(other._method),
										   	_funcMap(other._funcMap) {}

Response::~Response() {}

Response &Response::operator=(const Response& other) {
	if (this != &other) {
		_data = other._data;
		_client = other._client;
		_cgi = other._cgi;
		_tgInfo = other._tgInfo;
		_method = other._method;
		_funcMap = other._funcMap;
	}
	return (*this);
}

void Response::methodGET() {
	getStatus();
	getAllow();
	getAuthenticate();
	getDate();
	getServer();
	getConnection();
	getLastModified();
	getContentLanguage();
	getContentLength();
	getContentLocation();
	getContentType();
	getLocation();
	getRetryAfter();
	getBlankLine();
	getContent();
}

void Response::methodHEAD() {
	getStatus();
	getAllow();
	getAuthenticate();
	getDate();
	getServer();
	getConnection();
	getLastModified();
	getContentLanguage();
	getContentLength();
	getContentLocation();
	getContentType();
	getLocation();
	getRetryAfter();
	getBlankLine();
}

void Response::methodPOST() {
	getStatus();
	getAllow();
	getAuthenticate();
	getDate();
	getServer();
	getConnection();
	getContentLength();
	getContentLocation();
	getContentType();
	getLocation();
	getRetryAfter();
	getBlankLine();
	getContent();
}

void Response::methodPUT() {
	getStatus();
	getDate();
	getServer();
	getConnection();
	getLastModified();
	getLocation();
	getRetryAfter();
	getBlankLine();
}

void Response::methodCONNECT() {
	getStatus();
	getDate();
	getServer();
	getBlankLine();
}

void Response::methodOPTIONS() {
	getStatus();
	getAllow();
	getDate();
	getServer();
	getContentType();
	getBlankLine();
}

void Response::methodTRACE() {
	ft::getStringInfo(_client->_hdr + _client->_body, _tgInfo);
	getStatus();
	getDate();
	getServer();
	getConnection();
	getContentType();
	getContentLength();
	getBlankLine();
	getContent();
}

void Response::getStatus() {
	_client->_resp += 	"HTTP/"+_client->_hdrMap["http_version"]+" "+
						_client->_httpStatusCode.getStatusCode()+" "+
						_data->getMessage(_client->_httpStatusCode)+"\r\n";
}

void Response::getDate() {
	_client->_resp += "Date: "+ft::currentTime()+"\r\n";
}

void Response::getServer() {
	_client->_resp += "Server: webserver-ALPHA\r\n";
}

void Response::getContentLength() {
	_client->_resp += "Content-Length: "+_tgInfo._size+"\r\n";
}

void Response::getContentLocation() {
	_client->_resp += "Content-Location: "+_tgInfo._path+"\r\n";
}

void Response::getContentType() {
	if (_method == "TRACE")
		_client->_resp += "Content-Type: message/http\r\n";
	else {
		size_t dotPos = _tgInfo._path.rfind('.');

		if (dotPos != std::string::npos) {
			std::string extension(_tgInfo._path.substr(dotPos + 1));
			Data::_mimeMapIt mimeIt(_data->getMimeMap().find(extension));

			if (mimeIt != _data->getMimeMap().end()) {
				_client->_resp += "Content-Type: " + mimeIt->second + "\r\n";
			}
		}
		else
			_client->_resp += "Content-Type: text/html\r\n";
	}
}

void Response::getLocation() {
	_client->_resp += "Location: "+_client->_respLoc->_uri+"\r\n";

}

void Response::getConnection() {
	if (_client->isKeepAlive())
		_client->_resp += "Connection: keep-alive\r\n";
	else
		_client->_resp += "Connection: close\r\n";
}

void Response::getBlankLine() {
	_client->_resp += "\r\n";
}

void Response::getContent() {
	_client->_resp += _tgInfo._body;
}

void Response::getLastModified() {
	_client->_resp += "Last-Modified: "+_tgInfo._lstMod+"\r\n";
}

void Response::getRetryAfter() {
	_client->_resp += "Retry-After: 240\r\n";
}

void Response::getAllow() {
	if (_method == "OPTIONS" ||
		_client->_httpStatusCode.getStatusCode() == "405") {
		size_t i;

		_client->_resp += "Allow: ";
		if (_client->_respLoc->_allowed_method.empty()) {
			for (i = 0; i < ft::defaultAllowedMethodSize - 1; i++)
				_client->_resp += ft::defaultAllowedMethod[i]+", ";
			_client->_resp += ft::defaultAllowedMethod[i];
		}
		else {
			for (i = 0; i < _client->_respLoc->_allowed_method.size() - 1; i++)
				_client->_resp += _client->_respLoc->_allowed_method[i]+", ";
			_client->_resp += _client->_respLoc->_allowed_method[i];
		}
		_client->_resp += "\r\n";
	}
}

void Response::getAuthenticate() {
	if (_client->_httpStatusCode.getStatusCode() == "401") {
		_client->_resp += "WWW-Authenticate: Basic realm=\""+_client->_respLoc->_auth_basic+"\""+"\r\n";
	}
}

void Response::getContentLanguage() {
	if (!_client->_cntntLang.empty())
		_client->_resp += "Content-Language: "+_client->_cntntLang+"\r\n";
}

void Response::initErrorFile(const HttpStatusCode &httpStatusCode) {
	_client->_httpStatusCode = httpStatusCode;
	_tgInfo._path = _data->getErrorPath(_client);
	ft::getFileInfo(_tgInfo._path, _tgInfo);
	if (_method != "HEAD") {
		_tgInfo._body = ft::readFile(_tgInfo._path);
		_method = "GET";
	}
}

void Response::initAutoIndex() {
	if (!ft::isEndWith(_tgInfo._path, "/"))
		_tgInfo._path += '/';
	for (		std::vector<std::string>::const_iterator indexIt = _client->_respLoc->_index.begin();
				 indexIt != _client->_respLoc->_index.end();
				 indexIt++) {
		findTarget(_tgInfo._path + *indexIt);
		if (_tgInfo._type == filetype::e_valid)
			break;
	}
	if (_tgInfo._type != filetype::e_valid &&	!_client->_respLoc->_autoindex)
		throw HttpStatusCode("404");
	else if (_tgInfo._type != filetype::e_valid && _client->_respLoc->_autoindex)
		constructAutoIndex();
}

void Response::findTarget(const std::string &filepath) {
	std::vector<std::string> acptLang;
	std::vector<std::string> candFiles;

	acptLang = ft::split(_client->_hdrMap["accept-language"], ",;");
	for (size_t i = 0; i < acptLang.size(); i++) {
		if (acptLang[i].find("=") == std::string::npos)
			candFiles.push_back(acptLang[i]);
	}
	candFiles.push_back("");

	for (size_t i = 0; i < candFiles.size(); i++) {
		std::string path = filepath;

		if (!candFiles[i].empty())
			path += "."+candFiles[i];
		ft::getFileInfo(path, _tgInfo);
		if (_tgInfo._type == filetype::e_valid ||
			_tgInfo._type == filetype::e_directory) {
			_tgInfo._path = path;
			_client->_cntntLang = candFiles[i];
			if (_tgInfo._type == filetype::e_valid && _method != "HEAD")
				_tgInfo._body = ft::readFile(_tgInfo._path);
			break;
		}
	}
}

void Response::constructResp() {
	std::string target = _client->_hdrMap["request_target"].substr(_client->_respLoc->_uri.size());

	_tgInfo._path = _client->_respLoc->_root;
	if (!ft::isStartWith(target, "/"))
		_tgInfo._path += "/";
	_tgInfo._path += target;

	try {
		if (_data->isErrorStatus(&_client->_httpStatusCode))
			throw HttpStatusCode(_client->_httpStatusCode);
		authorization();
		if (	_method == "GET" ||
			 	_method == "HEAD") {
			findTarget(_tgInfo._path);
			switch (_tgInfo._type) {
				case filetype::e_valid:
					_tgInfo._body = ft::readFile(_tgInfo._path);
					break;
				case filetype::e_invalid:
					throw HttpStatusCode("400");
				case filetype::e_directory:
					initAutoIndex();
					break;
				case filetype::e_file_type_error:
					throw HttpStatusCode("404");
			}
		}
		else if (_method == "POST") {
			_cgi->startCgi(_client, &_tgInfo);
		}
		else if (_method == "PUT") {
			ft::getFileInfo(_tgInfo._path, _tgInfo);
			switch (_tgInfo._type) {
				case filetype::e_file_type_error:
					_client->_httpStatusCode = HttpStatusCode("201");
					break;
				default:
					_client->_httpStatusCode = HttpStatusCode("204");
					break;
			}

			try {
				int fd;
				long valread;

				if ((fd = open(_tgInfo._path.c_str(), O_TRUNC | O_CREAT | O_WRONLY, 0666)) == -1)
					TraceException("open failed");
				if ((valread = write(fd, _client->_body.c_str(), _client->_body.size())) == -1)
					TraceException("write failed");
				if (valread >= 0 && static_cast<size_t>(valread) != _client->_body.size())
					TraceException("write failed");
				if (close(fd) == -1)
					TraceException("close failed");
			}
			catch (std::runtime_error &runtimeError) {
				std::cerr << "runtimeError: " << runtimeError.what();
				throw HttpStatusCode("500");
			}
		}
	}
	catch (HttpStatusCode &httpStatusCode) {
		initErrorFile(httpStatusCode);
	}
	(this->*_funcMap.find(_method)->second)();
	_client->_sendLeftBytes = _client->_resp.size();
}

void Response::constructAutoIndex() {
	DIR *dir;
	struct dirent *object;
	struct ft::TargetInfo tmp;
	std::string tmpStr;
	size_t printOffset = 50;
	std::vector<std::string> filesInDir;

	if (!(dir = opendir(_tgInfo._path.c_str())))
		TraceException("opendir failed");

	if (!(object = readdir(dir)))
		TraceException("readdir failed");
	while (object) {
		filesInDir.push_back(object->d_name);
		object = readdir(dir);
	}
	if (closedir(dir) == -1)
		TraceException("closedir failed");

	filesInDir.erase(std::remove(filesInDir.begin(), filesInDir.end(), "."), filesInDir.end());
	std::sort(filesInDir.begin(), filesInDir.end());

	_tgInfo._body =	(	"<html>\n"
						"<head><title>Index of "+_client->_hdrMap["request_target"]+"</title></head>\n"
						"<body>\n"
						"<h1>Index of "+_client->_hdrMap["request_target"]+"</h1>\n"
						"<hr><pre>");

	for (size_t i = 0; i < filesInDir.size(); i++)
	{
		tmpStr = filesInDir[i];

		ft::getFileInfo(_tgInfo._path + tmpStr, tmp);
		if (tmp._type == filetype::e_directory && !tmpStr.empty())
			tmpStr += tmpStr[tmpStr.size()-1] != '/' ? "/" : "";

		_tgInfo._body += "<a href=\"";
		_tgInfo._body += tmpStr;
		_tgInfo._body += "\">";
		_tgInfo._body += tmpStr;
		_tgInfo._body += "</a>";
		for (size_t j = tmpStr.size(); j < printOffset; j++)
			_tgInfo._body += " ";

		tmpStr = ft::convertTime(tmp._stat.st_mtime);
		_tgInfo._body += tmpStr;

		for (size_t j = tmpStr.size(); j < printOffset; j++)
			_tgInfo._body += " ";

		_tgInfo._body += tmp._type == filetype::e_valid ? tmp._size : "-";
		_tgInfo._body += "\n";
	}
	_tgInfo._body += (	"</pre><hr></body>\n"
						"</html>\n");
	_tgInfo._size = ft::valueToString(_tgInfo._body.size());
	_tgInfo._lstMod = ft::currentTime();
}

void Response::authorization() {
	if (!(_client->_respLoc->_auth_basic.empty())) {
		if (_client->_hdrMap.find("authorization") != _client->_hdrMap.end()) {
			std::vector<std::string> authoriz(ft::split(_client->_hdrMap["authorization"], ft::delimConfig));

			if (authoriz.size() != 2 || authoriz[0] != "Basic")
				throw HttpStatusCode("401");
			else {
				struct ft::TargetInfo tmp;
				ft::getFileInfo(_client->_respLoc->_auth_basic_user_file, tmp);

				switch (tmp._type) {
					case filetype::e_valid:
					{
						std::vector<std::string> users(ft::split(ft::readFile(_client->_respLoc->_auth_basic_user_file), "\n"));
						std::for_each(users.begin(), users.end(), &Base64::encodeInPlace);
						if (std::find(users.begin(), users.end(), authoriz[1]) == users.end())
							throw HttpStatusCode("401");
						break;
					}
					default:
						throw HttpStatusCode("400");
				}
			}
		}
		else {
			throw HttpStatusCode("401");
		}
	}
}

void Response::sendPart() {
	if (_client->_sendLeftBytes) {
		size_t bytesToSend = ft::bufferSize > _client->_sendLeftBytes ? _client->_sendLeftBytes : ft::bufferSize;
		_client->_valread = send(_client->_socket, &_client->_resp[_client->_sendBytes], bytesToSend, MSG_DONTWAIT);
		if (_client->_valread == -1)
			TraceException("send failed");
		_client->_sendBytes += _client->_valread;
		_client->_sendLeftBytes -= _client->_valread;
	}
}

void Response::sendResponse(Client *client) {
	_client = client;
	_method = _client->_hdrMap["method"];

	if (!_client->_sendLeftBytes) {
		constructResp();
		if (ft::getDebug())
			std::cout << *this << std::endl;
	}
	sendPart();
	_client->clear(_data->isErrorStatus(_client));
}
