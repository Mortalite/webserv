#pragma once

#include <iostream>
#include <map>
#include <cassert>
#include <sys/wait.h>

#include "utils/Base64.hpp"
#include "utils/Data.hpp"

class Cgi {
public:
	typedef std::map<std::string, std::string> envMapType;
	Cgi();
	Cgi(const Cgi& other);
	~Cgi();

	Cgi& operator=(const Cgi &other);

	size_t size(char **array);
	char ** copy(char **array);
	void del(char **&array);

	void convertEnvVar();
	void makeEnvVar();
	void findTarget();
	void cgiExecve();
	void parseCgiResp();
	void startCgi(Client *client, struct TargetInfo *tgInfo);

private:
	Client *_client;
	struct TargetInfo *_tgInfo;
	std::map<std::string, std::string> _envMap;
	char **_envVar;
	std::string _cgiResp;
};
