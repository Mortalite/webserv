#include "utils/HttpStatusCode.hpp"

HttpStatusCode::HttpStatusCode(int statusCode) {
	_httpIterator = _httpMap.find(statusCode);
}

HttpStatusCode::~HttpStatusCode() throw() {}

const char *HttpStatusCode::what() const throw() {
	return (getName().c_str());
}

