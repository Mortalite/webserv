#pragma once

#include <iostream>

#include "../parser/Response.hpp"
#include "../utils/Data.hpp"

class Cgi{
private:
	char **_env_var;
public:
	int make_env_var(Response & resp);

};