#pragma once

#include <iostream>
#include <vector>
#include <map>

struct Base {

	typedef void (Base::*_func)(std::vector<std::string>&);
	typedef std::map<std::string, _func> _baseFuncType;

	Base();
	Base(	std::string root,
	   		std::vector<std::pair<std::string, std::string> > customErrors,
			std::vector<std::string> allowed_method,
			std::vector<std::string> index,
			bool autoindex);
	Base(const Base& other);
	virtual ~Base();

	Base& operator=(const Base& other);

	void initBaseFuncMap();
	void parseRoot(std::vector<std::string> &splitBuffer);
	void parseCustomErrors(std::vector<std::string> &splitBuffer);
	void parseAllowedMethod(std::vector<std::string> &splitBuffer);
	void parseIndex(std::vector<std::string> &splitBuffer);
	void parseAutoindex(std::vector<std::string> &splitBuffer);

	std::string _root;
	std::vector<std::pair<std::string, std::string> > _error_page;
	std::vector<std::string> 	_allowed_method,
								_index;
	bool _autoindex;
	_baseFuncType _baseFuncMap;


};
