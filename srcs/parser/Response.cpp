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
	if ((*_headersMap)["request_target"] == "/")
		(*_headersMap)["request_target"] = "config/index.html";

	getStatus();
	getDate();
	getServer();
	getConnection();
	getLastModified();
	getContentType();
	getContentLength();
	getRetryAfter();
	getBlankLine();
	getContent(readFile((*_headersMap)["request_target"]));
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

		dotPos = (*_headersMap)["request_target"].find_last_of('.');
		if (dotPos != std::string::npos) {
			static std::string extension;
			static Data::_mimeMapIt mimeIt;

			extension = (*_headersMap)["request_target"].substr(dotPos + 1);
			mimeIt = _data->getMimeMap().find(extension);
			if (mimeIt != _data->getMimeMap().end())
				_response.append("Content-Type: "+mimeIt->second+"\r\n");
			return ;
		}
		_response.append("Content-Type: text/html\r\n");
	}
}

void Response::getContentLength() {
    std::ostringstream ss;

	ss << _fileStat.st_size;
	_response.append("Content-Length: "+ss.str()+"\r\n");
}

void Response::getConnection() {
	if (_client->isKeepAlive())
		_response.append("Connection: keep-alive\r\n");
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
	if ((*_headersMap).find("referer") != (*_headersMap).end()) {
		static std::string refPath;
		static std::string::size_type pos;

		pos = (*_headersMap)["referer"].rfind(':');
		if (pos != std::string::npos) {
			pos = (*_headersMap)["referer"].find('/', pos);
			if (pos != std::string::npos) {
				refPath = (*_headersMap)["referer"].substr(pos + 1);
				(*_headersMap)["request_target"] = (*_headersMap)["request_target"].substr(1);
				std::cout << "refPath = " << refPath << std::endl;
				if (refPath.empty() || isValidFile(refPath))
					(*_headersMap)["request_target"].insert(0, refPath);
				else
					(*_headersMap)["request_target"].insert(0,_data->getErrorsDirectory());
			}
		}
	}
}

void Response::getLastModified() {
	_response.append("Last-Modified: "+convertTime(_fileStat.st_mtime)+"\r\n");
}

// для случаев, когда произошла ошибка
void Response::getRetryAfter() {
	_response.append("Retry-After: 120\r\n");
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

	try {
		getReferer();
		if (((_client->_headersMap)["request_target"])[0] == '/' && ((_client->_headersMap)["request_target"]) != "/")
			(_client->_headersMap)["request_target"].insert(0,".");
		if (((_client->_headersMap)["request_target"]) == "/")
			((_client->_headersMap)["request_target"]) = "config/index.html";

		std::cout << "(_headersMap)[request_target]) = " << (_client->_headersMap)["request_target"] << std::endl;
		std::cout << "(_headersMap)[host]) = " << (_client->_headersMap)["host"] << std::endl;
		if (_method != "TRACE" && !isValidFile((_client->_headersMap)["request_target"]))
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

void Response::initResponse(Client *client) {
	_client = client;
	_headers = &client->_headers;
	_body = &client->_body;
	_httpStatusCode = &client->_httpStatusCode;
	_headersMap = &client->_headersMap;
	_method = _headersMap->find("method")->second;
	_response.clear();

	std::cout << "RESPONSE LOCATION" << std::endl;
	std::cout << *_client->_responseLocation << std::endl;
}

void Response::initTarget() {
	static size_t uriLen;

	_target = (*_headersMap)["request_target"];
	uriLen = _client->_responseLocation->_uri.size();
	std::cout << "_client->_responseLocation->_uri = " << _client->_responseLocation->_uri << std::endl;
	std::cout << "extract = " << _client->_headersMap["request_target"].substr(uriLen) << std::endl;
	std::cout << "path = " << _client->_responseLocation->_root+"/"+(_client->_headersMap)["request_target"].substr(uriLen) << std::endl;
	_target = _client->_responseLocation->_root+"/"+(_client->_headersMap)["request_target"].substr(uriLen);

}


void Response::sendResponse(Client *client) {
	static long valread;

	initResponse(client);
//	initTarget();
	getResponse();
	valread = send(client->_socket, _response.c_str(), _response.size(), MSG_DONTWAIT);
	std::cout << "valread = " << valread << std::endl;
	if (valread == -1)
		throw std::runtime_error("send error");
	client->responseSent();
}
