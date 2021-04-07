#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <string>
#include <exception>

class HttpStatusCode: public std::exception {

	private:
		std::string _statusCode;

	public:
		HttpStatusCode(std::string statusCode);
		~HttpStatusCode() throw();

		const char * what() const throw();

		void setStatusCode(std::string statusCode);
		std::string getStatusCode() const;

		HttpStatusCode& operator=(const HttpStatusCode &other);
};

#endif
