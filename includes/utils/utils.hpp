#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <algorithm>
#include <arpa/inet.h>

in_port_t ft_htons(in_port_t port);
std::string ft_trim(const std::string& input);
std::vector<std::string> ft_split(const std::string& input, const std::string& delim);
std::string& ft_str_to_lower(std::string& input);

#endif
