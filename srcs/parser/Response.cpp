#include "parser/Response.hpp"

/*
** Делаю ассоциативный массив - (метод, функция для построения ответа)
*/
Response::Response(const Data* data):	_data(data) {
	_funcMap.insert(std::make_pair("GET", &Response::methodGET));
	_funcMap.insert(std::make_pair("HEAD", &Response::methodHEAD));
	_funcMap.insert(std::make_pair("POST", &Response::methodPOST));
	_funcMap.insert(std::make_pair("PUT", &Response::methodPUT));
	_funcMap.insert(std::make_pair("DELETE", &Response::methodDELETE));
	_funcMap.insert(std::make_pair("CONNECT", &Response::methodCONNECT));
	_funcMap.insert(std::make_pair("OPTIONS", &Response::methodOPTIONS));
	_funcMap.insert(std::make_pair("TRACE", &Response::methodTRACE));
}

Response::Response(const Response &other):  _data(other._data),
											_method(other._method),
											_response(other._response),
											_responseBody(other._responseBody),
											_funcMap(other._funcMap) {}

Response::~Response() {}

Response &Response::operator=(const Response& other) {
	if (this != &other) {
		_data = other._data;
		_method = other._method;
		_response = other._response;
		_responseBody = other._responseBody;
		_funcMap = other._funcMap;
	}
	return (*this);
}

/*
** Проверка разрывать или нет соединение
*/
int Response::isKeepAlive() {
	Client::_headersType& headersMap = _client->_headersMap;

	if (headersMap.find("connection") != headersMap.end()) {
		if (headersMap["connection"].find("close") != std::string::npos)
			return (e_closeConnection);
	}
	else if (headersMap["http_version"] == "1.1")
		return (e_recvHeaders);
	else if (headersMap["http_version"] == "1.0") {
		if (headersMap.find("connection") != headersMap.end()) {
			if (headersMap["connection"].find("keep-alive") != std::string::npos)
				return (e_recvHeaders);
		}
	}
	return (e_closeConnection);
}

/*
** Проверяю есть ли файл, потом типы
*/
bool Response::isValidFile(std::string& fileName) {
	static int ret;

	ret = stat(fileName.c_str(), &_fileStat);
	if (ret == -1)
		return (false);
	switch (_fileStat.st_mode & S_IFMT) {
		case S_IFBLK:  return (false); // Block device
		case S_IFCHR:  return (false); // Character device
		case S_IFDIR:  return (false); // Directory
		case S_IFIFO:  return (true); // FIFO/PIPE
		case S_IFLNK:  return (true); // Symlink
		case S_IFREG:  return (true); // Regular file
		case S_IFSOCK: return (true); // Socket
		default:       return (false); // Unknown
	}
}

/*
** Тут в зависимости от настроек index конфигурации сервера или локации,
** надо открывать нужный файл
*/
void Response::methodGET() {
	Client::_headersType& headersMap = _client->_headersMap;

	if (headersMap["request_target"] == "/")
		headersMap["request_target"] = "config/index.html";

	getStatus();
	getDate();
	getServer();
	getConnection();
	getLastModified();
	getContentType();
	getContentLength();
	getBlankLine();
	getContent(readFile(headersMap["request_target"]));
}

void Response::methodHEAD() {
	getStatus();
	getDate();
	getServer();
	getConnection();
	getContentType();
}

void Response::methodPOST() {
	getDate();
}

void Response::methodPUT() {
	getDate();
}

void Response::methodDELETE() {
	getDate();
}

void Response::methodCONNECT() {
	getStatus();
	getDate();
	getServer();
}

/*
** Надо в зависимости от конфигурации добавлять разрешения
*/
void Response::methodOPTIONS() {
	getStatus();

	getDate();
	getServer();
	getContentType();
}

void Response::methodTRACE() {
	getStatus();
	getDate();
	getServer();
	getConnection();
	getContentType();
	getContentLength();
	getBlankLine();
	getContent(_client->_headers+_client->_body);
}

void Response::getStatus() {
	_response.append("HTTP/"+_client->_headersMap["http_version"]+" "+\
				_client->_httpStatusCode.getStatusCode()+" "+\
				_data->getMessage(_client->_httpStatusCode)+"\r\n");
}

void Response::getDate() {
	_response.append("Date: "+currentTime()+"\r\n");
}

