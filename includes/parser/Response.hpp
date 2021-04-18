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

class Response {

	public:
		Response(const Data* data);
		~Response();

		static int& getDebug();
		void printDebugInfo();
		void sendResponse(Client::_clientIt &clientIt);

	private:
		typedef void (Response::*_func)();
		typedef std::map<std::string, _func> _funcType;

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

		void setClient(Client *client);
		int isKeepAlive(Client::_clientIt &clientIt);
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
		void getReferer();
		std::string& getResponse(Client::_clientIt &clientIt);
};

#endif
