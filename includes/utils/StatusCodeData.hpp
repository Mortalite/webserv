#ifndef STATUSCODEDATA_HPP
#define STATUSCODEDATA_HPP

#include <map>
#include <string>

class StatusCodeData {

private:
	class Node {
		private:
			int _type;
			std::string _name;

		public:
			Node(int type, std::string name):_type(type), _name(name) {};

			int getType() const {
				return (_type);
			}

			const std::string& getName() const {
				return (_name);
			}
	};

public:
	typedef std::map<int, Node*> _httpMapType;

	enum CODE_STATUS {
		e_informational,
		e_success,
		e_redirection,
		e_clientError,
		e_serverError
	};

	StatusCodeData();
	virtual ~StatusCodeData() throw();

	int getType() const;
	const std::string& getName() const;

protected:
	_httpMapType _httpMap;
	_httpMapType::iterator _httpIterator;

};


#endif
