#pragma once

#include <iostream>
#include <list>
#include <parser/Server.hpp>
#include <utils/HttpStatusCode.hpp>
#include "utils/utils.hpp"

struct Client {
	typedef std::map<std::string, std::string> _headersType;
	typedef std::list<Client*> _clientsType;
	typedef _clientsType::iterator _clientIt;
	typedef std::list<std::pair<std::string, std::string> > _refererType;
	typedef _refererType::iterator _refererIt;

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
		printContainerMap(stream, "_headerMap", client._hdrMap);
		stream << "_body = " << client._body << std::endl;
		stream << "_hexNum = " << client._hexNum << std::endl;
		stream << "_httpStatusCode = " << client._httpStatusCode << std::endl;
		return (stream);
	}

	bool isKeepAlive();
	void responseSent();

	const Server 	*_acptSvr,
			*_respSvr;
	const Location *_respLoc;
	HttpStatusCode _httpStatusCode;
	_headersType _hdrMap;
	int 	_socket,
			_flag,
			_chunkMod,
			_size;
	std::string _hdr,
				_body,
				_hexNum,
				_resp,
				_cntntLang;
	size_t	_recvLeftBytes,
			_recvBytes,
			_sendLeftBytes,
			_sendBytes;
};
