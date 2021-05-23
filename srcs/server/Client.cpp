#include "server/Client.hpp"

Client::Client(const Server *acceptServer, int socket, int flag): _acptSvr(acceptServer),
																  _respSvr(NULL),
																  _respLoc(NULL),
																  _httpStatusCode(HttpStatusCode("200")),
																  _socket(socket),
																  _flag(flag),
																  _chunkMod(action::e_recvChunkHex),
																  _size(0),
																  _valread(0),
																  _recvLeftBytes(0),
																  _recvBytes(0),
																  _sendLeftBytes(0),
																  _sendBytes(0) {
	if (gettimeofday(&_lastActionTime, NULL) == -1)
		TraceException("gettimeofday failed");
	memset(&_listenAddr, '\0', sizeof(sockaddr_in));
	memset(&_acptAddr, '\0', sizeof(sockaddr_in));
	_hdrMap["http_version"] = "1.1";
}

Client::Client(const Client &other): _acptSvr(other._acptSvr),
									 _respSvr(other._respSvr),
									 _respLoc(other._respLoc),
									 _listenAddr(other._listenAddr),
									 _acptAddr(other._acptAddr),
									 _httpStatusCode(other._httpStatusCode),
									 _lastActionTime(other._lastActionTime),
									 _hdrMap(other._hdrMap),
									 _socket(other._socket),
									 _flag(other._flag),
									 _chunkMod(other._chunkMod),
									 _size(other._size),
									 _valread(other._valread),
									 _recvLeftBytes(other._recvLeftBytes),
									 _recvBytes(other._recvBytes),
									 _sendLeftBytes(other._sendLeftBytes),
									 _sendBytes(other._sendBytes),
									 _hdr(other._hdr),
									 _body(other._body),
									 _prevHexNum(other._prevHexNum),
									 _hexNum(other._hexNum),
									 _resp(other._resp),
									 _cntntLang(other._cntntLang) {}

Client::~Client() {}

Client &Client::operator=(const Client &other) {
	if (this != &other) {
		_acptSvr = other._acptSvr;
		_respSvr = other._respSvr;
		_respLoc = other._respLoc;
		_listenAddr = other._listenAddr;
		_acptAddr = other._acptAddr;
		_httpStatusCode = other._httpStatusCode;
		_lastActionTime = other._lastActionTime;
		_hdrMap = other._hdrMap;
		_socket = other._socket;
		_flag = other._flag;
		_chunkMod = other._chunkMod;
		_size = other._size;
		_valread = other._valread;
		_recvLeftBytes = other._recvLeftBytes;
		_recvBytes = other._recvBytes;
		_sendLeftBytes = other._sendLeftBytes;
		_sendBytes = other._sendBytes;
		_hdr = other._hdr;
		_body = other._body;
		_prevHexNum = other._prevHexNum;
		_hexNum = other._hexNum,
		_resp = other._resp,
		_cntntLang = other._cntntLang;
	}
	return (*this);
}

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

bool Client::isTimeout() {
	static struct timeval now;
	static double diff;

	if (gettimeofday(&now, NULL) == -1)
		TraceException("gettimeofday failed");

	diff = static_cast<double>(now.tv_sec - _lastActionTime.tv_sec) +
			1e-6*static_cast<double>(now.tv_usec - _lastActionTime.tv_usec);
	return (diff > static_cast<double>(ft::defaultClientTimeout));
}

void Client::clear(bool isError) {
	if (!_sendLeftBytes) {
		_flag = !isError && isKeepAlive() ? action::e_recvHeaders : action::e_closeConnection;
		_sendBytes = 0;
		_resp.clear();
		_hdrMap.clear();
	}
	_hdr.clear();
	_body.clear();
	_prevHexNum.clear();
	_hexNum.clear();
	_cntntLang.clear();
	if (gettimeofday(&_lastActionTime, NULL) == -1)
		TraceException("gettimeofday failed");
}

std::string Client::getHdrOrDflt(std::string header, std::string defaultHeader) const {
	_headersMapType::const_iterator it = _hdrMap.find(header);
	if (it != _hdrMap.end())
		return (it->second);
	return (defaultHeader);
}
