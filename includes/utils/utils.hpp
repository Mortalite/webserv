#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <arpa/inet.h>

in_port_t ft_htons(in_port_t port);
std::vector<std::string> split(const std::string& input, const std::string& delim);

#endif
