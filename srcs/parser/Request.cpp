#include "parser/Request.hpp"

/*
** Создаю буффер для получения запросов.
*/
Request::Request(const Data* data) {
	_data = data;
	_bodyBuffer = 1000*1000*1000;
	_buffer.reserve(_bodyBuffer + 1);
}

Request::Request(const Request &other): _data(other._data),
										_bodyBuffer(other._bodyBuffer),
										_buffer(other._buffer) {}

Request::~Request() {}

Request &Request::operator=(const Request &other) {
	if (this != &other) {
		_data = other._data;
		_bodyBuffer = other._bodyBuffer;
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
	long valread = recv(client->_socket, &_buffer[0], readHeaderSize(client->_headers), 0);
	if (valread > 0) {
		_buffer[valread] = '\0';
		client->_headers.append(&_buffer[0]);
	}
	else if (valread < 0)
		throw std::runtime_error("recv error");

	if (isLastEqual(client->_headers, "\r\n\r\n")) {
		parseHeaders(client);
		std::pair<int, long> pairType = getBodyType(client);
		client->_flag = pairType.first;
		client->_size = pairType.second;
		client->_httpStatusCode = HttpStatusCode("200");
	}
}

/*
** Читаю тело с заголовком content-length, использую вектор символов,
** чтобы не иметь проблем с выделением, удалением памяти.
*/
void Request::recvContentBody(Client *client) {
	static long size;
	static long valread;

	size = client->_size;
	if (size > _bodyBuffer) {
		_buffer.resize(size);
		_bodyBuffer = size;
	}
	valread = recv(client->_socket, &_buffer[0], size, 0);
	if (valread > 0) {
		_buffer[valread] = '\0';
		client->_body.append(&_buffer[0]);
		client->_flag = e_sendResponse;
	}
	else
		throw std::runtime_error("recv error");
}

/*
** Читаю тело с заголовком transfer-encoding = chunk, сначала
** получаю число в 16-ой системе счисления - это размер следующей
** части запроса, потом читаю этот запрос и так до бесконечности, пока
** размер запроса != 0.
*/
void Request::recvChunkBody(Client *client) {
	static long chunkMod;
	static long valread;

	chunkMod = client->_chunkMod;
	if (chunkMod == e_recvChunkData) {
		static long size;

		size = client->_size + 2;
		if (size > _bodyBuffer) {
			_buffer.resize(size + 1);
			_bodyBuffer = size;
		}

		valread = recv(client->_socket, &_buffer[0], size, 0);
		if (valread > 0) {
			_buffer[valread] = '\0';
			client->_body.append(&_buffer[0]);
		}
		client->_chunkMod = e_recvChunkHex;
	}
	else {
		static std::string headers_delim("\r\n");

		valread = recv(client->_socket, &_buffer[0], 1, 0);
		if (valread > 0) {
			_buffer[valread] = '\0';
			client->_hexNum.append(&_buffer[0]);
		}

		if (isLastEqual(client->_hexNum, "\r\n")) {
		    client->_size = strToLong(client->_hexNum);
			client->_chunkMod = e_recvChunkData;
			client->_hexNum.clear();
		}
	}
	if (valread == 0)
		client->_flag = e_sendResponse;
}

/*
** Записываю все заголовки в ассоциативный массив, проверяю на правильность
*/
void Request::parseHeaders(Client *client) {
	static std::vector<std::string> headers;
	static std::vector<std::string> requestLine;
	static std::string::size_type ptr;
	_servers = &_data->getServers();

	headers = split(client->_headers, "\r\n");
	requestLine = split(headers[0], " ");
	if (requestLine.size() != 3 ||\
	((ptr = requestLine[2].find("/")) == std::string::npos) ||\
	!isAllowedMethod(requestLine[0])) {
		client->_headersMap["http_version"] = "1.1";
		throw HttpStatusCode("400");
	}
	else {
		client->_headersMap["method"] = requestLine[0];
		client->_headersMap["request_target"] = requestLine[1];
		client->_headersMap["http_version"] = requestLine[2].substr(ptr + 1);
	}

	static std::string field_name;
	static std::string field_value;
	static std::string header_delim( " \t");

	for (size_t i = 1; i < headers.size(); i++) {
		if ((ptr = headers[i].find(":")) != std::string::npos) {
			field_name = headers[i].substr(0, ptr);
			field_value = trim(headers[i].substr(ptr + 1), header_delim);
			client->_headersMap[toLower(field_name)] = toLower(field_value);
		}
		else if (!headers[i].empty())
			throw HttpStatusCode("400");
	}
	if (client->_headersMap["http_version"] == "1.1") {
		if (!client->_headersMap.count("host"))
			throw HttpStatusCode("400");
	}

	for (Server::_serversType::const_iterator it = _servers->begin(); it != _servers->end(); it++) {
		if (client->_acceptServer->_listenPort == (*it)._listenPort) {
			if (!client->_responseServer)
				client->_responseServer = &*it;
			else {
				std::string host = split(client->_headersMap["host"], ":")[0];
				if (std::find(it->_serverName.begin(), it->_serverName.end(), host) != it->_index.end())
					client->_responseServer = &*it;
			}
		}
	}

	static size_t maxLen;
	static size_t uriLen;

	maxLen = 0;
	for (	Location::_locationsType::const_iterator it = client->_responseServer->_locations.begin();
			 it != client->_responseServer->_locations.end(); it++) {
		uriLen = it->_uri.size();
		if (!strncmp(it->_uri.c_str(), client->_headersMap["request_target"].c_str(), uriLen) && uriLen > maxLen) {
			client->_responseLocation = &*it;
			maxLen = uriLen;
		}
	}

	std::cout << "_client->_responseLocation->_uri = " << client->_responseLocation->_uri << std::endl;
	std::cout << "extract = " << client->_headersMap["request_target"].substr(maxLen) << std::endl;
	std::cout << "path = " << client->_responseLocation->_root+"/"+(client->_headersMap)["request_target"].substr(maxLen) << std::endl;


}

/*
** Проверка метода из заголовка на валидность.
*/
bool Request::isAllowedMethod(const std::string &method) {
	static std::string allowedMethods[] = { 	"GET", "HEAD", "POST", "PUT",
												"DELETE", "CONNECT", "OPTIONS", "TRACE" };
	static const int numOfMethods = sizeof(allowedMethods)/sizeof(allowedMethods[0]);

	if (std::find(allowedMethods, allowedMethods+numOfMethods, method) != allowedMethods+numOfMethods)
		return (true);
	return (false);
}

/*
** Проверяю тип тела.
*/
std::pair<int, long> Request::getBodyType(Client *client) {
	if (client->_headersMap.find("transfer-encoding") != client->_headersMap.end()) {
		if (client->_headersMap["transfer-encoding"].find("chunked") != std::string::npos)
			return (std::make_pair(e_recvChunkBody, 0));
	}
	else if (client->_headersMap.find("content-length") != client->_headersMap.end()) {
		char *ptr;
		long content_length;

		content_length = strtol(client->_headersMap["content-length"].c_str(), &ptr, 10);
		if (!(*ptr))
			return (std::make_pair(e_recvContentBody, content_length));
		else
			throw HttpStatusCode("404");
	}
	return (std::make_pair(e_sendResponse, 0));
}
