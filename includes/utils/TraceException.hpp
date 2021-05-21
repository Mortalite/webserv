#pragma once

#include <iostream>
#include <sstream>
#include <exception>
#include <string.h>
#include <cerrno>

class TraceException: public std::runtime_error {
	std::string msg;

public:
	TraceException(const std::string &arg, const char *file, int line): std::runtime_error(arg) {
		std::ostringstream o;
		o << file << ":" << line << ": " << arg << strerror(errno);
		msg = o.str();
	}

	~TraceException() throw() {}

	const char *what() const throw() {
		return (msg.c_str());
	}
};

#define TraceException(arg) throw TraceException(arg, __FILE__, __LINE__)