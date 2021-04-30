#include "server/Client.hpp"

Client::Client(const Server *server, int socket, int flag):	_server(server),
														_socket(socket),
														_flag(flag),
														_chunkMod(e_recvChunkHex),
														_size(0),
														_httpStatusCode(HttpStatusCode("200")) {}

Client::Client(const Client &other):_server(other._server),
									_socket(other._socket),
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
		_server = other._server;
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

void Client::wipeData() {
	_headers.clear();
	_headersMap.clear();
	_body.clear();
	_hexNum.clear();
}

