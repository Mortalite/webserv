#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "utils/utils.hpp"

struct Base {

	typedef void (Base::*_func)(std::vector<std::string>&);
	typedef std::map<std::string, _func> _baseFuncType;

	Base();
	Base(	std::string root,
		 	std::string auth_basic,
		 	std::string auth_basic_user_file,
	   		std::vector<std::pair<std::string, std::string> > customErrors,
			std::vector<std::string> allowed_method,
			std::vector<std::string> index,
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
	void parseCustomErrors(std::vector<std::string> &splitBuffer);
	void parseAllowedMethod(std::vector<std::string> &splitBuffer);
	void parseIndex(std::vector<std::string> &splitBuffer);
	void parseAutoindex(std::vector<std::string> &splitBuffer);

	std::string _root,
				_auth_basic,
				_auth_basic_user_file;
	std::vector<std::pair<std::string, std::string> > _error_page;
	std::vector<std::string> 	_allowed_method,
								_index;
	bool _autoindex;
	_baseFuncType _baseFuncMap;


};
