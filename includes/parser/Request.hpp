#ifndef WEBSERV_REQUEST_HPP
#define WEBSERV_REQUEST_HPP

#include <iostream>
#include <vector>
#include <map>
#include "utils/utils.hpp"

class Request {

	private:
		std::map<std::string, std::string> requestMap;

	public:
		void parseRequest(const std::string& data);
};

#endif
