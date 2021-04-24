#ifndef HTTPSTATUSCODE_HPP
#define HTTPSTATUSCODE_HPP

#include <string>
#include <exception>

class HttpStatusCode: public std::exception {

public:
	HttpStatusCode(std::string statusCode);
	HttpStatusCode(const HttpStatusCode& other);
	~HttpStatusCode() throw();

	HttpStatusCode& operator=(const HttpStatusCode &other);

	const char * what() const throw();

	void setStatusCode(std::string statusCode);
	const std::string& getStatusCode() const;

private:
	std::string _statusCode;
};

#endif
