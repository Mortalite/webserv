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
			_mapHeaders[ft::toLower(field_name)] = ft::toLower(field_value);
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

std::pair<int, long> Request::getBodyType() {
	if (_mapHeaders.find("transfer-encoding") != _mapHeaders.end()) {
		if (_mapHeaders["transfer-encoding"].find("chunked") != std::string::npos)
			return (std::make_pair(ft::e_chunked_body, 0));
	}
	else if (_mapHeaders.find("content-length") != _mapHeaders.end()) {
		char *ptr;
		long content_length = strtol(_mapHeaders["content-length"].c_str(), &ptr, 10);
		if (!(*ptr))
			return (std::make_pair(ft::e_content_body, content_length));
	}
	return (std::make_pair(ft::e_flags_default, 0));
}

