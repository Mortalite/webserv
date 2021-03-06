
#include <utils/StatusCodeData.hpp>

StatusCodeData::StatusCodeData() {
	_httpMap[200] = new Node(e_success, "OK");
	_httpMap[201] = new Node(e_success, "Created");
	_httpMap[404] = new Node(e_clientError, "Not Found");
}

StatusCodeData::~StatusCodeData() throw() {
	for (_httpMapType::iterator it = _httpMap.begin(); it != _httpMap.end();) {
		delete (*it).second;
		_httpMap.erase(it++);
	}
}

int StatusCodeData::getType() const {
	return ((*_httpIterator).first);
}

const std::string &StatusCodeData::getName() const {
	return ((*_httpIterator).second->getName());
}

