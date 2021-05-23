#include "utils/HttpStatusCode.hpp"

HttpStatusCode::HttpStatusCode(const std::string &statusCode): _statusCode(statusCode) {}

HttpStatusCode::HttpStatusCode(const HttpStatusCode &other):_statusCode(other._statusCode) {}

HttpStatusCode::~HttpStatusCode() throw() {}

HttpStatusCode &HttpStatusCode::operator=(const HttpStatusCode &other) {
	if (this != &other) {
		_statusCode = other._statusCode;
	}
	return (*this);
}

const char *HttpStatusCode::what() const throw() {
	return ("HttpStatusCode Exception");
}

const std::string& HttpStatusCode::getStatusCode() const {
	return (_statusCode);
}

void HttpStatusCode::setStatusCode(std::string statusCode) {
	_statusCode = statusCode;
}
