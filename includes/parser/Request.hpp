#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <vector>
#include <map>
#include "utils/utils.hpp"
#include "utils/Data.hpp"

class Request {

	private:
		std::map<std::string, std::string> _mapHeaders;
		typedef std::map<std::string, std::string> _mapType;
		const std::string* _body;

	public:
		Request();
		~Request();
		static Data& getData();
		void parseBody(const std::string& data);
		void parseHeaders(const std::string& headers);
		void createResponse();
		int isChunked();
};

#endif
