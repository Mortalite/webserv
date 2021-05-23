#include "parser/Request.hpp"

Request::Request(const Data* data) {
	_data = data;
	_buffer.reserve(ft::bufferSize);
}

Request::Request(const Request &other): _data(other._data),
										_buffer(other._buffer) {}

Request::~Request() {}

Request &Request::operator=(const Request &other) {
	if (this != &other) {
		_data = other._data;
		_buffer = other._buffer;
	}
	return (*this);
}

void Request::recvHeaders(Client *client) {
	_valread = recv(client->_socket, &_buffer[0], ft::readHeaderSize(client->_hdr), MSG_DONTWAIT);
	if (_valread > 0)
		client->_hdr.append(&_buffer[0], _valread);
	else if (_valread == -1)
		TraceException("recv failed");

	if (ft::isEndWith(client->_hdr, "\r\n\r\n")) {
		parseHeaders(client);
		std::pair<int, long> pairType = getBodyType(client);
		client->_flag = pairType.first;
		client->_recvLeftBytes = pairType.second;
		client->_httpStatusCode = HttpStatusCode("200");
	}
}

void Request::recvBodyPart(Client *client) {
	size_t offset = client->_flag == action::e_recvChunkBody ? 2 : 0;

	if (client->_recvLeftBytes > client->_respLoc->_client_max_body_size + offset) {
		client->_httpStatusCode = HttpStatusCode("413");
		client->_flag = action::e_sendResponse;
	}
	else if (client->_recvLeftBytes) {
		size_t bytesToRead = ft::bufferSize > client->_recvLeftBytes ? client->_recvLeftBytes : ft::bufferSize;
		_valread = recv(client->_socket, &_buffer[0], bytesToRead, MSG_DONTWAIT);
		if (_valread > 0)
			client->_body.append(&_buffer[0], _valread);
		else if (_valread == -1)
			TraceException("recv failed");
		client->_recvBytes += _valread;
		client->_recvLeftBytes -= _valread;
	}
}

void Request::recvContentBody(Client *client) {
	recvBodyPart(client);
	if (!client->_recvLeftBytes)
		client->_flag = action::e_sendResponse;
}

void Request::recvChunkBody(Client *client) {
	if (client->_chunkMod == action::e_recvChunkData) {
		recvBodyPart(client);
		if (!client->_recvLeftBytes) {
			if (client->_prevHexNum == "0")
				client->_flag = action::e_sendResponse;
			client->_body = client->_body.erase(client->_body.rfind("\r\n"));
			client->_chunkMod = action::e_recvChunkHex;
			client->_recvBytes = 0;
		}
	}
	else {
		_valread = recv(client->_socket, &_buffer[0], 1, MSG_DONTWAIT);
		if (_valread > 0)
			client->_hexNum.append(&_buffer[0], _valread);
		else if (_valread == -1)
			TraceException("recv failed");

		if (ft::isEndWith(client->_hexNum, "\r\n")) {
			client->_prevHexNum = client->_hexNum = client->_hexNum.erase(client->_hexNum.rfind("\r\n"));
			client->_recvLeftBytes = 2 + ft::strToLong(client->_hexNum, 16);
			client->_chunkMod = action::e_recvChunkData;
			client->_hexNum.clear();
		}
	}
}

void Request::parseHeaders(Client *client) {
	static std::vector<std::string> hdr;
	static std::vector<std::string> reqLine;
	static std::string::size_type ptr;
	static std::string tmp;
	_servers = &_data->getServers();

	hdr = ft::split(client->_hdr, ft::delimHeaders);
	reqLine = ft::split(hdr[0], " ");
	if (reqLine.size() != 3 || ((ptr = reqLine[2].find("/")) == std::string::npos))
		throw HttpStatusCode("400");
	else {
		client->_hdrMap["method"] = reqLine[0];
		client->_hdrMap["request_target"] = reqLine[1];
		client->_hdrMap["http_version"] = reqLine[2].substr(ptr + 1);
	}

	static std::string fieldName;
	static std::string fieldValue;

	for (size_t i = 1; i < hdr.size(); i++) {
		if ((ptr = hdr[i].find(":")) != std::string::npos) {
			fieldName = ft::tolower(hdr[i].substr(0, ptr));
			fieldValue = ft::trim(hdr[i].substr(ptr + 1), ft::delimConfig);
			client->_hdrMap[fieldName] = fieldValue;
		}
		else if (!hdr[i].empty())
			throw HttpStatusCode("400");
	}
	if (client->_hdrMap["http_version"] == "1.1") {
		if (!client->_hdrMap.count("host"))
			throw HttpStatusCode("400");
	}

	for (Server::_svrsType::const_iterator it = _servers->begin(); it != _servers->end(); it++) {
		if (client->_acptSvr->_listenPort == it->_listenPort) {
			if (!client->_respSvr) {
				client->_respSvr = &*it;
			}
			else {
				if (client->_hdrMap.find("server_name") != client->_hdrMap.end()) {
					if (std::find(it->_serverName.begin(), it->_serverName.end(), client->_hdrMap["server_name"]) !=
						it->_index.end())
						client->_respSvr = &*it;
				}
			}
		}
	}

	static size_t 	maxSize,
					uriSize,
					requestSize;
	static Location::_locsType::const_iterator locationIt;

	maxSize = 0;
	requestSize = client->_hdrMap["request_target"].size();
	for (	locationIt = client->_respSvr->_locations.begin();
			locationIt != client->_respSvr->_locations.end();
			locationIt++) {
		uriSize = locationIt->_uri.size();
		if (requestSize >= uriSize) {
			if (!client->_hdrMap["request_target"].compare(0, uriSize, locationIt->_uri) &&
				uriSize > maxSize) {
				client->_respLoc = &*locationIt;
				maxSize = uriSize;
			}
		}
	}

	tmp = client->_hdrMap["request_target"].substr(client->_respLoc->_uri.size());
	if ((ptr = tmp.find('?')) != std::string::npos) {
		client->_hdrMap["query_string"] = tmp.substr(ptr + 1);
		client->_hdrMap["request_target"].erase(ptr);
	}

	if (!client->_respLoc->_allowed_method.empty() &&
		!ft::isInSet(client->_respLoc->_allowed_method, client->_hdrMap["method"]))
		throw HttpStatusCode("405");
}

std::pair<int, long> Request::getBodyType(Client *client) {
	if (client->_hdrMap.find("transfer-encoding") != client->_hdrMap.end()) {
		if (client->_hdrMap["transfer-encoding"].find("chunked") != std::string::npos)
			return (std::make_pair(action::e_recvChunkBody, 0));
	}
	else if (client->_hdrMap.find("content-length") != client->_hdrMap.end()) {
		long contentLength = 0;

		try {
			contentLength = ft::strToLong(client->_hdrMap["content-length"], 10);
		}
		catch (HttpStatusCode &httpStatusCode) {
			throw HttpStatusCode("400");
		}
		if (contentLength > 0)
			return (std::make_pair(action::e_recvContentBody, contentLength));
		return (std::make_pair(action::e_sendResponse, 0));
	}
	return (std::make_pair(action::e_sendResponse, 0));
}
