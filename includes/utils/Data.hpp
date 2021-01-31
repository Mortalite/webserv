#ifndef DATA_HPP
#define DATA_HPP

#include <iostream>
#include <map>

class Data {

	private:
		std::map<std::string, std::string> _mime_types;

	public:
		Data();
		~Data();
		const std::map<std::string, std::string>& getMimeTypes() const;


};

#endif
