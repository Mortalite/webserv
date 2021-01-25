#include "parser/Request.hpp"

void Request::parseRequest(const char* s) {

	std::string str(s), delim("\r\n");
	std::vector<std::string> result = split(str, delim);

	for (size_t i = 0; i < result.size(); i++)
		std::cout << "result[" << i << "] = " << result[i]  << std::endl << std::flush;

}
