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

	private:
		typedef std::map<std::string, std::string> _headersType;
		typedef void (Request::*_func)();
		typedef std::map<std::string, _func> _funcType;

	public:
		Request(const Data* data, const HttpStatusCode* httpStatusCode);
		~Request();

		size_t isLearYear(int year);
		size_t yearSize(int year);
		std::string getDate();
		std::pair<int, long> getBodyType();

		void parseHeaders(const std::string& headers);
		void parseBody(const std::string& data);
		int isKeepAlive();
		bool isAllowedMethod(std::string& method);
		void methodGET();
		void methodHEAD();
		void methodPOST();
		void methodPUT();
		void methodDELETE();
		void methodCONNECT();
		void methodOPTIONS();
		void methodTRACE();
		std::string getStatus();
	std::string getServer();
	std::string getContentType();


	std::string getResponse();

	private:
		const Data* _data;
		const HttpStatusCode* _httpStatusCode;
		_headersType _headersMap;
		_funcType _funcMap;
		std::vector<std::string> _body;
		std::vector<char> _timeBuffer;
};

#endif
