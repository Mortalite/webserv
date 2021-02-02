#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <algorithm>
#include <arpa/inet.h>

namespace ft {

	in_port_t htons(in_port_t port);
	std::string trim(const std::string &input, const std::string &delim);
	std::vector<std::string> split(const std::string &input, const std::string &delim);
	std::string& to_lower(std::string &input);
	int is_last_equal(const std::string &str, const std::string &extension);

}

#endif
