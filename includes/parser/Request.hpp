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
		typedef void (Request::*_func)(std::string&);
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
		void methodGET(std::string &response);
		void methodHEAD(std::string &response);
		void methodPOST(std::string &response);
		void methodPUT(std::string &response);
		void methodDELETE(std::string &response);
		void methodCONNECT(std::string &response);
		void methodOPTIONS(std::string &response);
		void methodTRACE(std::string &response);
		void getStatus(std::string &response);
		void getDate(std::string &response);
		void getServer(std::string &response);
		void getContentType(std::string &response, std::string &filename);
		std::string getResponse();

	private:
		const Data* _data;
		const HttpStatusCode* _httpStatusCode;
		std::string _headers;
		std::string _body;
		_headersType _headersMap;
		_funcType _funcMap;
		std::vector<char> _timeBuffer;
};

#endif
