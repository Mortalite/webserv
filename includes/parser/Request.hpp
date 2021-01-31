#ifndef WEBSERV_REQUEST_HPP
#define WEBSERV_REQUEST_HPP

#include <iostream>
#include <vector>
#include <map>
#include "utils/utils.hpp"

class Request {

	private:
		std::map<std::string, std::string> headers_map;
		std::map<std::string, std::string> mime_types;

	public:
		Request();
		~Request();
		void parse_body(const std::string& data);
		std::map<std::string, std::string>& parse_headers(const std::string& headers);
};

#endif
