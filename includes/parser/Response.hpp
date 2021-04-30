#ifndef RESPONSE_HPP
#define RESPONSE_HPP

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
	Response(const Data *data);
	Response(const Response &other);
	~Response();

	Response& operator=(const Response &other);

	void sendResponse(Client *client);

	friend std::ostream& operator<<(std::ostream& stream, const Response &response) {
		if (getDebug() == 1) {
			stream << WHITE_B << "Response" << RESET << std::endl;
			stream << BLUE_B << BLUE << "headers:" << RESET << std::endl;
			stream << response._client->_headers << std::endl;
			if (response._client->_body.size() < 300) {
				stream << BLUE_B << BLUE << "body:" << RESET << std::endl;
				stream << response._client->_body << std::endl;
			}
			else {
				stream << BLUE_B << BLUE << "body:" << RESET << std::endl;
				stream << response._client->_body.substr(0, 300) << RESET << std::endl;
			}
			if (response._response.size() < 2000) {
				stream << BLUE_B << BLUE << "response:" << RESET << std::endl;
				stream << response._response << std::endl;
			}
			else {
				stream << BLUE_B << BLUE << "response:" << RESET << std::endl;
				stream << response._response.substr(0, 2000) << std::endl;
			}
		}
		return (stream);
	}

private:
	int isKeepAlive();
	bool isValidFile(std::string& fileName);
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
	void getContentType();
	void getContentLength();
	void getConnection();
	void getBlankLine();
	void getContent(const std::string &content);
	void getReferer();
	void getLastModified();
	void getErrorPage();
	void getResponse();

	typedef void (Response::*_func)();
	typedef std::map<std::string, _func> _funcType;

	const Data *_data;
	Client *_client;
	struct stat _fileStat;
	std::string _method;
	std::string _response;
	std::string _responseBody;
	_funcType _funcMap;
};

#endif
