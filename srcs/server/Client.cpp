#include "server/Client.hpp"

Client::Client(const Server *acceptServer, int socket, int flag): _acceptServer(acceptServer),
																  _responseServer(NULL),
																  _responseLocation(NULL),
																  _socket(socket),
																  _flag(flag),
																  _chunkMod(e_recvChunkHex),
																  _size(0),
																  _httpStatusCode(HttpStatusCode("200")) {
	_headersMap["http_version"] = "1.1";
}

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

/*
** Проверка разрывать или нет соединение
*/
bool Client::isKeepAlive() {
	if (_headersMap.count("connection")) {
		if (_headersMap["connection"].find("close") != std::string::npos)
			return (false);
	}
	if (_headersMap["http_version"] == "1.1" || _headersMap["http_version"] == "2.0")
		return (true);
	if (_headersMap["http_version"] == "1.0") {
		if (_headersMap.find("connection") != _headersMap.end()) {
			if (_headersMap["connection"].find("keep-alive") != std::string::npos)
				return (true);
		}
	}
	return (false);
}


void Client::responseSent() {
	if (isKeepAlive())
		_flag = e_recvHeaders;
	else
		_flag = e_closeConnection;

	_headers.clear();
	_headersMap.clear();
	_body.clear();
	_hexNum.clear();
}
