#include "utils/HttpStatusCode.hpp"

HttpStatusCode::HttpStatusCode(int statusCode):	_statusCode(statusCode) {}

HttpStatusCode::~HttpStatusCode() throw() {}

const char *HttpStatusCode::what() const throw() {
	return ("HttpStatusCode Exception");
}

int HttpStatusCode::getStatusCode() const {
	return (_statusCode);
}

void HttpStatusCode::setStatusCode(int statusCode) {
	_statusCode = statusCode;
}



