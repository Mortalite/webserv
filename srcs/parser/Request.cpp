#include "parser/Request.hpp"

Request::Request(const Data* data):	_data(data),\
                                    _headers(NULL) {
	_timeBuffer.reserve(100);
	_funcMap.insert(std::make_pair("GET", &Request::methodGET));
	_funcMap.insert(std::make_pair("HEAD", &Request::methodHEAD));
	_funcMap.insert(std::make_pair("POST", &Request::methodPOST));
	_funcMap.insert(std::make_pair("PUT", &Request::methodPUT));
	_funcMap.insert(std::make_pair("DELETE", &Request::methodDELETE));
	_funcMap.insert(std::make_pair("CONNECT", &Request::methodCONNECT));
	_funcMap.insert(std::make_pair("OPTIONS", &Request::methodOPTIONS));
	_funcMap.insert(std::make_pair("TRACE", &Request::methodTRACE));
}

Request::~Request() {}

void Request::parseHeaders(Client::_clientIt &clientIt) {
    Client::_headersType& headersMap = (*clientIt)->getHeadersMap();
	static std::vector<std::string> headers;
	static std::vector<std::string> requestLine;
	static std::string::size_type ptr;

    headers = split((*clientIt)->getHeaders(), "\r\n");
	requestLine = split(headers[0], " ");
	if (requestLine.size() != 3 ||\
	((ptr = requestLine[2].find("/")) == std::string::npos) ||\
	!isAllowedMethod(requestLine[0])) {
        headersMap["http_version"] = "1.1";
        throw HttpStatusCode("400");
    }
	else {
        headersMap["method"] = requestLine[0];
        headersMap["request_target"] = requestLine[1];
        headersMap["http_version"] = requestLine[2].substr(ptr + 1);
	}

	static std::string field_name;
	static std::string field_value;
	static std::string header_delim( " \t");

	for (size_t i = 1; i < headers.size(); i++) {
		if ((ptr = headers[i].find(":")) != std::string::npos) {
			field_name = headers[i].substr(0, ptr);
			field_value = trim(headers[i].substr(ptr + 1), header_delim);
            headersMap[toLower(field_name)] = toLower(field_value);
		}
		else if (!headers[i].empty())
			throw HttpStatusCode("400");
	}
	if (headersMap["http_version"] == "1.1" || headersMap["http_version"] == "2.0") {
	    if (headersMap.find("host") == headersMap.end())
            throw HttpStatusCode("400");
	}

//	DEBUG
/*	std::cout << "Parse headers" << std::endl;
	int count = 0;
	for (_headersType::iterator i = _headersMap.begin(); i != _headersMap.end(); i++)
		std::cout << "result[" << count++ << "] = " << "(" << (*i).first << ", " << (*i).second << ")" << std::endl;*/
}

std::pair<int, long> Request::getBodyType(Client::_clientIt &clientIt) {
    Client::_headersType& headersMap = (*clientIt)->getHeadersMap();

    if (headersMap.find("transfer-encoding") != headersMap.end()) {
		if (headersMap["transfer-encoding"].find("chunked") != std::string::npos)
			return (std::make_pair(e_recvChunkBody, 0));
	}
	else if (headersMap.find("content-length") != headersMap.end()) {
		char *ptr;
		long content_length;

		content_length = strtol(headersMap["content-length"].c_str(), &ptr, 10);
		if (!(*ptr))
			return (std::make_pair(e_recvContentBody, content_length));
	}
	return (std::make_pair(e_sendResponse, 0));
}

