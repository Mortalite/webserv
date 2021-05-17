#pragma once

#include <iostream>
#include <map>

#include "parser/Response.hpp"
#include "utils/Data.hpp"

class Response;

class Cgi {
public:
	Cgi();
	Cgi(const Cgi& other);
	~Cgi();

	Cgi& operator=(const Cgi &other);

	size_t size(char **array);
	void copy(char **array);
	void del(char **array);


	void makeEnvVar();
	void findTarget();
	void startCgi(Response *resp);


private:
	Response *_resp;
	const Data *_data;
	const Client *_client;
	const Server *_server;
	struct TargetInfo *_tgInfo;
	std::map<std::string, std::string> _envMap;
	char **_envVar;

};