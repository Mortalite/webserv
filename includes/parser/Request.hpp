#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <vector>
#include <map>
#include "utils/utils.hpp"
#include "utils/HttpStatusCode.hpp"

class Request {

	public:
		typedef std::map<std::string, std::string> _mapType;

		Request();
		~Request();

		void parseHeaders(const std::string& headers);
		void parseBody(const std::string& data);
		std::pair<int, long> getBodyType();

	private:
		_mapType _mapHeaders;
		std::vector<std::string> _body;

};

#endif
