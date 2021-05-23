#pragma once

#include <iostream>
#include <list>
#include <parser/Server.hpp>
#include <utils/HttpStatusCode.hpp>
#include "utils/utils.hpp"

struct Client {
	typedef std::map<std::string, std::string> _headersMapType;
	typedef std::list<Client*> _clientsType;
	typedef _clientsType::iterator _clientIt;

	Client(const Server *acceptServer, int socket, int flag);
	Client(const Client& other);
	~Client();

	Client& operator=(const Client& other);

	friend std::ostream& operator<<(std::ostream &stream, const Client& client) {
		std::cout << BLUE_B << "CLIENT" << RESET << std::endl;
		if (client._acptSvr)
			stream << *client._acptSvr << std::endl;
		if (client._respSvr)
			stream << *client._respSvr << std::endl;
		if (client._respLoc)
			stream << *client._respLoc << std::endl;
		stream << "_socket = " << client._socket << std::endl;
		stream << "_flag = " << client._flag << std::endl;
		stream << "_chunkMod = " << client._chunkMod << std::endl;
		stream << "_size = " << client._size << std::endl;
		stream << "_hdr = " << client._hdr << std::endl;
		debug::printContainerMap(stream, "_headerMap", client._hdrMap);
		stream << "_body = " << client._body << std::endl;
		stream << "_hexNum = " << client._hexNum << std::endl;
		stream << "_httpStatusCode = " << client._httpStatusCode << std::endl;
		return (stream);
	}

	bool isKeepAlive();
	bool isTimeout();
	std::string getHdrOrDflt(std::string header, std::string defaultHeader) const;
	void clear(bool isError);

	const Server 	*_acptSvr,
					*_respSvr;
	const Location *_respLoc;
	sockaddr_in		_listenAddr,
					_acptAddr;
	HttpStatusCode _httpStatusCode;
	struct timeval _lastActionTime;
	_headersMapType _hdrMap;
	int 	_socket,
			_flag,
			_chunkMod,
			_size;
	long	_valread;
	size_t	_recvLeftBytes,
			_recvBytes,
			_sendLeftBytes,
			_sendBytes;
	std::string _hdr,
				_body,
				_prevHexNum,
				_hexNum,
				_resp,
				_cntntLang;
};
