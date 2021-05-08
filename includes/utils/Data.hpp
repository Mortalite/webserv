#pragma once

#include <iostream>
#include <map>
#include "parser/Server.hpp"
#include "server/Client.hpp"
#include "utils/HttpStatusCode.hpp"
#include "utils/utils.hpp"

class Data {

private:
	class Node {
	private:
		int _type;
		std::string _name,
					_path;

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
	std::string getMessage(const HttpStatusCode &httpStatusCode) const;
	std::string getErrorPath(const Client *client) const;
	std::string getErrorPath(const HttpStatusCode *httpStatusCode) const;


	const Server::_svrsType &getServers() const;
	bool isErrorStatus(const HttpStatusCode *httpStatusCode) const;
	bool isErrorStatus(const _httpMapIt &httpMapIt) const;

	void parseMimeTypes(const std::string &mimeTypes = mimeTypesConfig);
	void parseConfiguration(const std::string &configuration = webserverConfig);

private:
	std::string _buffer;
	std::vector<std::string> _splitBuffer;
	int _fd;

	_mimeMapType _mimeMap;
	_httpMapType _httpMap;
	Server::_svrsType _servers;
};
