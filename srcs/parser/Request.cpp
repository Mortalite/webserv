#include "parser/Request.h"

Request::Request() {
	BODY_BUFFER = 1024*1024*1024;
	_buffer.reserve(BODY_BUFFER + 1);
}

Request::~Request() {

}

/*
** Читаю заголовки по 1 символу, если это конец заголовков "\r\n\r\n",
** то обрабатываю их, если что-то не так, то бросается\ловится исключение,
** и сохраняется значение кода ошибки
*/
void Request::recvHeaders(Client::_clientIt &clientIt) {
	static Client* client;

	client = (*clientIt);
	long valread = recv(client->getSocket(), &_buffer[0], readHeaderSize(client->getHeaders()), 0);
	if (valread > 0) {
		_buffer[valread] = '\0';
		client->appendHeader(&_buffer[0]);
	}
	else if (valread < 0)
		throw std::runtime_error("recv error");

	if (isLastEqual(client->getHeaders(), "\r\n\r\n")) {
		parseHeaders(clientIt);
		std::pair<int, long> pairType = getBodyType(clientIt);
		client->setFlag(pairType.first);
		client->setSize(pairType.second);
		client->getHttpStatusCode()->setStatusCode("200");
	}
}

/*
** Читаю тело с заголовком content-length, потом распечатываю
*/
void Request::recvContentBody(Client::_clientIt &clientIt) {
	static Client* client;
	static long size;
	static long valread;

	client = (*clientIt);
	size = client->getSize() + 2;
	if (size > BODY_BUFFER) {
		_buffer.resize(size + 1);
		BODY_BUFFER = size;
	}

	valread = recv(client->getSocket(), &_buffer[0], size, 0);
	if (valread > 0) {
		_buffer[valread] = '\0';
		client->appendBody(&_buffer[0]);
	}
	else if (valread == 0)
		client->setFlag(e_sendResponse);
	else
		throw std::runtime_error("recv error");
}

void Request::recvChunkBody(Client::_clientIt &clientIt) {
	static Client* client;
	static long chunkMod;
	static long valread;

	client = (*clientIt);
	chunkMod = client->getChunkMod();

	if (chunkMod == e_recvChunkData) {
		static long size;

		size = client->getSize() + 2;
		if (size > BODY_BUFFER) {
			_buffer.resize(size + 1);
			BODY_BUFFER = size;
		}

		valread = recv(client->getSocket(), &_buffer[0], size, 0);
		if (valread > 0) {
			_buffer[valread] = '\0';
			client->appendBody(&_buffer[0]);
		}

		client->setChunkMod(e_recvChunkHex);
	}
	else {
		static std::string headers_delim("\r\n");

		valread = recv(client->getSocket(), &_buffer[0], 1, 0);
		if (valread > 0) {
			_buffer[valread] = '\0';
			client->appendHexNum(&_buffer[0]);
		}

		if (isLastEqual(client->getHexNum(), "\r\n")) {
			static std::string str;
			static char *ptr;
			static size_t result;

			str = trim(client->getHexNum(), headers_delim);
			result = strtol(&str[0], &ptr, 16);
			if (!(*ptr))
				client->setSize(result);
			else {
				/*
				** Ошибка
				*/
			}
			client->setChunkMod(e_recvChunkData);
			client->setHexNum("");
		}
	}

	if (valread == 0)
		client->setFlag(e_sendResponse);
}

void Request::parseHeaders(Client::_clientIt &clientIt) {
	Client::_headersType& headersMap = (*clientIt)->getHeadersMap();
	static std::vector<std::string> headers;
	static std::vector<std::string> requestLine;
	static std::string::size_type ptr;

	headers = split((*clientIt)->getHeaders(), "\r\n");
	requestLine = split(headers[0], " ");
	if (requestLine.size() != 3 ||\
	((ptr = requestLine[2].find("/")) == std::string::npos) ||\
	!isAllowedMethod(requestLine[0])) {
		headersMap["http_version"] = "1.1";
		throw HttpStatusCode("400");
	}
	else {
		headersMap["method"] = requestLine[0];
		headersMap["request_target"] = requestLine[1];
		headersMap["http_version"] = requestLine[2].substr(ptr + 1);
	}

	static std::string field_name;
	static std::string field_value;
	static std::string header_delim( " \t");

	for (size_t i = 1; i < headers.size(); i++) {
		if ((ptr = headers[i].find(":")) != std::string::npos) {
			field_name = headers[i].substr(0, ptr);
			field_value = trim(headers[i].substr(ptr + 1), header_delim);
			headersMap[toLower(field_name)] = toLower(field_value);
		}
		else if (!headers[i].empty())
			throw HttpStatusCode("400");
	}
	if (headersMap["http_version"] == "1.1" || headersMap["http_version"] == "2.0") {
		if (headersMap.find("host") == headersMap.end())
			throw HttpStatusCode("400");
	}
}

bool Request::isAllowedMethod(const std::string &method) {
	static std::string allowedMethods[8] = { "GET", "HEAD", "POST", "PUT",\
											"DELETE", "CONNECT", "OPTIONS", "TRACE" };
	static int numOfMethods = 8;
	static int i;

	i = 0;
	for (;i < numOfMethods; i++) {
		if (method == allowedMethods[i])
			return (true);
	}
	return (false);
}

std::pair<int, long> Request::getBodyType(Client::_clientIt &clientIt) {
	Client::_headersType& headersMap = (*clientIt)->getHeadersMap();

	if (headersMap.find("transfer-encoding") != headersMap.end()) {
		if (headersMap["transfer-encoding"].find("chunked") != std::string::npos)
			return (std::make_pair(e_recvChunkBody, 0));
	}
	else if (headersMap.find("content-length") != headersMap.end()) {
		char *ptr;
		long content_length;

		content_length = strtol(headersMap["content-length"].c_str(), &ptr, 10);
		if (!(*ptr))
			return (std::make_pair(e_recvContentBody, content_length));
	}
	return (std::make_pair(e_sendResponse, 0));
}