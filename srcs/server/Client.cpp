#include "server/Client.hpp"

Client::Client(const Server *acceptServer, int socket, int flag): _acceptServer(acceptServer),
																  _responseServer(NULL),
																  _responseLocation(NULL),
																  _socket(socket),
																  _flag(flag),
																  _chunkMod(e_recvChunkHex),
																  _size(0),
																  _httpStatusCode(HttpStatusCode("200")) {}

Client::Client(const Client &other): _acceptServer(other._acceptServer),
									 _responseServer(other._responseServer),
									 _responseLocation(other._responseLocation),
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
		_acceptServer = other._acceptServer;
		_responseServer = other._responseServer;
		_responseLocation = other._responseLocation;
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

