#include "parser/Response.hpp"

Response::Response(const Data* data):	_data(data),\
                                    _headers(NULL) {
	_timeBuffer.reserve(100);
	_funcMap.insert(std::make_pair("GET", &Response::methodGET));
	_funcMap.insert(std::make_pair("HEAD", &Response::methodHEAD));
	_funcMap.insert(std::make_pair("POST", &Response::methodPOST));
	_funcMap.insert(std::make_pair("PUT", &Response::methodPUT));
	_funcMap.insert(std::make_pair("DELETE", &Response::methodDELETE));
	_funcMap.insert(std::make_pair("CONNECT", &Response::methodCONNECT));
	_funcMap.insert(std::make_pair("OPTIONS", &Response::methodOPTIONS));
	_funcMap.insert(std::make_pair("TRACE", &Response::methodTRACE));
}

Response::~Response() {}

int &Response::getDebug() {
	static int debug = 0;
	return (debug);
}

void Response::printDebugInfo() {
	if (this->getDebug() == 1) {
		std::cout << BLUE_B << BLUE << "headers:" << RESET << std::endl;
		std::cout << WHITE_B << *_headers << RESET << std::endl;
		if (_body->size() < 1000) {
			std::cout << BLUE_B << BLUE << "body:" << RESET << std::endl;
			std::cout << WHITE_B << *_body << RESET << std::endl;
		}
		if (_response.size() < 1000) {
			std::cout << BLUE_B << BLUE << "response:" << RESET << std::endl;
			std::cout << WHITE_B << _response << RESET << std::endl;
		}
	}
}

void Response::setClient(Client *client) {
	_client = client;
	_headers = &client->getHeaders();
	_body = &client->getBody();
	_httpStatusCode = client->getHttpStatusCode();
	_headersMap = &client->getHeadersMap();
	_response.clear();
}

int Response::isKeepAlive(Client::_clientIt &clientIt) {
	Client::_headersType& headersMap = (*clientIt)->getHeadersMap();

	if (headersMap.find("connection") != headersMap.end()) {
		if (headersMap["connection"].find("close") != std::string::npos)
			return (e_closeConnection);
	}
	else if (headersMap["http_version"] == "1.1" || headersMap["http_version"] == "2.0")
		return (e_recvHeaders);
	else if (headersMap["http_version"] == "1.0") {
		if (headersMap.find("connection") != headersMap.end()) {
			if (headersMap["connection"].find("keep-alive") != std::string::npos)
				return (e_recvHeaders);
		}
	}
	return (e_closeConnection);
}

void Response::methodGET() {
	if ((*_headersMap)["request_target"] == "/")
        _responseBody = readFile("config/index.html");
    else {
    	std::string filename = (*_headersMap)["request_target"];
		_responseBody = readFile(filename);
	}

	getStatus();
    getDate();
    getServer();
    getConnection();
    getContentType();
    getContentLength(_responseBody);
    getBlankLine();
    getContent(_responseBody);
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
    getContentLength(*_headers);
    getBlankLine();
    getContent(*_headers);
}

void Response::getStatus() {
    Client::_headersType& headersMap = _client->getHeadersMap();

    _response.append("HTTP/"+headersMap["http_version"]+" "+\
				_httpStatusCode->getStatusCode()+" "+\
				_data->getMessage(*_httpStatusCode)+"\r\n");
}

void Response::getDate() {
	_response.append(currentTime()+"\r\n");
}

void Response::getServer() {
	_response.append("Server: webserver-ALPHA\r\n");
}

void Response::getContentType(const std::string &filename) {
    if (_method == "TRACE") {
        _response.append("Content-Type: message/http\r\n");
    }
    else {
        static size_t dotPos;

        dotPos = filename.find_last_of('.');
        if (dotPos != std::string::npos) {
            static std::string extension;
            static Data::_mimeMapIt mimeIt;

            extension = filename.substr(dotPos + 1);
            mimeIt = _data->getMimeMap().find(extension);
            if (mimeIt != _data->getMimeMap().end())
                _response.append("Content-Type: " + mimeIt->second + "\r\n");
            return ;
        }
        _response.append("Content-Type: text/html\r\n");
    }
}

void Response::getContentLength(const std::string &content) {
    static std::ostringstream ss;

    ss << content.size();
    _response.append("Content-Length: " + ss.str() + "\r\n");
    ss.str( std::string() );
    ss.clear();
}

void Response::getConnection() {
    Client::_headersType& headersMap = _client->getHeadersMap();

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
	if ((*_headersMap).find("referer") != (*_headersMap).end()) {
		static std::string refPath;
		static std::string::size_type pos;

		pos = (*_headersMap)["referer"].rfind(':');
		if (pos != std::string::npos) {
			pos = (*_headersMap)["referer"].find('/', pos);
			if (pos != std::string::npos) {
				refPath = (*_headersMap)["referer"].substr(pos + 1);
				(*_headersMap)["request_target"] = (*_headersMap)["request_target"].substr(1);
				if (isValidFile(refPath))
					(*_headersMap)["request_target"].insert(0, refPath);
				else
					(*_headersMap)["request_target"].insert(0,_data->getErrorsDirectory());
			}
		}
	}
}

std::string& Response::getResponse(Client::_clientIt &clientIt) {

    setClient((*clientIt));
	try {
		getReferer();
		if (((*_headersMap)["request_target"])[0] == '/' && ((*_headersMap)["request_target"]) != "/")
			(*_headersMap)["request_target"].erase(0,1);
		std::cout << "(*_headersMap)[request_target]) = " << (*_headersMap)["request_target"] << std::endl;

		if (!isValidFile((*_headersMap)["request_target"]))
			throw HttpStatusCode("404");
		if (_httpStatusCode && _data->isErrorStatus(_httpStatusCode))
			throw *_httpStatusCode;
        _method = (*_client->getHeadersMap().find("method")).second;
		(this->*_funcMap.find(_method)->second)();
	}
	catch (const HttpStatusCode &httpStatusCode) {
		static std::string filename;

		filename = _data->getErrorPath(httpStatusCode);
		_responseBody = readFile(filename);
		getStatus();
		getServer();
		getDate();
		getContentType(filename);
        getContentLength(_responseBody);
        getConnection();
        getBlankLine();
        getContent(_responseBody);
	}
	catch (std::runtime_error& error) {
		std::cout << ", runtime_error = " << error.what() << std::endl;
	}
	printDebugInfo();
	return (_response);
}

void Response::sendResponse(Client::_clientIt &clientIt) {
	static Client* client;
	static std::string* response;
	static long valread;

	client = (*clientIt);
	response = &getResponse(clientIt);
	valread = send(client->getSocket(), response->c_str(), response->size(), MSG_DONTWAIT);
	client->setFlag(isKeepAlive(clientIt));
	client->wipeData();
}
