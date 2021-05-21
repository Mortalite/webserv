#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "utils/utils.hpp"

struct Base {

	typedef void (Base::*_func)(std::vector<std::string>&);
	typedef std::map<std::string, _func> _baseFuncType;

	Base();
	Base(const std::string &root,
		 const std::string &auth_basic,
		 const std::string &auth_basic_user_file,
		 const std::string &cgi_index,
		 const std::string &cgi_path,
		 const std::vector<std::pair<std::string, std::string> > &error_page,
		 const std::vector<std::string> &allowed_method,
		 const std::vector<std::string> &index,
		 const std::vector<std::string> &cgi_extension,
		 long clientMaxBodySize,
		 bool autoindex);
	Base(const Base& other);
	virtual ~Base();

	Base& operator=(const Base& other);
	friend std::ostream& operator<<(std::ostream& stream, const Base& base) {
		base.print(stream);
		return (stream);
	}
	virtual std::ostream& print(std::ostream &out) const;

	void initBaseFuncMap();
	void parseRoot(std::vector<std::string> &splitBuffer);
	void parseAuthBasic(std::vector<std::string> &splitBuffer);
	void parseAuthBasicUserFile(std::vector<std::string> &splitBuffer);
	void parseCgiIndex(std::vector<std::string> &splitBuffer);
	void parseCgiPath(std::vector<std::string> &splitBuffer);
	void parseErrorPage(std::vector<std::string> &splitBuffer);
	void parseAllowedMethod(std::vector<std::string> &splitBuffer);
	void parseIndex(std::vector<std::string> &splitBuffer);
	void parseCgiExtension(std::vector<std::string> &splitBuffer);
	void parseClientMaxBodySize(std::vector<std::string> &splitBuffer);
	void parseAutoindex(std::vector<std::string> &splitBuffer);

	std::string _root,
				_auth_basic,
				_auth_basic_user_file,
				_cgi_index,
				_cgi_path;
	std::vector<std::pair<std::string, std::string> > _error_page;
	std::vector<std::string> 	_allowed_method,
								_index,
								_cgi_extension;
	long _client_max_body_size;
	bool _autoindex;
	_baseFuncType _baseFuncMap;


};
