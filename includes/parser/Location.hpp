#pragma once

#include <string>
#include <vector>
#include <list>
#include <map>
#include "parser/Base.hpp"
#include "utils/utils.hpp"

struct Location:public Base {
	typedef void (Location::*_func)(std::vector<std::string>&);
	typedef std::map<std::string, _func> _locFuncType;
	typedef std::list<Location> _locsType;
	typedef _locsType::iterator _locsIt;

	Location();
	Location(const Location& other);
	~Location();

	Location& operator=(const Location& other);
	friend std::ostream& operator<<(std::ostream& stream, const Location &location) {
		stream << "URI = " << location._uri << std::endl;
		stream << "root = " << location._root << std::endl;
		for (size_t i = 0; i < location._allowed_method.size(); i++)
			stream << "allowed_method[" << i << "] = " << location._allowed_method[i] << std::endl;
		for (size_t i = 0; i < location._index.size(); i++)
			stream << "index[" << i << "] = " << location._index[i] << std::endl;
		stream << "autoindex = " << location._autoindex << std::endl;
		return (stream);
	}

	void parseURI(std::vector<std::string> &splitBuffer);
	Location& parseLocation(int fd, std::vector<std::string> & splitBuffer);

	std::string _uri;
	_locFuncType _locFuncMap;
};
