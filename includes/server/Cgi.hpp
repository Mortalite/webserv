#pragma once

#include <iostream>
#include <map>

#include "parser/Response.hpp"
#include "utils/Data.hpp"

class Response;

class Cgi{
private:
	char **_env_var;
public:
	int make_env_var(Response & resp);
	void start_cgi(Response & resp);
	std::string make_query_string(const Client * client);

};