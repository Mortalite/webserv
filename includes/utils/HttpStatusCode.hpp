#pragma once

#include <exception>
#include <fstream>

class HttpStatusCode: public std::exception {

public:
	HttpStatusCode(std::string statusCode);
	HttpStatusCode(const HttpStatusCode& other);
	~HttpStatusCode() throw();

	HttpStatusCode& operator=(const HttpStatusCode &other);
	friend std::ostream& operator<<(std::ostream &stream, const HttpStatusCode& httpStatusCode) {
		stream << "_statusCode = " << httpStatusCode._statusCode << std::endl;
		return (stream);
	}

	const char * what() const throw();

	void setStatusCode(std::string statusCode);
	const std::string& getStatusCode() const;

private:
	std::string _statusCode;
};
