#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>
#include <map>
#include "utils/utils.hpp"

class Location {
public:
	typedef void (Location::*_func)(std::vector<std::string>&);
	typedef std::map<std::string, _func> _locationFuncType;
	Location();
	Location(const Location& other);
	~Location();

	void parseURI(std::vector<std::string> &splitBuffer);
	void parseRoot(std::vector<std::string> &splitBuffer);
	void parseIndex(std::vector<std::string> &splitBuffer);
	void parseAutoindex(std::vector<std::string> &splitBuffer);

	Location& parseLocation(int fd, std::vector<std::string> & splitBuffer);

	Location& operator=(const Location& other);
	friend std::ostream& operator<<(std::ostream& stream, const Location &location) {
		stream << "URI = " << location._URI << std::endl;
		stream << "root = " << location._root << std::endl;
		for (size_t i = 0; i < location._index.size(); i++)
			stream << "index[" << i << "] = " << location._index[i] << std::endl;
		stream << "autoindex = " << location._autoindex << std::endl;
		return (stream);
	}

	std::string _URI;
	std::string _root;
	std::vector<std::string> _index;
	bool _autoindex;
	_locationFuncType _locationFuncMap;
};

#endif
