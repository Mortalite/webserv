#pragma once

#include <string>
#include <vector>
#include <map>
#include "parser/Base.hpp"
#include "utils/utils.hpp"

class Location:public Base {
public:
	typedef void (Location::*_func)(std::vector<std::string>&);
	typedef std::map<std::string, _func> _locationFuncType;

	Location();
	Location(const Location& other);
	~Location();

	Location& operator=(const Location& other);
	friend std::ostream& operator<<(std::ostream& stream, const Location &location) {
		stream << "URI = " << location._uri << std::endl;
		stream << "root = " << location._root << std::endl;
		for (size_t i = 0; i < location._index.size(); i++)
			stream << "index[" << i << "] = " << location._index[i] << std::endl;
		stream << "autoindex = " << location._autoindex << std::endl;
		return (stream);
	}

	void parseURI(std::vector<std::string> &splitBuffer);
	Location& parseLocation(int fd, std::vector<std::string> & splitBuffer);

	std::string _uri;
	_locationFuncType _locationFuncMap;
};
