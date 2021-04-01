#include "parser/Request.hpp"

Request::Request(const Data* data, const HttpStatusCode* httpStatusCode):	_data(data),\
																			_httpStatusCode(httpStatusCode) {
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

std::string Request::getDate() {
	static int year0 = 1900;
	static int epoch_year = 1970;
	static size_t secs_day = 24*60*60;
	static size_t _ytab[2][12] =
			{
					{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
					{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
			};

	static struct tm tm;
	static struct timeval tv;

	gettimeofday(&tv, NULL);

	static int year;
	static time_t time;
	static size_t dayclock;
	static size_t dayno;

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
	strftime(&_timeBuffer[0], 100, "Date: %a, %d %b %Y %H:%M:%S GMT", &tm);
//	DEBUG
//	std::cout << "strftime = " << &_timeBuffer[0] << std::endl;
	return (&_timeBuffer[0]);
}


void Request::parseBody(const std::string& data) {
	std::string delim("\r\n");
	_body = ft::split(data, delim);

//	DEBUG
	std::cout << "Parse body" << std::endl;
	for (size_t i = 0; i < _body.size(); i++)
		std::cout << "result[" << i << "] = " << _body[i] << std::endl << std::flush;

}

void Request::parseHeaders(const std::string &data) {
	std::vector<std::string> headers = ft::split(data, "\r\n");
	std::vector<std::string> requestLine = ft::split(headers[0], " ");
	static std::string::size_type ptr;

	if (requestLine.size() != 3 ||\
	((ptr = requestLine[2].find("/")) == std::string::npos) ||\
	!isAllowedMethod(requestLine[0]))
		throw HttpStatusCode("400");
	else {
		_headersMap["method"] = requestLine[0];
		_headersMap["request_target"] = requestLine[1];
		_headersMap["http_version"] = requestLine[2].substr(ptr + 1);
	}

	static std::string field_name;
	static std::string field_value;
	static std::string header_delim = " \t";
	for (size_t i = 1; i < headers.size(); i++) {
		if ((ptr = headers[i].find(":")) != std::string::npos) {
			field_name = headers[i].substr(0, ptr);
			field_value = ft::trim(headers[i].substr(ptr + 1), header_delim);
			_headersMap[ft::toLower(field_name)] = ft::toLower(field_value);
		}
		else if (!headers[i].empty())
			throw HttpStatusCode("400");
	}

//	DEBUG
	std::cout << "Parse headers" << std::endl;
	int count = 0;
	for (_headersType::iterator i = _headersMap.begin(); i != _headersMap.end(); i++)
		std::cout << "result[" << count++ << "] = " << "(" << (*i).first << ", " << (*i).second << ")" << std::endl;
}

std::pair<int, long> Request::getBodyType() {
	if (_headersMap.find("transfer-encoding") != _headersMap.end()) {
		if (_headersMap["transfer-encoding"].find("chunked") != std::string::npos)
			return (std::make_pair(ft::e_recvChunkBody, 0));
	}
	else if (_headersMap.find("content-length") != _headersMap.end()) {
		char *ptr;
		long content_length;

		content_length = strtol(_headersMap["content-length"].c_str(), &ptr, 10);
		if (!(*ptr))
			return (std::make_pair(ft::e_recvContentBody, content_length));
	}
	throw HttpStatusCode("400");
}

int Request::isKeepAlive() {
	if (_headersMap.find("connection") != _headersMap.end()) {
		if (_headersMap["connection"].find("close") != std::string::npos)
			return (ft::e_closeConnection);
	}
	else if (_headersMap["http_version"] == "1.1" || _headersMap["http_version"] == "2.0")
		return (ft::e_recvHeaders);
	else if (_headersMap["http_version"] == "1.0") {
		if (_headersMap.find("connection") != _headersMap.end()) {
			if (_headersMap["connection"].find("keep-alive") != std::string::npos)
				return (ft::e_recvHeaders);
		}
	}
	std::cout << "ALERT" << std::endl;
	return (ft::e_closeConnection);
}

bool Request::isAllowedMethod(std::string& method) {
	static std::string allowedMethods[8] = { "GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE" };
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

}

void Request::methodHEAD() {

}

void Request::methodPOST() {

}

void Request::methodPUT() {

}

void Request::methodDELETE() {

}

void Request::methodCONNECT() {

}

void Request::methodOPTIONS() {

}

void Request::methodTRACE() {

}

std::string Request::getStatus() {
	return ("HTTP/"+_headersMap["http_version"]+" "+_httpStatusCode->getStatusCode()+" "+_data->getMessage(_httpStatusCode));
}

std::string Request::getServer() {
	return ("Server: webserver-ALPHA");
}

std::string Request::getContentType() {
	return ("Content-Type: text/html");
}

std::string Request::getResponse() {
	static std::string method;

	method = (*_headersMap.find("method")).second;
	if (_data->isErrorStatus(_httpStatusCode)) {
		std::string response;

		response += getStatus();
		std::cout << "$" << _httpStatusCode->getStatusCode() << "$" << std::endl;

	}
	else {
		try {
			(this->*_funcMap.find(method)->second)();
		}
		catch (HttpStatusCode &httpStatusCode) {
			std::cout << "$" << httpStatusCode.getStatusCode() << "$" << std::endl;

		}
	}

	std::string response;
	response = "";

	response += getStatus() + "\r\n";
	response += getDate() + "\r\n";
	response += "\r\n";
	std::cout << "respone:\n" << response << std::endl;
	return (response);
}
