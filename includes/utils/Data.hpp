#ifndef DATA_HPP
#define DATA_HPP

#include <iostream>
#include <map>

class Data {

	private:
		class Node {
			private:
				int _type;
				std::string _name;

			public:
				Node(int type, std::string name):	_type(type),\
													_name(name) {};

				int getType() const {
					return (_type);
				}

				const std::string& getName() const {
					return (_name);
				}

				const char* getNameChar() const {
					return (_name.c_str());
				}
		};

		enum CODE_STATUS {
			e_informational,
			e_success,
			e_redirection,
			e_clientError,
			e_serverError
		};

		typedef std::map<std::string, std::string> _mimeType;
		typedef std::map<int, Node*> _httpMapType;

		_mimeType _mimeMap;
		_httpMapType _httpMap;

	public:
		Data();
		~Data();
		const _mimeType& getMimeMap() const;
		const _httpMapType& getHttpMap() const;

};

#endif
