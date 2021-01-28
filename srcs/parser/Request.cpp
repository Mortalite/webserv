#include "parser/Request.hpp"

void Request::parseRequest(const std::string& data) {

	std::string delim("\r\n");
	std::vector<std::string> result = split(data, delim);

	for (size_t i = 0; i < result.size(); i++)
		std::cout << "result[" << i << "] = " << result[i]  << std::endl << std::flush;

}
