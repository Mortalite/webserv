#include "server/Client.hpp"

Client::Client(Data* data, int socket, int flag, std::string header, std::string body): _socket(socket),\
																						_flag(flag),\
																						_chunkMod(1),\
																						_size(0),\
																						_header(header),\
																						_body(body),\
																						_data(data),\
																						_request(new Request()),\
																						_response(new Response()),\
																						_httpStatusCode(new HttpStatusCode(200)){

}

Client::~Client() {
	delete _request;
	delete _response;
	delete _httpStatusCode;
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

Data *Client::getData() const {
	return _data;
}

Request *Client::getRequest() const {
	return (_request);
}

Response *Client::getResponse() const {
	return (_response);
}

HttpStatusCode *Client::getHttpStatusCode() const {
	return (_httpStatusCode);
}

int Client::getChunkMod() const {
	return (_chunkMod);
}

size_t Client::getSize() const {
	return _size;
}

const std::string &Client::getHexNum() const {
	return (_hexNum);
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

void Client::setChunkMod(int count) {
	_chunkMod = count;
}

void Client::setHexNum(const std::string &hexNum) {
	_hexNum = hexNum;
}

void Client::setSize(size_t size) {
	_size = size;
}

void Client::appendHeader(std::string str) {
	_header.append(str);
}

void Client::appendBody(std::string str) {
	_body.append(str);
}

void Client::appendHexNum(std::string str) {
	_hexNum.append(str);
}

void Client::parseHeaders() {
	_request->parseHeaders(_header);
}

void Client::parseBody() {
	_request->parseBody(_body);
}

