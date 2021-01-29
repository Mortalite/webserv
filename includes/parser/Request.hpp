#ifndef WEBSERV_REQUEST_HPP
#define WEBSERV_REQUEST_HPP

#include <iostream>
#include <vector>
#include <map>
#include "utils/utils.hpp"

class Request {

	private:
		std::map<std::string, std::string> requestMap;
		std::map<std::string, std::string> mime_types;

	public:
		Request();
		~Request();
		void parseRequest(const std::string& data);
};

#endif
