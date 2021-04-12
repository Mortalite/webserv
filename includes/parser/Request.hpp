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
		std::pair<int, long> getBodyType();

		void parseHeaders(const std::string &input);
		void parseBody(const std::string &input);
		int isKeepAlive();
		bool isAllowedMethod(const std::string &method);
		void methodGET();
		void methodHEAD();
		void methodPOST();
		void methodPUT();
		void methodDELETE();
		void methodCONNECT();
		void methodOPTIONS();
		void methodTRACE();
		void getStatus();
		void getDate();
		void getServer();
		void getContentType(std::string &filename);
		std::string getResponse();

	private:
		const Data* _data;
		const HttpStatusCode* _httpStatusCode;
		std::string _headers;
		std::string _body;
		std::string _method;
		std::string _response;
		_headersType _headersMap;
		_funcType _funcMap;
		std::vector<char> _timeBuffer;
};

#endif
