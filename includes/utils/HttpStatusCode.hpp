#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <exception>

#include "utils/StatusCodeData.hpp"

class HttpStatusCode: public StatusCodeData, public std::exception {
	private:
		StatusCodeData httpStatusCode;

	public:
		HttpStatusCode(int statusCode);
		~HttpStatusCode() throw();

		const char * what() const throw();

};

#endif
