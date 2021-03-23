#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <vector>
#include <map>
#include <sys/time.h>
#include "utils/Data.hpp"
#include "utils/HttpStatusCode.hpp"
#include "utils/utils.hpp"

class Request {

	public:
		typedef std::map<std::string, std::string> _mapType;

		Request(const Data* data, const HttpStatusCode* httpStatusCode);
		~Request();

		size_t isLearYear(int year);
		size_t yearSize(int year);
		std::string getDate();

		void parseHeaders(const std::string& headers);
		void parseBody(const std::string& data);
		std::pair<int, long> getBodyType();
		std::string sendResponse();
		int keepAlive();
		bool isAllowedMethod(std::string& method);

	private:
		const Data* _data;
		const HttpStatusCode* _httpStatusCode;
		_mapType _mapHeaders;
		std::vector<std::string> _body;
		std::vector<char> _timeBuffer;
};

#endif
