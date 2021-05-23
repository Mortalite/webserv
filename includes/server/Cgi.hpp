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
	void startCgi(Client *client, struct ft::TargetInfo *tgInfo);

private:
	void clear();
	size_t size(char **array);
	char *copy(char *array);
	char **copy(char **array);
	void freeArray(char *&array);
	void freeArray(char **&array);

	void convertEnvVar();
	void makeEnvVar();
	void findTarget();
	void cgiExecve();
	void parseCgiResp();

	Client *_client;
	struct ft::TargetInfo *_tgInfo;
	std::map<std::string, std::string> _envMap;
	char 	*_cgi_path,
			*_path;
	char **_envVar;
	std::string _cgiResp;
};
