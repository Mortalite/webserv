#include "parser/Request.hpp"

Request::Request() {}

Request::~Request() {}

void Request::parse_body(const std::string& data) {
	std::string delim("\r\n");
	std::vector<std::string> body = ft_split(data, delim);
	
	std::cout << "Parse body" << std::endl;

	for (size_t i = 0; i < body.size(); i++)
		std::cout << "result[" << i << "] = " << body[i] << std::endl << std::flush;

}

std::map<std::string, std::string>& Request::parse_headers(const std::string &data) {
	std::vector<std::string> headers = ft_split(data, "\r\n");
	std::vector<std::string> request_line = ft_split(headers[0], " ");

	if (request_line.size() != 3) {
		/*
		** Ошибка 400
		*/
	}
	else {
		_map_headers["method"] = request_line[0];
		_map_headers["request_target"] = request_line[1];
		_map_headers["http_version"] = request_line[2];
	}

	std::string::size_type ptr;
	std::string field_name, field_value;

	for (size_t i = 1; i < headers.size(); i++) {
		if ((ptr = headers[i].find(":")) != std::string::npos) {
			field_name = headers[i].substr(0, ptr);
			field_value = headers[i].substr(ptr + 1);
			_map_headers[ft_str_to_lower(field_name)] = ft_trim(field_value);
		}
		else {
			/*
			** Ошибка 400
			*/
		}
	}

	int count = 0;
	for (_map_type::iterator i = _map_headers.begin(); i != _map_headers.end(); i++)
		std::cout << "result[" << count++ << "] = " << "(" << (*i).first << ", " << (*i).second << ")" << std::endl;

	return (_map_headers);
}

Data &Request::getData() {
	static Data data;
	return (data);
}
