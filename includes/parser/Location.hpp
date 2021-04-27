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

	const std::string &getUri() const;
	void setUri(const std::string &uri);
	const std::string &getRoot() const;
	void setRoot(const std::string &root);
	const std::vector<std::string> &getIndex() const;
	void setIndex(const std::vector<std::string> &index);
	bool isAutoindex() const;
	void setAutoindex(bool autoindex);

	void parseURI(std::vector<std::string> &splitBuffer);
	void parseRoot(std::vector<std::string> &splitBuffer);
	void parseIndex(std::vector<std::string> &splitBuffer);
	void parseAutoindex(std::vector<std::string> &splitBuffer);

	Location& parseLocation(int fd, std::vector<std::string> & splitBuffer);

	Location& operator=(const Location& other);
	friend std::ostream& operator<<(std::ostream& stream, const Location &location) {
		stream << "URI = " << location.getUri() << std::endl;
		stream << "root = " << location.getRoot() << std::endl;
		for (size_t i = 0; i < location.getIndex().size(); i++)
			stream << "index[" << i << "] = " << location.getIndex()[i] << std::endl;
		stream << "autoindex = " << location.isAutoindex() << std::endl;
		return (stream);
	}

private:
	std::string _URI;
	std::string _root;
	std::vector<std::string> _index;
	bool _autoindex;
	_locationFuncType _locationFuncMap;
};

#endif
