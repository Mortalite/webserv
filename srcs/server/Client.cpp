#include "server/Client.hpp"

Client::Client(int socket, int flag): 	_socket(socket),\
                                        _flag(flag),\
                                        _chunkMod(e_recvChunkHex),\
                                        _size(0),\
                                        _httpStatusCode(HttpStatusCode("200")) {}

Client::Client(const Client &other):_socket(other._socket),
                                    _flag(other._flag),
                                    _chunkMod(other._chunkMod),
                                    _size(other._size),
                                    _headers(other._headers),
                                    _headersMap(other._headersMap),
                                    _body(other._body),
                                    _hexNum(other._hexNum),
                                    _httpStatusCode(other._httpStatusCode) {}

Client::~Client() {}

Client &Client::operator=(const Client &other) {
    if (this != &other) {
        _socket = other._socket;
        _flag = other._flag;
        _chunkMod = other._chunkMod;
        _size = other._size;
        _headers = other._headers;
        _headersMap = other._headersMap;
        _body = other._body;
        _hexNum = other._hexNum;
        _httpStatusCode = other._httpStatusCode;
    }
    return (*this);
}

int Client::getSocket() const {
	return (_socket);
}

int Client::getFlag() const {
	return (_flag);
}

const std::string &Client::getHeaders() const {
	return (_headers);
}

Client::_headersType &Client::getHeadersMap() {
    return (_headersMap);
}

const std::string &Client::getBody() const {
	return (_body);
}

const HttpStatusCode& Client::getHttpStatusCode() const {
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

void Client::setHeaders(const std::string &header) {
    _headers = header;
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

void Client::setHttpStatusCode(const HttpStatusCode &httpStatusCode) {
	_httpStatusCode = httpStatusCode;
}

void Client::appendHeader(std::string str) {
	_headers.append(str);
}

void Client::appendBody(std::string str) {
	_body.append(str);
}

void Client::appendHexNum(std::string str) {
	_hexNum.append(str);
}

void Client::wipeData() {
    _headers.clear();
    _headersMap.clear();
    _body.clear();
    _hexNum.clear();
}

