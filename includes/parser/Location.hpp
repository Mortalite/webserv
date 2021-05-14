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
	virtual std::ostream& print(std::ostream &out) const;

	void parseURI(std::vector<std::string> &splitBuffer);
	Location& parseLocation(int fd, std::vector<std::string> & splitBuffer);

	std::string _uri;
	_locFuncType _locFuncMap;
};
