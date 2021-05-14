#include "parser/Response.hpp"

/*
** Делаю ассоциативный массив - (метод, функция для построения ответа)
*/
Response::Response(const Data* data):_data(data) {
	_funcMap.insert(std::make_pair("GET", &Response::methodGET));
	_funcMap.insert(std::make_pair("HEAD", &Response::methodHEAD));
	_funcMap.insert(std::make_pair("POST", &Response::methodPOST));
	_funcMap.insert(std::make_pair("PUT", &Response::methodPUT));
	_funcMap.insert(std::make_pair("CONNECT", &Response::methodCONNECT));
	_funcMap.insert(std::make_pair("OPTIONS", &Response::methodOPTIONS));
	_funcMap.insert(std::make_pair("TRACE", &Response::methodTRACE));
}

Response::Response(const Response &other): _data(other._data),
										   _client(other._client),
										   _tgInfo(other._tgInfo),
										   _method(other._method),
										   _funcMap(other._funcMap) {}

Response::~Response() {}

Response &Response::operator=(const Response& other) {
	if (this != &other) {
		_data = other._data;
		_client = other._client;
		_tgInfo = other._tgInfo;
		_method = other._method;
		_funcMap = other._funcMap;
	}
	return (*this);
}

/*
** Тут в зависимости от настроек index конфигурации сервера или локации,
** надо открывать нужный файл
*/
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
	getDate();
	getBlankLine();
}

