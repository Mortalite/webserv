#ifndef DATA_HPP
#define DATA_HPP

#include <iostream>
#include <map>

class Data {

	private:
		typedef std::map<std::string, std::string> _mimeType;
		_mimeType _mimeMap;

	public:
		Data();
		~Data();
		const _mimeType& getMimeMap() const;

};

#endif
