#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <vector>
#include <map>
#include "utils/utils.hpp"

class Request {

	public:
		typedef std::map<std::string, std::string> _mapType;

		Request();
		~Request();

		void parseBody(const std::string& data);
		void parseHeaders(const std::string& headers);
		std::pair<int, long> getBodyType();

	private:
		_mapType _mapHeaders;
		const std::string* _body;

};

#endif
