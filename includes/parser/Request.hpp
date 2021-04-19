#ifndef REQUEST_H
#define REQUEST_H


#include "server/Client.hpp"
#include "utils/HttpStatusCode.hpp"

class Request {

	public:
		Request();
		~Request();
		void recvHeaders(Client::_clientIt &clientIt);
		void recvContentBody(Client::_clientIt &clientIt);
		void recvChunkBody(Client::_clientIt &clientIt);
		void parseHeaders(Client::_clientIt &clientIt);

		bool isAllowedMethod(const std::string &method);
		std::pair<int, long> getBodyType(Client::_clientIt &clientIt);

	private:
		long BODY_BUFFER;
		std::vector<char> _buffer;

};


#endif