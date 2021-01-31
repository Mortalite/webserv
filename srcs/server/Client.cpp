#include "server/Client.hpp"

Client::Client(int socket, int flag, std::string header, std::string body):		_socket(socket),\
																				_flag(flag),\
																				_header(header),\
																				_body(body),
																				_request(new Request){

}

Client::~Client() {
	delete _request;
}

int Client::getSocket() const {
	return (_socket);
}

int Client::getFlag() const {
	return (_flag);
}

const std::string &Client::getHeader() const {
	return (_header);
}

const std::string &Client::getBody() const {
	return (_body);
}

void Client::setSocket(int socket) {
	_socket = socket;
}

void Client::setFlag(int flag) {
	_flag = flag;
}

void Client::setHeader(const std::string &header) {
	_header = header;
}

void Client::setBody(const std::string &body) {
	_body = body;
}

Request *Client::getRequest() const {
	return (_request);
}

