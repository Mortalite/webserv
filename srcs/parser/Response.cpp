#include "parser/Response.hpp"

/*
** Делаю ассоциативный массив - (метод, функция для построения ответа)
*/
Response::Response(const Data* data):_data(data) {
	_funcMap.insert(std::make_pair("GET", &Response::methodGET));
	_funcMap.insert(std::make_pair("HEAD", &Response::methodHEAD));
	_funcMap.insert(std::make_pair("POST", &Response::methodPOST));
	_funcMap.insert(std::make_pair("PUT", &Response::methodPUT));
	_funcMap.insert(std::make_pair("DELETE", &Response::methodDELETE));
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
	getDate();
	getServer();
	getConnection();
	getLastModified();
	getContentType();
	getContentLength();
	getRetryAfter();
	getBlankLine();
	getContent();
}

void Response::methodHEAD() {
	getStatus();
	getDate();
	getServer();
	getConnection();
	getLastModified();
	getContentType();
	getContentLength();
	getRetryAfter();
	getBlankLine();
}

void Response::methodPOST() {
	Cgi * cgi_post = new Cgi();
	cgi_post->start_cgi(*this);
	getDate();
	getBlankLine();
}

void Response::methodPUT() {
	getDate();
	getBlankLine();
}

void Response::methodDELETE() {
	getDate();
	getBlankLine();
}

void Response::methodCONNECT() {
	getStatus();
	getDate();
	getServer();
	getBlankLine();
}

/*
** Надо в зависимости от конфигурации добавлять разрешения
*/
void Response::methodOPTIONS() {
	getStatus();

	getDate();
	getServer();
	getContentType();
	getBlankLine();
}

void Response::methodTRACE() {
	getTargetInfoString(_client->_hdr + _client->_body, _tgInfo);
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

void Response::getContentLength() {
	_client->_resp += "Content-Length: "+_tgInfo._size+"\r\n";
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

void Response::initClient(Client *client) {
	_client = client;
	_method = _client->_hdrMap.find("method")->second;

	if (getDebug()) {
		std::cout << "RESPONSE LOCATION" << std::endl;
		std::cout << *_client->_respLoc << std::endl;
	}
}

void Response::initPath() {
	_tgInfo._path = _client->_respLoc->_root+"/"+
					_client->_hdrMap["request_target"].substr(_client->_respLoc->_uri.size());
	if (!_tgInfo._path.empty() && _tgInfo._path[0] == '/')
		_tgInfo._path.insert(0, ".");
}

void Response::initErrorFile(const HttpStatusCode &httpStatusCode) {
	_client->_httpStatusCode = httpStatusCode;
	_tgInfo._path = _data->getErrorPath(_client);
	getTargetInfoFile(_tgInfo._path, _tgInfo);
}

void Response::initAutoIndex() {
	for (		std::vector<std::string>::const_iterator indexIt = _client->_respLoc->_index.begin();
				 indexIt != _client->_respLoc->_index.end();
				 indexIt++) {
		getTargetInfoFile(_tgInfo._path+*indexIt, _tgInfo);
		if (_tgInfo._type == e_valid) {
			_tgInfo._path += *indexIt;
			break;
		}
	}
	if (_tgInfo._type != e_valid &&	!_client->_respLoc->_autoindex)
		initErrorFile(HttpStatusCode("403"));
	else if (_tgInfo._type != e_valid && _client->_respLoc->_autoindex) {
		static DIR *dir;
		static struct dirent *object;
		static struct TgInfo tmp;
		static std::string tmpStr;
		static int printOffset = 50;

		_isAutoIndex = true;
		if (!(dir = opendir(_tgInfo._path.c_str())))
			throw std::runtime_error("opendir autoindex failed");

		_tgInfo._body.clear();
		_tgInfo._body +=	("<html>\n"
							"<head><title>Index of "+_client->_hdrMap["request_target"]+"</title></head>\n"
							"<body>\n"
							"<h1>Index of "+_client->_hdrMap["request_target"]+"</h1>\n"
							"<hr><pre>");

		object = readdir(dir);
		while (object) {
			tmpStr = object->d_name;

			getTargetInfoFile(_tgInfo._path+tmpStr, tmp);
			if (tmp._type == e_directory)
				tmpStr += '/';

			_tgInfo._body += "<a href=\""+tmpStr+"\">"+tmpStr+"</a>";

			for (size_t i = tmpStr.size(); i < printOffset; i++)
				_tgInfo._body += " ";

			tmpStr = convertTime(tmp._stat.st_mtime);
			_tgInfo._body += tmpStr;

			for (size_t i = tmpStr.size(); i < printOffset; i++)
				_tgInfo._body += " ";

			if (tmp._type == e_valid)
				_tgInfo._body += tmp._size;
			else
				_tgInfo._body += "-";
			_tgInfo._body += "\n";
			object = readdir(dir);
		}
		_tgInfo._body += ("</pre><hr></body>\n"
						 "</html>\n");
		_tgInfo._size = ossToString(_tgInfo._body.size());
		_tgInfo._lstMod = currentTime();
		closedir(dir);
	}
}

void Response::constructResp() {
	if (_method == "GET" ||
		_method == "HEAD")	{
		if (_data->isErrorStatus(&_client->_httpStatusCode))
			initErrorFile(_client->_httpStatusCode);
		else {
			_isAutoIndex = false;
			getTargetInfoFile(_tgInfo._path, _tgInfo);

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

void Response::sendPart() {
	if (_client->_sendLeftBytes) {
		static long valread;

		_client->_resp.reserve(_client->_sendLeftBytes);
		valread = send(_client->_socket, &_client->_resp[_client->_sendBytes], _client->_sendLeftBytes, MSG_DONTWAIT);
		if (valread == -1)
			throw std::runtime_error("send failed");
		_client->_sendBytes += valread;
		_client->_sendLeftBytes -= valread;
	}
}

void Response::getResponse() {
	initPath();
	constructResp();
	(this->*_funcMap.find(_method)->second)();
	_client->_sendLeftBytes = _client->_resp.size();
}

void Response::sendResponse(Client *client) {
	initClient(client);
	if (!_client->_sendLeftBytes)
		getResponse();
	sendPart();
	std::cout << *this << std::endl;
	client->responseSent();
}

const Data * Response::getData(){
	return _data;
}

const Client * Response::getClient(){
	return _client;
}