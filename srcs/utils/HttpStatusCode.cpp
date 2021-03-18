#include "utils/HttpStatusCode.hpp"

HttpStatusCode::HttpStatusCode(std::string statusCode):	_statusCode(statusCode) {}

HttpStatusCode::~HttpStatusCode() throw() {}

const char *HttpStatusCode::what() const throw() {
	return ("HttpStatusCode Exception");
}

std::string HttpStatusCode::getStatusCode() const {
	return (_statusCode);
}

void HttpStatusCode::setStatusCode(std::string statusCode) {
	_statusCode = statusCode;
}



