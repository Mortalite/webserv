#include "parser/Request.hpp"

/*
** Создаю буффер для получения запросов.
*/
Request::Request(const Data* data) {
	_data = data;
	_buffer.reserve(bodyBufferSize);
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

/*
** Читаю заголовки по 1 символу либо по 4, если последний символ не '\r' или 'n',
** если это конец заголовков "\r\n\r\n", то обрабатываю их, если что-то не так,
** то бросается\ловится исключение, и сохраняется значение кода ошибки
*/
void Request::recvHeaders(Client *client) {
	_valread = recv(client->_socket, &_buffer[0], ft::readHeaderSize(client->_hdr), MSG_DONTWAIT);
	if (_valread > 0)
		client->_hdr.append(&_buffer[0], _valread);
	else if (_valread == -1)
		throw std::runtime_error("Request::recvHeaders() recv error");

	if (ft::isEndWith(client->_hdr, "\r\n\r\n")) {
		parseHeaders(client);
		std::pair<int, long> pairType = getBodyType(client);
		client->_flag = pairType.first;
		client->_recvLeftBytes = pairType.second;
		client->_httpStatusCode = HttpStatusCode("200");
	}
}

void Request::recvBodyPart(Client *client) {
	if (client->_recvLeftBytes > client->_acptSvr->_clientMaxBodySize) {
		client->_httpStatusCode = HttpStatusCode("413");
		client->_flag = e_sendResponse;
	}
	else if (client->_recvLeftBytes) {
		_buffer.reserve(client->_recvLeftBytes);
		_valread = recv(client->_socket, &_buffer[client->_recvBytes], client->_recvLeftBytes, MSG_DONTWAIT);
		if (_valread > 0) {
			if (client->_flag == e_recvChunkBody)
				_valread -= 2;
			client->_body.append(&_buffer[0], _valread);
		}
		else if (_valread == -1)
			throw std::runtime_error("Request::recvHeaders() recv error");

		client->_recvBytes += _valread;
		client->_recvLeftBytes -= _valread;
	}
}

/*
** Читаю тело с заголовком content-length
*/
void Request::recvContentBody(Client *client) {
	recvBodyPart(client);
	if (!client->_recvLeftBytes)
		client->_flag = e_sendResponse;
}

/*
** Читаю тело с заголовком transfer-encoding = chunk, сначала
** получаю число в 16-ой системе счисления - это размер следующей
** части запроса, потом читаю этот запрос и так до бесконечности, пока
** размер запроса != 0.
*/
void Request::recvChunkBody(Client *client) {
	if (client->_chunkMod == e_recvChunkData) {
		recvBodyPart(client);
		if (!client->_recvLeftBytes) {
			client->_chunkMod = e_recvChunkHex;
			client->_recvBytes = 0;
		}
	}
	else {
		_valread = recv(client->_socket, &_buffer[0], 1, MSG_DONTWAIT);
		if (_valread > 0)
			client->_hexNum.append(&_buffer[0], _valread);
		else if (_valread == -1)
			throw std::runtime_error("Request::recvChunkBody() recv error");

		if (ft::isEndWith(client->_hexNum, "\r\n")) {
			if (client->_hexNum == "0") {
				client->_flag = e_sendResponse;
			}
			else {
				client->_recvLeftBytes = ft::strToLong(client->_hexNum)+2;
				client->_chunkMod = e_recvChunkData;
				client->_hexNum.clear();
			}
		}
	}
}

/*
** Записываю все заголовки в ассоциативный массив, проверяю на правильность
*/
void Request::parseHeaders(Client *client) {
	static std::vector<std::string> hdr;
	static std::vector<std::string> reqLine;
	static std::string::size_type ptr;
	static std::string tmp;
	_servers = &_data->getServers();

	hdr = ft::split(client->_hdr, delimHeaders);
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
			fieldName = hdr[i].substr(0, ptr);
			fieldValue = ft::trim(hdr[i].substr(ptr + 1), delimConfig);
			client->_hdrMap[ft::tolower(fieldName)] = fieldValue;
		}
		else if (!hdr[i].empty())
			throw HttpStatusCode("400");
	}
	if (client->_hdrMap["http_version"] == "1.1") {
		if (!client->_hdrMap.count("host"))
			throw HttpStatusCode("400");
	}

	hdr = ft::split(client->_hdrMap["host"], ":");
	client->_hdrMap["server_name"] = ft::trim(hdr[0], delimConfig);
	client->_hdrMap["server_port"] = ft::trim(hdr[1], delimConfig);
	for (Server::_svrsType::const_iterator it = _servers->begin(); it != _servers->end(); it++) {
		if (client->_acptSvr->_listenPort == (*it)._listenPort) {
			if (!client->_respSvr)
				client->_respSvr = &*it;
			else {
				if (std::find(it->_serverName.begin(), it->_serverName.end(), client->_hdrMap["server_name"]) != it->_index.end())
					client->_respSvr = &*it;
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

/*
** Проверяю тип тела.
*/
std::pair<int, long> Request::getBodyType(Client *client) {
	if (client->_hdrMap.find("transfer-encoding") != client->_hdrMap.end()) {
		if (client->_hdrMap["transfer-encoding"].find("chunked") != std::string::npos)
			return (std::make_pair(e_recvChunkBody, 0));
	}
	else if (client->_hdrMap.find("content-length") != client->_hdrMap.end()) {
		char *ptr;
		long contentLength;

		contentLength = strtol(client->_hdrMap["content-length"].c_str(), &ptr, 10);
		if (contentLength == LONG_MIN ||
			contentLength == LONG_MAX)
			throw std::runtime_error("Request::getBodyType() strtol error");

		if (!(*ptr)) {
			if (contentLength > 0)
				return (std::make_pair(e_recvContentBody, contentLength));
			return (std::make_pair(e_sendResponse, 0));
		}
		else
			throw HttpStatusCode("404");
	}
	return (std::make_pair(e_sendResponse, 0));
}