void Response::methodPUT() {
	getDate();
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
	getStringInfo(_client->_hdr + _client->_body, _tgInfo);
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
	_client->_resp += "Date: "+currentTime()+"\r\n";
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
		static size_t dotPos;

		dotPos = _tgInfo._path.rfind('.');
		if (dotPos != std::string::npos) {
			static std::string extension;
			static Data::_mimeMapIt mimeIt;

			extension = _tgInfo._path.substr(dotPos + 1);
			mimeIt = _data->getMimeMap().find(extension);
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

// для случаев, когда произошла ошибка
void Response::getRetryAfter() {
	_client->_resp += "Retry-After: 120\r\n";
}

void Response::getAllow() {
	if (_method == "OPTIONS" ||
		_client->_httpStatusCode.getStatusCode() == "405") {
		static size_t i;

		_client->_resp += "Allow: ";
		if (_client->_respLoc->_allowed_method.empty()) {
			for (i = 0; i < defaultAllowedMethodSize - 1; i++)
				_client->_resp += defaultAllowedMethod[i]+", ";
			_client->_resp += defaultAllowedMethod[i];
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
	getFileInfo(_tgInfo._path, _tgInfo);
}

void Response::initAutoIndex() {
	for (		std::vector<std::string>::const_iterator indexIt = _client->_respLoc->_index.begin();
				 indexIt != _client->_respLoc->_index.end();
				 indexIt++) {
		findTarget(_tgInfo._path + *indexIt);
		if (_tgInfo._type == e_valid)
			break;
	}
	if (_tgInfo._type != e_valid &&	!_client->_respLoc->_autoindex)
		initErrorFile(HttpStatusCode("403"));
	else if (_tgInfo._type != e_valid && _client->_respLoc->_autoindex)
		constructAutoIndex();
}

void Response::findTarget(std::string filepath) {
	static size_t i;
	std::vector<std::string> acptLang;
	std::vector<std::pair<std::string, std::string> > candFiles;

	acptLang = split(_client->_hdrMap["accept-language"], ",;");
	for (i = 0; i < acptLang.size(); i++) {
		if (acptLang[i].find("=") == std::string::npos)
			candFiles.push_back(std::make_pair(filepath, acptLang[i]));
	}
	candFiles.push_back(std::make_pair(filepath, ""));

	for (i = 0; i < candFiles.size(); i++) {
		static std::string path;

		path = candFiles[i].first;
		if (!candFiles[i].second.empty())
			path += "."+candFiles[i].second;
		getFileInfo(path, _tgInfo);
		if (_tgInfo._type == e_valid ||
			_tgInfo._type == e_directory) {
			_client->_cntntLang = candFiles[i].second;
			_tgInfo._path = path;
			break;
		}
	}
}

void Response::constructResp() {
	if (_method == "GET" ||
		_method == "HEAD")	{
		if (_data->isErrorStatus(&_client->_httpStatusCode))
			initErrorFile(_client->_httpStatusCode);
		else {
			_isAutoIndex = false;
			findTarget(_tgInfo._path);
			switch (_tgInfo._type) {
				case e_invalid:
					initErrorFile(HttpStatusCode("400"));
					break;
				case e_directory:
					initAutoIndex();
					break;
				case e_file_type_error:
					initErrorFile(HttpStatusCode("404"));
					break;
			}
			if (!_isAutoIndex && _method == "GET") {
				_tgInfo._body = readFile(_tgInfo._path);
			}
		}
	}
}

void Response::constructAutoIndex() {
	static DIR *dir;
	static struct dirent *object;
	static struct TgInfo tmp;
	static std::string tmpStr;
	static int printOffset = 50;

	_isAutoIndex = true;
	if (!(dir = opendir(_tgInfo._path.c_str())))
		throw std::runtime_error("opendir autoindex failed");

	_tgInfo._body.clear();
	_tgInfo._body +=	(	"<html>\n"
						 	"<head><title>Index of "+_client->_hdrMap["request_target"]+"</title></head>\n"
					   		"<body>\n"
						 	"<h1>Index of "+_client->_hdrMap["request_target"]+"</h1>\n"
							"<hr><pre>");

	object = readdir(dir);
	while (object) {
		tmpStr = object->d_name;

		getFileInfo(_tgInfo._path + tmpStr, tmp);
		if (tmp._type == e_directory)
			tmpStr += '/';

		_tgInfo._body += "<a href=\""+tmpStr+"\">"+tmpStr+"</a>";

		for (size_t i = tmpStr.size(); i < printOffset; i++)
			_tgInfo._body += " ";

		tmpStr = convertTime(tmp._stat.st_mtime);
		_tgInfo._body += tmpStr;

		for (size_t i = tmpStr.size(); i < printOffset; i++)
			_tgInfo._body += " ";

		_tgInfo._body += tmp._type == e_valid ? tmp._size : "-";
		_tgInfo._body += "\n";
		object = readdir(dir);
	}
	_tgInfo._body += (	"</pre><hr></body>\n"
					  	"</html>\n");
	_tgInfo._size = valueToString(_tgInfo._body.size());
	_tgInfo._lstMod = currentTime();
	closedir(dir);
}

void Response::authorization() {
	if (!(_client->_respLoc->_auth_basic.empty())) {
		if (_client->_hdrMap.find("authorization") != _client->_hdrMap.end()) {
			static std::vector<std::string> splitBuffer;

			splitBuffer = split(_client->_hdrMap["authorization"], " ");
			if (splitBuffer.size() == 2 && toLower(splitBuffer[0]) == "basic") {

			}
		}
		else {
			initErrorFile(HttpStatusCode("401"));
		}
	}
}

void Response::prepResp() {
	_tgInfo._path = _client->_respLoc->_root+"/"+
					_client->_hdrMap["request_target"].substr(_client->_respLoc->_uri.size());
	if (!_tgInfo._path.empty() && _tgInfo._path[0] == '/')
		_tgInfo._path.insert(0, ".");

	authorization();
	constructResp();
	(this->*_funcMap.find(_method)->second)();
	_client->_sendLeftBytes = _client->_resp.size();
}

void Response::sendResponse(Client *client) {
	_client = client;
	_method = _client->_hdrMap.find("method")->second;

	if (!_client->_sendLeftBytes)
		prepResp();
	if (getDebug())
		std::cout << *this << std::endl;
	client->sendPart();
	client->responseSent();
}

