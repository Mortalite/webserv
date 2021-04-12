#include "parser/Request.hpp"

Request::Request(const Data* data, const HttpStatusCode* httpStatusCode):	_data(data),\
																			_httpStatusCode(httpStatusCode),\
																			_headers(NULL),\
																			_body(NULL) {
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

size_t Request::isLearYear(int year) {
	return (!(year % 4) && (year % 100 || !(year % 400)));
}

size_t Request::yearSize(int year) {
	if (isLearYear(year))
		return (366);
	return (365);
}

void Request::getDate() {
	static int year0 = 1900;
	static int epoch_year = 1970;
	static size_t secs_day = 24*60*60;
	static size_t _ytab[2][12] =
			{
					{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
					{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
			};
	static int year;
	static time_t time;
	static size_t dayclock;
	static size_t dayno;
	static struct tm tm;
	static struct timeval tv;

	gettimeofday(&tv, NULL);

	year = epoch_year;
	time = tv.tv_sec;
	dayclock = time % secs_day;
	dayno = time / secs_day;

	tm.tm_sec = dayclock % 60;
	tm.tm_min = (dayclock % 3600) / 60;
	tm.tm_hour = dayclock / 3600;
	tm.tm_wday = (dayno + 4) % 7;
	while (dayno >= yearSize(year))
	{
		dayno -= yearSize(year);
		year++;
	}
	tm.tm_year = year - year0;
	tm.tm_yday = dayno;
	tm.tm_mon = 0;
	while (dayno >= _ytab[isLearYear(year)][tm.tm_mon])
	{
		dayno -= _ytab[isLearYear(year)][tm.tm_mon];
		tm.tm_mon++;
	}
	tm.tm_mday = dayno + 1;
	tm.tm_isdst = 0;
	strftime(&_timeBuffer[0], 100, "Date: %a, %d %b %Y %H:%M:%S GMT\r\n", &tm);
	_response += &_timeBuffer[0];
//	DEBUG
//	std::cout << "strftime = " << &_timeBuffer[0] << std::endl;
}

void Request::parseHeaders(const std::string &input) {
	static std::vector<std::string> headers;
	static std::vector<std::string> requestLine;
	static std::string::size_type ptr;

	_headers = &input;
	headers = split(input, "\r\n");
	requestLine = split(headers[0], " ");
	if (requestLine.size() != 3 ||\
	((ptr = requestLine[2].find("/")) == std::string::npos) ||\
	!isAllowedMethod(requestLine[0])) {
        _headersMap["http_version"] = "1.1";
        throw HttpStatusCode("400");
    }
	else {
		_headersMap["method"] = requestLine[0];
		_headersMap["request_target"] = requestLine[1];
		_headersMap["http_version"] = requestLine[2].substr(ptr + 1);
	}

	static std::string field_name;
	static std::string field_value;
	static std::string header_delim( " \t");

	for (size_t i = 1; i < headers.size(); i++) {
		if ((ptr = headers[i].find(":")) != std::string::npos) {
			field_name = headers[i].substr(0, ptr);
			field_value = trim(headers[i].substr(ptr + 1), header_delim);
			_headersMap[toLower(field_name)] = toLower(field_value);
		}
		else if (!headers[i].empty())
			throw HttpStatusCode("400");
	}

//	DEBUG
/*	std::cout << "Parse headers" << std::endl;
	int count = 0;
	for (_headersType::iterator i = _headersMap.begin(); i != _headersMap.end(); i++)
		std::cout << "result[" << count++ << "] = " << "(" << (*i).first << ", " << (*i).second << ")" << std::endl;*/
}

void Request::parseBody(const std::string &input) {
	static std::vector<std::string> body;
	static std::string delim("\r\n");

//	body = split(input, delim);

	_body = &input;
//	std::cout << "body.size() = " << input.size() << ", " << _body->size() << std::endl;
//	DEBUG
/*	std::cout << "Parse body" << std::endl;
	for (size_t i = 0; i < body.size(); i++)
		std::cout << "result[" << i << "] = " << body[i] << std::endl << std::flush;*/

}

std::pair<int, long> Request::getBodyType() {
	if (_headersMap.find("transfer-encoding") != _headersMap.end()) {
		if (_headersMap["transfer-encoding"].find("chunked") != std::string::npos)
			return (std::make_pair(e_recvChunkBody, 0));
	}
	else if (_headersMap.find("content-length") != _headersMap.end()) {
		char *ptr;
		long content_length;

		content_length = strtol(_headersMap["content-length"].c_str(), &ptr, 10);
		if (!(*ptr))
			return (std::make_pair(e_recvContentBody, content_length));
	}
	return (std::make_pair(e_sendResponse, 0));
}

int Request::isKeepAlive() {
	if (_headersMap.find("connection") != _headersMap.end()) {
		if (_headersMap["connection"].find("close") != std::string::npos)
			return (e_closeConnection);
	}
	else if (_headersMap["http_version"] == "1.1" || _headersMap["http_version"] == "2.0")
		return (e_recvHeaders);
	else if (_headersMap["http_version"] == "1.0") {
		if (_headersMap.find("connection") != _headersMap.end()) {
			if (_headersMap["connection"].find("keep-alive") != std::string::npos)
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
	getDate();
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
	getDate();
}

void Request::methodOPTIONS() {
	getDate();
}

void Request::methodTRACE() {
    getStatus();
	getDate();
	getServer();
    getConnection();
    getContentType();
    getContentLength();
    getBlankLine();
    getBody();
}

void Request::getStatus() {
	_response.append("HTTP/"+_headersMap["http_version"]+" "+\
				_httpStatusCode->getStatusCode()+" "+\
				_data->getMessage(*_httpStatusCode)+"\r\n");
}

void Request::getServer() {
	_response.append("Server: webserver-ALPHA\r\n");
}

void Request::getContentType(const std::string &filename) {
    if (_method == "TRACE") {
        _response.append("Content-Type: text/html\r\n");
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

void Request::getContentLength() {
    if (_body) {
        static std::ostringstream ss;

        ss << _body->size();
        _response.append("Content-Length: " + ss.str() + "\r\n");
        ss.str( std::string() );
        ss.clear();
    }
}

void Request::getConnection() {
    if (_headersMap.find("connection") != _headersMap.end()) {
        if (_headersMap["connection"].find("close") != std::string::npos)
            _response.append("Connection: close\r\n");
    }
    else if (_headersMap["http_version"] == "1.1" || _headersMap["http_version"] == "2.0")
        _response.append("Connection: keep-alive\r\n");
    else if (_headersMap["http_version"] == "1.0") {
        if (_headersMap.find("connection") != _headersMap.end()) {
            if (_headersMap["connection"].find("keep-alive") != std::string::npos)
                _response.append("Connection: keep-alive\r\n");
        }
    }
    else
        _response.append("Connection: close\r\n");
}


void Request::getBlankLine() {
    _response.append("\r\n");
}

void Request::getBody() {
    if (_body)
        _response.append(*_body);
}

std::string Request::getResponse() {
	static std::string filename;

    _response.clear();
    try {
		if (_data->isErrorStatus(_httpStatusCode))
			throw *_httpStatusCode;
//		std::cout << "RESPONSE CODE: " << _data->getMessage(*_httpStatusCode) << std::endl;
        _method = (*_headersMap.find("method")).second;
        (this->*_funcMap.find(_method)->second)();
	}
	catch (const HttpStatusCode &httpStatusCode) {
		filename = _data->getErrorPath(httpStatusCode);
//		std::cout << "RESPONSE CODE: " << _data->getMessage(httpStatusCode) << std::endl;
		getStatus();
		getServer();
		getDate();
		getContentType(filename);

		return (_response);
	}

	std::cout << "RESPONSE:\n" << _response << std::endl;
	return (_response);
}

