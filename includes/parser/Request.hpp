#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <sys/time.h>
#include "utils/Data.hpp"
#include "utils/HttpStatusCode.hpp"
#include "utils/utils.hpp"
#include "server/Client.hpp"

class Request {

	private:
		typedef void (Request::*_func)();
		typedef std::map<std::string, _func> _funcType;

	public:
		Request(const Data* data);
		~Request();

		size_t isLearYear(int year);
		size_t yearSize(int year);
		std::pair<int, long> getBodyType(Client::_clientIt &clientIt);

		void parseHeaders(Client::_clientIt &clientIt);
		int isKeepAlive(Client::_clientIt &clientIt);
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
		void getContentType(const std::string &filename = "Default");
        void getContentLength(const std::string &content);
        void getConnection();
        void getBlankLine();
        void getContent(const std::string &content);
		std::string& getResponse(Client::_clientIt &clientIt);
        void setClient(Client *client);

	private:
		const Data *_data;
        Client *_client;
		const HttpStatusCode *_httpStatusCode;
		const std::string *_headers;
        const std::string *_body;
        Client::_headersType *_headersMap;
		std::string _method;
		std::string _response;
        std::string _responseBody;
		_funcType _funcMap;
		std::vector<char> _timeBuffer;
};

#endif
