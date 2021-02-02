#include "parser/Request.hpp"

Request::Request() {}

Request::~Request() {}

void Request::parseBody(const std::string& data) {
	std::string delim("\r\n");
	std::vector<std::string> body = ft::split(data, delim);

	_body = &data;
	std::cout << "Parse body" << std::endl;
	for (size_t i = 0; i < body.size(); i++)
		std::cout << "result[" << i << "] = " << body[i] << std::endl << std::flush;

}

void Request::parseHeaders(const std::string &data) {
	std::vector<std::string> headers = ft::split(data, "\r\n");
	std::vector<std::string> requestLine = ft::split(headers[0], " ");

	if (requestLine.size() != 3) {
		/*
		** Ошибка 400
		*/
	}
	else {
		_mapHeaders["method"] = requestLine[0];
		_mapHeaders["request_target"] = requestLine[1];
		_mapHeaders["http_version"] = requestLine[2];
	}

	std::string::size_type ptr;
	std::string field_name, field_value, header_delim = " \t";

	for (size_t i = 1; i < headers.size(); i++) {
		if ((ptr = headers[i].find(":")) != std::string::npos) {
			field_name = headers[i].substr(0, ptr);
			field_value = ft::trim(headers[i].substr(ptr + 1), header_delim);
			_mapHeaders[ft::to_lower(field_name)] = ft::to_lower(field_value);
		}
		else {
			/*
			** Ошибка 400
			*/
		}
	}

	std::cout << "Parse headers" << std::endl;
	int count = 0;
	for (_mapType::iterator i = _mapHeaders.begin(); i != _mapHeaders.end(); i++)
		std::cout << "result[" << count++ << "] = " << "(" << (*i).first << ", " << (*i).second << ")" << std::endl;

}

Data &Request::getData() {
	static Data data;
	return (data);
}

void Request::createResponse() {

}


int Request::isChunked() {
	if (_mapHeaders.find("transfer-encoding") != _mapHeaders.end())
		if (_mapHeaders["transfer-encoding"].find("chunked") != std::string::npos)
			return (1);
	return (0);
}

