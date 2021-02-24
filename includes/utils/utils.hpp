#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <algorithm>
#include <arpa/inet.h>

namespace ft {

	/*
	** Флаги чтения запроса
	*/
	enum FLAGS {
		e_headers,
		e_content_body,
		e_chunked_body,
		e_multipart_body,
		e_flags_default
	};

	enum CHUNKED {
		e_chunk_data,
		e_chunk_hex,
		e_chunked_default
	};

	in_port_t htons(in_port_t port);
	int inSet(const char &character, const std::string &delim);
	std::string trim(const std::string &input, const std::string &delim);
	std::vector<std::string> split(const std::string &input, const std::string &delim);
	std::string& toLower(std::string &input);
	int isLastEqual(const std::string &str, const std::string &extension);

}

#endif