void Response::getServer() {
	_response.append("Server: webserver-ALPHA\r\n");
}

void Response::getContentType() {
	if (_method == "TRACE")
		_response.append("Content-Type: message/http\r\n");
	else {
		static size_t dotPos;

		dotPos = _client->_headersMap["request_target"].find_last_of('.');
		if (dotPos != std::string::npos) {
			static std::string extension;
			static Data::_mimeMapIt mimeIt;

			extension = _client->_headersMap["request_target"].substr(dotPos + 1);
			mimeIt = _data->getMimeMap().find(extension);
			if (mimeIt != _data->getMimeMap().end())
				_response.append("Content-Type: " + mimeIt->second + "\r\n");
			return ;
		}
		_response.append("Content-Type: text/html\r\n");
	}
}

void Response::getContentLength() {
    std::ostringstream ss;

	ss << _fileStat.st_size;
	_response.append("Content-Length: " + ss.str() + "\r\n");
}

void Response::getConnection() {
	Client::_headersType& headersMap = _client->_headersMap;

	if (headersMap.find("connection") != headersMap.end()) {
		if (headersMap["connection"].find("close") != std::string::npos)
			_response.append("Connection: close\r\n");
	}
	else if (headersMap["http_version"] == "1.1" || headersMap["http_version"] == "2.0")
		_response.append("Connection: keep-alive\r\n");
	else if (headersMap["http_version"] == "1.0") {
		if (headersMap.find("connection") != headersMap.end()) {
			if (headersMap["connection"].find("keep-alive") != std::string::npos)
				_response.append("Connection: keep-alive\r\n");
		}
	}
	else
		_response.append("Connection: close\r\n");
}

void Response::getBlankLine() {
	_response.append("\r\n");
}

void Response::getContent(const std::string &content) {
	_response.append(content);
}

void Response::getReferer() {
	Client::_headersType& headersMap = _client->_headersMap;

	if (headersMap.find("referer") != headersMap.end()) {
		static std::string refPath;
		static std::string::size_type pos;

		pos = headersMap["referer"].rfind(':');
		if (pos != std::string::npos) {
			pos = headersMap["referer"].find('/', pos);
			if (pos != std::string::npos) {
				refPath = headersMap["referer"].substr(pos + 1);
				headersMap["request_target"] = headersMap["request_target"].substr(1);
				std::cout << "refPath = " << refPath << std::endl;
				if (refPath.empty() || isValidFile(refPath))
					headersMap["request_target"].insert(0, refPath);
				else
					headersMap["request_target"].insert(0,_data->getErrorsDirectory());
			}
		}
	}
}

void Response::getLastModified() {
	_response.append("Last-Modified: "+convertTime(_fileStat.st_mtime)+"\r\n");
}

void Response::getErrorPage() {
	static std::string filename;

	filename = _data->getErrorPath(_client->_httpStatusCode);
	stat(filename.c_str(), &_fileStat);
	getStatus();
	getServer();
	getDate();
	getContentType();
	getContentLength();
	getConnection();
	getBlankLine();
	getContent(readFile(filename));
}

void Response::getResponse() {
	_response.clear();
	_method = (_client->_headersMap.find("method"))->second;

	try {
		getReferer();
		if (((_client->_headersMap)["request_target"])[0] == '/' && ((_client->_headersMap)["request_target"]) != "/")
			(_client->_headersMap)["request_target"].insert(0,".");
		if (((_client->_headersMap)["request_target"]) == "/")
			((_client->_headersMap)["request_target"]) = "config/index.html";

		std::cout << "(_headersMap)[request_target]) = " << (_client->_headersMap)["request_target"] << std::endl;

		if (!isValidFile((_client->_headersMap)["request_target"]))
			throw HttpStatusCode("404");
		if (_data->isErrorStatus(&_client->_httpStatusCode))
			throw _client->_httpStatusCode;
		(this->*_funcMap.find(_method)->second)();
	}
	catch (const HttpStatusCode &httpStatusCode) {
		_client->_httpStatusCode = httpStatusCode;
		getErrorPage();
	}
	std::cout << *this << std::endl;
}

void Response::sendResponse(Client *client) {
	static long valread;

	_client = client;
	getResponse();
	valread = send(client->_socket, _response.c_str(), _response.size(), MSG_DONTWAIT);
	client->_flag = isKeepAlive();
	client->wipeData();
}
