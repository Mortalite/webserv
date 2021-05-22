#pragma once

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <vector>
#include <map>
#include <dirent.h>
#include "server/Cgi.hpp"
#include "server/Client.hpp"
#include "utils/Base64.hpp"
#include "utils/Data.hpp"
#include "utils/HttpStatusCode.hpp"
#include "utils/utils.hpp"

class Response {
public:
	typedef void (Response::*_func)();
	typedef std::map<std::string, _func> _funcType;

	Response(Data *data, Cgi *cgi);
	Response(const Response &other);
	~Response();

	Response& operator=(const Response &other);
	friend std::ostream& operator<<(std::ostream& stream, const Response &response) {
		if (ft::getDebug() == 1) {
			stream << WHITE_B << "Response" << RESET << std::endl;
			stream << BLUE_B << BLUE << "headers:" << RESET << std::endl;
			stream << "socket = " << response._client->_socket << std::endl;
			stream << response._client->_hdr << std::endl;
			if (response._client->_body.size() < 300) {
				stream << BLUE_B << BLUE << "body:" << RESET << std::endl;
				stream << response._client->_body << std::endl;
			}
			else {
				stream << BLUE_B << BLUE << "body:" << RESET << std::endl;
				stream << response._client->_body.substr(0, 300) << RESET << std::endl;
			}
			if (response._client->_resp.size() < 300) {
				stream << BLUE_B << BLUE << "response:" << RESET << std::endl;
				stream << response._client->_resp << std::endl;
			}
			else {
				stream << BLUE_B << BLUE << "response:" << RESET << std::endl;
				stream << response._client->_resp.substr(0, 300) << std::endl;
			}
			std::cout << WHITE_B << "END" << RESET << std::endl;
		}
		return (stream);
	}

	void initErrorFile(const HttpStatusCode &httpStatusCode);
	void sendResponse(Client *client);

private:
	void methodGET();
	void methodHEAD();
	void methodPOST();
	void methodPUT();
	void methodCONNECT();
	void methodOPTIONS();
	void methodTRACE();

	void getStatus();
	void getDate();
	void getServer();
	void getContentLanguage();
	void getContentLength();
	void getContentLocation();
	void getContentType();
	void getLocation();
	void getConnection();
	void getBlankLine();
	void getContent();
	void getLastModified();
	void getRetryAfter();
	void getAllow();
	void getAuthenticate();

	void initAutoIndex();

	void findTarget(const std::string &filepath);
	void constructResp();
	void constructAutoIndex();
	void authorization();
	void sendPart();

	Data *_data;
	Client *_client;
	Cgi *_cgi;
	struct TargetInfo _tgInfo;
	std::string _method;

private:
	_funcType _funcMap;
};
