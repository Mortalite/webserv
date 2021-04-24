#ifndef DATA_HPP
#define DATA_HPP

#include <iostream>
#include <map>
#include <cstdlib>
#include <fcntl.h>
#include "utils/utils.hpp"
#include "utils/HttpStatusCode.hpp"

class Data {

private:
	class Node {
	private:
		int _type;
		std::string _name;
		std::string _path;

	public:
		Node(int type, const std::string &name):_type(type),
												_name(name) {};

		Node(int type, const std::string &name, const std::string& path):	_type(type),
																			_name(name),
																			_path(path) {};

		int getType() const {
			return (_type);
		}

		const std::string& getName() const {
			return (_name);
		}

		const char* getNameChar() const {
			return (_name.c_str());
		}

		const std::string& getPath() const {
			return (_path);
		}

		void setPath(const std::string &path) {
			_path = path;
		}
	};

	enum CODE_STATUS {
		e_informational,
		e_success,
		e_redirection,
		e_clientError,
		e_serverError
	};

public:
	typedef std::map<std::string, std::string> _mimeMapType;
	typedef std::map<std::string, Node*> _httpMapType;
	typedef _mimeMapType::const_iterator _mimeMapIt;
	typedef _httpMapType::iterator _httpMapIt;

	Data();
	Data(const Data& other);
	~Data();

	Data& operator=(const Data& other);

	const _mimeMapType& getMimeMap() const;
	const _httpMapType& getHttpMap() const;
	const std::string& getErrorsDirectory() const;

	std::string getMessage(const HttpStatusCode &httpStatusCode) const;
	std::string getErrorPath(const HttpStatusCode &httpStatusCode) const;
	bool isErrorStatus(const HttpStatusCode *httpStatusCode) const;
	bool isErrorStatus(const _httpMapIt &httpMapIt) const;

	void parseMimeTypes(const std::string& mimeTypes = "./config/mime.types");

private:
	_mimeMapType _mimeMap;
	_httpMapType _httpMap;
	std::string _errorsDirectory;
};

#endif
