#include "parser/Request.hpp"

/*
** Создаю буффер для получения запросов.
*/
Request::Request() {
	_bodyBuffer = 1024*1024*1024;
	_buffer.reserve(_bodyBuffer + 1);
}

Request::Request(const Request &other): _bodyBuffer(other._bodyBuffer),
										_buffer(other._buffer) {}

Request::~Request() {}

Request &Request::operator=(const Request &other) {
	if (this != &other) {
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
		client->setHttpStatusCode(HttpStatusCode("200"));
	}
}

/*
** Читаю тело с заголовком content-length, использую вектор символов,
** чтобы не иметь проблем с выделением, удалением памяти.
*/
void Request::recvContentBody(Client::_clientIt &clientIt) {
	static Client* client;
	static long size;
	static long valread;

	client = (*clientIt);
	size = client->getSize();
	if (size > _bodyBuffer) {
		_buffer.resize(size);
		_bodyBuffer = size;
	}

	valread = recv(client->getSocket(), &_buffer[0], size, 0);
	if (valread > 0) {
		_buffer[valread] = '\0';
		client->appendBody(&_buffer[0]);
		client->setFlag(e_sendResponse);
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
void Request::recvChunkBody(Client::_clientIt &clientIt) {
	static Client* client;
	static long chunkMod;
	static long valread;

	client = (*clientIt);
	chunkMod = client->getChunkMod();

	if (chunkMod == e_recvChunkData) {
		static long size;

		size = client->getSize() + 2;
		if (size > _bodyBuffer) {
			_buffer.resize(size + 1);
			_bodyBuffer = size;
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

/*
** Записываю все заголовки в ассоциативный массив, проверяю на правильность
*/
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
		else
			throw HttpStatusCode("404");
	}
	return (std::make_pair(e_sendResponse, 0));
}
