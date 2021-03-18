#include "server/Client.hpp"

Client::Client(Data* data, int socket, int flag, std::string header, std::string body): _socket(socket),\
																						_flag(flag),\
																						_chunkMod(1),\
																						_size(0),\
																						_header(header),\
																						_body(body),\
																						_data(data),\
																						_httpStatusCode(new HttpStatusCode("200")),\
																						_request(new Request(_data, _httpStatusCode)) {

}

Client::~Client() {
	delete _request;
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

HttpStatusCode *Client::getHttpStatusCode() const {
	return (_httpStatusCode);
}

int Client::getChunkMod() const {
	return (_chunkMod);
}

long Client::getSize() const {
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

void Client::setSize(long size) {
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

bool Client::isReadFlag() const {
	return (_flag == ft::e_recvHeaders ||\
			_flag == ft::e_recvContentBody ||\
			_flag == ft::e_recvChunkBody ||\
			_flag == ft::e_closeConnection);
}

