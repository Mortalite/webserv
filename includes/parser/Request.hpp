#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <vector>
#include <map>
#include "utils/utils.hpp"
#include "utils/Data.hpp"

class Request {

	private:
		std::map<std::string, std::string> _map_headers;
		typedef std::map<std::string, std::string> _map_type;
	public:
		Request();
		~Request();
		static Data& getData();
		void parse_body(const std::string& data);
		std::map<std::string, std::string>& parse_headers(const std::string& headers);
};

#endif
