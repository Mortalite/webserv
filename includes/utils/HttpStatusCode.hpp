#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <exception>
#include "utils/Data.hpp"

class HttpStatusCode: public std::exception {

	private:
		int _statusCode;

	public:
		HttpStatusCode(int statusCode);
		~HttpStatusCode() throw();

		const char * what() const throw();

		void setStatusCode(int statusCode);
		int getStatusCode() const;

};

#endif
