#include "server/Client.hpp"

Client::Client(const Server *acceptServer, int socket, int flag): _acptSvr(acceptServer),
																  _respSvr(NULL),
																  _respLoc(NULL),
																  _socket(socket),
																  _flag(flag),
																  _chunkMod(e_recvChunkHex),
																  _size(0),
																  _recvLeftBytes(0),
																  _recvBytes(0),
																  _sendLeftBytes(0),
																  _sendBytes(0),
																  _httpStatusCode(HttpStatusCode("200")) {
	_hdrMap["http_version"] = "1.1";
}

Client::Client(const Client &other): _acptSvr(other._acptSvr),
									 _respSvr(other._respSvr),
									 _respLoc(other._respLoc),
									 _socket(other._socket),
									 _flag(other._flag),
									 _chunkMod(other._chunkMod),
									 _size(other._size),
									 _hdr(other._hdr),
									 _hdrMap(other._hdrMap),
									 _body(other._body),
									 _hexNum(other._hexNum),
									 _recvLeftBytes(other._recvLeftBytes),
									 _recvBytes(other._recvBytes),
									 _sendLeftBytes(other._sendLeftBytes),
									 _sendBytes(other._sendBytes),
									 _httpStatusCode(other._httpStatusCode) {}

Client::~Client() {}

Client &Client::operator=(const Client &other) {
	if (this != &other) {
		_acptSvr = other._acptSvr;
		_respSvr = other._respSvr;
		_respLoc = other._respLoc;
		_socket = other._socket;
		_flag = other._flag;
		_chunkMod = other._chunkMod;
		_size = other._size;
		_hdr = other._hdr;
		_hdrMap = other._hdrMap;
		_body = other._body;
		_hexNum = other._hexNum;
		_recvLeftBytes = other._recvLeftBytes;
		_recvBytes = other._recvBytes;
		_sendLeftBytes = other._sendLeftBytes;
		_sendBytes = other._sendBytes;
		_httpStatusCode = other._httpStatusCode;
	}
	return (*this);
}

/*
** Проверка разрывать или нет соединение
*/
bool Client::isKeepAlive() {
	if (_hdrMap.find("connection") != _hdrMap.end()) {
		if (_hdrMap["connection"].find("close") != std::string::npos)
			return (false);
	}
	if (_hdrMap["http_version"] == "1.1" || _hdrMap["http_version"] == "2.0")
		return (true);
	if (_hdrMap["http_version"] == "1.0") {
		if (_hdrMap.find("connection") != _hdrMap.end())
			if (_hdrMap["connection"].find("keep-alive") != std::string::npos)
				return (true);
	}
	return (false);
}

void Client::recvBodyPart() {
	if (_recvLeftBytes > _acptSvr->_clientMaxBodySize) {
		_httpStatusCode = HttpStatusCode("413");
		_flag = e_sendResponse;
	}
	else if (_recvLeftBytes) {
		_body.reserve(_recvLeftBytes+2);
		_valread = recv(_socket, &_body[_recvBytes], _recvLeftBytes, MSG_DONTWAIT);
		if (_valread == -1)
			throw std::runtime_error("recv failed");
		_recvBytes += _valread;
		_recvLeftBytes -= _valread;
	}
}

void Client::sendPart() {
	if (_sendLeftBytes) {
		_resp.reserve(_sendLeftBytes);
		_valread = send(_socket, &_resp[_sendBytes], _sendLeftBytes, MSG_DONTWAIT);
		if (_valread == -1)
			throw std::runtime_error("send failed");
		_sendBytes += _valread;
		_sendLeftBytes -= _valread;
	}
}

void Client::responseSent() {
	if (!_sendLeftBytes) {
		_flag = isKeepAlive() ? e_recvHeaders : e_closeConnection;
		_sendBytes = 0;
		_resp.clear();
	}
	_hdr.clear();
	_hdrMap.clear();
	_body.clear();
	_hexNum.clear();
	_cntntLang.clear();
}
