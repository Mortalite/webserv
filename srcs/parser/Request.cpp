#include "parser/Request.hpp"

Request::Request(const Data* data, const HttpStatusCode* httpStatusCode):	_data(data),\
																			_httpStatusCode(httpStatusCode) {
	_timeBuffer.reserve(100);
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

	struct tm tm;
	struct timeval tv;

	gettimeofday(&tv, NULL);

	int year = epoch_year;
	time_t time = tv.tv_sec;
	size_t dayclock =  time % secs_day;
	size_t dayno = time / secs_day;

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
	strftime(&_timeBuffer[0], 100, "%a, %d %b %Y %H:%M:%S GMT", &tm);
	std::cout << "strftime = " << &_timeBuffer[0] << std::endl;
	return (&_timeBuffer[0]);
}


void Request::parseBody(const std::string& data) {
	std::string delim("\r\n");
	_body = ft::split(data, delim);

	std::cout << "Parse body" << std::endl;
	for (size_t i = 0; i < _body.size(); i++)
		std::cout << "result[" << i << "] = " << _body[i] << std::endl << std::flush;

}

void Request::parseHeaders(const std::string &data) {
	std::vector<std::string> headers = ft::split(data, "\r\n");
	std::vector<std::string> requestLine = ft::split(headers[0], " ");
	std::string::size_type ptr;

	if (requestLine.size() != 3 || ((ptr = requestLine[2].find("/")) == std::string::npos))
		throw HttpStatusCode("400");
	else {
		_mapHeaders["method"] = requestLine[0];
		_mapHeaders["request_target"] = requestLine[1];
		_mapHeaders["http_version"] = requestLine[2].substr(ptr + 1);
	}

	std::string field_name, field_value, header_delim = " \t";

	for (size_t i = 1; i < headers.size(); i++) {
		if ((ptr = headers[i].find(":")) != std::string::npos) {
			field_name = headers[i].substr(0, ptr);
			field_value = ft::trim(headers[i].substr(ptr + 1), header_delim);
			_mapHeaders[ft::toLower(field_name)] = ft::toLower(field_value);
		}
		else if (!headers[i].empty())
			throw HttpStatusCode("400");
	}

	std::cout << "Parse headers" << std::endl;
	int count = 0;
	for (_mapType::iterator i = _mapHeaders.begin(); i != _mapHeaders.end(); i++)
		std::cout << "result[" << count++ << "] = " << "(" << (*i).first << ", " << (*i).second << ")" << std::endl;

}

std::pair<int, long> Request::getBodyType() {
	if (_mapHeaders.find("transfer-encoding") != _mapHeaders.end()) {
		if (_mapHeaders["transfer-encoding"].find("chunked") != std::string::npos)
			return (std::make_pair(ft::e_recvChunkBody, 0));
	}
	else if (_mapHeaders.find("content-length") != _mapHeaders.end()) {
		char *ptr;
		long content_length = strtol(_mapHeaders["content-length"].c_str(), &ptr, 10);
		if (!(*ptr))
			return (std::make_pair(ft::e_recvContentBody, content_length));
	}
	throw HttpStatusCode("400");
}

std::string Request::sendResponse() {
	std::string response;

	response += _mapHeaders["http_version"] + " " + _httpStatusCode->getStatusCode() + " " + _data->getMessage(_httpStatusCode) + "\n";
	response += getDate();

	return (response);
}