int Request::isKeepAlive(Client::_clientIt &clientIt) {
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

bool Request::isAllowedMethod(const std::string &method) {
	static std::string allowedMethods[8] = { "GET", "HEAD", "POST", "PUT",\
											"DELETE", "CONNECT", "OPTIONS", "TRACE" };
	static int numOfMethods = 8;
	static int i;

	i = 0;
	for (;i < numOfMethods; i++) {
		if (method == allowedMethods[i])
			return (true);
	}
	return (false);
}

void Request::methodGET() {

	if ((*_headersMap)["request_target"] == "/")
        _responseBody = readFile("config/index.html");
    else {
    	std::string filename = (*_headersMap)["request_target"];
//		filename = trim(filename, "/");
		_responseBody = readFile(filename);
	}
    std::cout << "GET target AFTER READ = " << (*_headersMap)["request_target"]  << std::endl;

    getStatus();
    getDate();
    getServer();
    getConnection();
    getContentType();
    getContentLength(_responseBody);
    getBlankLine();
    getContent(_responseBody);
}

void Request::methodHEAD() {
	getDate();
}

void Request::methodPOST() {
	getDate();
}

void Request::methodPUT() {
	getDate();
}

void Request::methodDELETE() {
	getDate();
}

void Request::methodCONNECT() {
    getStatus();
    getDate();
    getServer();
}

/*
** Надо в зависимости от конфигурации добавлять разрешения
*/
void Request::methodOPTIONS() {
    getStatus();

    getDate();
    getServer();
    getContentType();
}

void Request::methodTRACE() {
    getStatus();
	getDate();
	getServer();
    getConnection();
    getContentType();
    getContentLength(*_headers);
    getBlankLine();
    getContent(*_headers);
}

void Request::getStatus() {
    Client::_headersType& headersMap = _client->getHeadersMap();

    _response.append("HTTP/"+headersMap["http_version"]+" "+\
				_httpStatusCode->getStatusCode()+" "+\
				_data->getMessage(*_httpStatusCode)+"\r\n");
}

void Request::getDate() {
	_response.append(currentTime()+"\r\n");
}

void Request::getServer() {
	_response.append("Server: webserver-ALPHA\r\n");
}

void Request::getContentType(const std::string &filename) {
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

void Request::getContentLength(const std::string &content) {
    static std::ostringstream ss;

    ss << content.size();
    _response.append("Content-Length: " + ss.str() + "\r\n");
    ss.str( std::string() );
    ss.clear();
}

void Request::getConnection() {
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

void Request::getBlankLine() {
    _response.append("\r\n");
}

void Request::getContent(const std::string &content) {
    _response.append(content);
}

void Request::checkRefer() {
	if ((*_headersMap).find("referer") != (*_headersMap).end()) {
		static std::string refPath;
		static std::string::size_type pos;

		pos = (*_headersMap)["referer"].rfind(':');
		if (pos != std::string::npos) {
			pos = (*_headersMap)["referer"].find('/', pos);
			if (pos != std::string::npos) {
				refPath = (*_headersMap)["referer"].substr(pos + 1);
				if (isValidFile(refPath))
					(*_headersMap)["request_target"].insert(0, refPath);
				else
					(*_headersMap)["request_target"].insert(0,_data->getErrorsDirectory());
			}
		}
	}
}


std::string& Request::getResponse(Client::_clientIt &clientIt) {
	static std::string filename;

    setClient((*clientIt));
    _response.clear();

	try {
		checkRefer();
		if (_httpStatusCode && _data->isErrorStatus(_httpStatusCode))
			throw *_httpStatusCode;
        _method = (*_client->getHeadersMap().find("method")).second;
		(this->*_funcMap.find(_method)->second)();
	}
	catch (const HttpStatusCode &httpStatusCode) {
		filename = _data->getErrorPath(httpStatusCode);
		std::cout << "filename = " << filename << std::endl;
		_responseBody = readFile(filename);
		
		getStatus();
		getServer();
		getDate();
		getContentType(filename);
        getContentLength(_responseBody);
        getConnection();
        getBlankLine();
        getContent(_responseBody);
		return (_response);
	}
	return (_response);
}

void Request::setClient(Client *client) {
    _client = client;
    _headers = &client->getHeaders();
    _body = &client->getBody();
    _httpStatusCode = client->getHttpStatusCode();
    _headersMap = &client->getHeadersMap();
}
