#include "server/Server.hpp"

/*
** Буффер размером 1 мб, с помощью reserve, выделяю память,
** в ассоциативный массив добавляю пару - (флаг, функция)
*/
Server::Server(Data* data) {
	_data = data;
	_request = new Request(_data);
	BODY_BUFFER = 1024*1024*1024;
	_buffer.reserve(BODY_BUFFER + 1);
	_funcMap.insert(std::make_pair(e_recvHeaders, &Server::recvHeaders));
	_funcMap.insert(std::make_pair(e_recvContentBody, &Server::recvContentBody));
	_funcMap.insert(std::make_pair(e_recvChunkBody, &Server::recvChunkBody));
	_funcMap.insert(std::make_pair(e_sendResponse, &Server::sendResponse));
	_funcMap.insert(std::make_pair(e_closeConnection, &Server::closeConnection));
}

/*
** В случае закрытия сервера, при Ctrl-C очищаю всех клиентов
*/
Server::~Server() {
    delete _request;
	for (Client::_clientIt clientIt = _clients.begin(); clientIt != _clients.end();)
		closeConnection(clientIt);
}

/*
** Статическая переменная, в случае когда несколько серверов,
** надо все их закрыть, когда перехвачен сигнал
*/
int& Server::getSignal() {
	static int signal = 0;
	return (signal);
}

Request *Server::getRequest() {
    return (_request);
}

void Server::setData(Data* data) {
	_data = data;
}

/*
** Закрываю сокет, удаляю Client* и удаляю из списка
*/
void Server::closeConnection(Client::_clientIt &clientIt) {
	close((*clientIt)->getSocket());
	delete *clientIt;
	_clients.erase(clientIt++);
}

/*
** Читаю заголовки по 1 символу, если это конец заголовков "\r\n\r\n",
** то обрабатываю их, если что-то не так, то бросается\ловится исключение,
** и сохраняется значение кода ошибки
*/
void Server::recvHeaders(Client::_clientIt &clientIt) {
	static Client* client;

	client = (*clientIt);
//	DEBUG
//	std::cout << "readHeaderSize = " << ft::readHeaderSize(client->getHeaders()) << std::endl;
	long valread = recv(client->getSocket(), &_buffer[0], readHeaderSize(client->getHeaders()), 0);
	if (valread > 0) {
		_buffer[valread] = '\0';
		client->appendHeader(&_buffer[0]);
	}
	if (isLastEqual(client->getHeaders(), "\r\n\r\n")) {
		try {
            _request->parseHeaders(clientIt);
			std::pair<int, long> pairType = _request->getBodyType(clientIt);
			client->setFlag(pairType.first);
			client->setSize(pairType.second);
			client->getHttpStatusCode()->setStatusCode("200");

        }
		catch (HttpStatusCode& httpStatusCode) {
            client->setHttpStatusCode(httpStatusCode);
            client->setFlag(e_sendResponse);
		}
	}
}

/*
** Читаю тело с заголовком content-length, потом распечатываю
*/
void Server::recvContentBody(Client::_clientIt &clientIt) {
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
}

void Server::recvChunkBody(Client::_clientIt &clientIt) {
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

void Server::sendResponse(Client::_clientIt &clientIt) {
	static Client* client;
	static std::string* response;
	static long valread;

	client = (*clientIt);
    std::cout << "headers:\n" << client->getHeaders() << std::endl;
    std::cout << "_body:\n" << client->getBody() << std::endl;

	try {
        response = &_request->getResponse(clientIt);
    }
	catch (std::runtime_error &error) {
	    std::cout << "runtime_error = " << error.what() << std::endl;
        closeConnection(clientIt);
        return ;
	}
    std::cout << "RESPONSE:\n" <<   response->c_str();
    valread = send(client->getSocket(), response->c_str(), response->size(), MSG_DONTWAIT);
	client->setFlag(_request->isKeepAlive(clientIt));
    client->wipeData();
}

void Server::initSet(Client::_clientIt &clientIt) {
	static int flag;

	flag = (*clientIt)->getFlag();
	if (flag == e_closeConnection)
		closeConnection(clientIt);
	else if (flag == e_recvHeaders ||\
		flag == e_recvContentBody ||\
		flag == e_recvChunkBody) {
		FD_SET((*clientIt)->getSocket(), &_readSet);
	}
	else
		FD_SET((*clientIt)->getSocket(), &_writeSet);
}

int Server::runServer() {
	int reuse = 1;
	int listen_sd;
	int new_sd;
	int max_sd = -1;
	int tmp;

	socklen_t addrlen;
	addrlen = sizeof(_address);

	static timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	/*
	** Записываем в структуру следующие параметры:
	** семейство адресов (AF_INET - IPv4)
	** адрес сокета (INADDR_ANY - 0.0.0.0)
	** адрес порта в сетевом виде (8080 - 36895)
	*/
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(PORT);
	memset(_address.sin_zero, '\0', sizeof(_address.sin_zero));

	/*
	** Создаю сокет, ставлю режим прослушивания на неблокирующий,
	** так же ставлю флаг для переиспользования сокета, связываю со структурой,
	** и начинаю прослушивать
	*/
	if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1 || \
		((fcntl(listen_sd, F_SETFL, O_NONBLOCK)) == -1) || \
		(setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) || \
		(bind(listen_sd, (struct sockaddr *)&_address, sizeof(_address)) < 0) || \
		(listen(listen_sd, SOMAXCONN) < 0)) {
		strerror(errno);
	}

	/*
	** Макросы для работы с select
	** FD_SET(int fd, fd_set* set) - добавляет дескриптор в набор
	** FD_CLR(int fd, fd_set* set) - удаляет дескриптор из набора
	** FD_ISSET(int fd, fd_set* set) - возвращает !0, если дескриптор есть в наборе
	** FD_ZERO(fd_set* set) - очищает набор
	** Насколько я понял, это просто структура, в которой максимум 1024 дескриптора и
	** в зависимости от макроса он в ней выставляет флаг.
	*/
	_clients.push_back(new Client(listen_sd, e_recvHeaders));
	while (true)
	{
		/*
		** В цикле каждый раз, перед вызовом select его нужно заново инициализировать,
		** так как он затирает данные после вызова.
		*/
		FD_ZERO(&_readSet);
		FD_ZERO(&_writeSet);
		for (Client::_clientIt clientIt = _clients.begin(); clientIt != _clients.end(); clientIt++) {
			tmp = (*clientIt)->getSocket();
			max_sd = std::max(tmp, max_sd);
			initSet(clientIt);
		}
		/*
		** timeout означает, что select будет ожидать указанное время (я поставил (0,0) он будет постоянно
		** пробегаться по всем сокетам и проверять их)
		*/
		if ((tmp = select(max_sd + 1, &_readSet, &_writeSet, NULL, &timeout)) == -1)
			strerror(errno);
//		else if (tmp == 0)
//			std::cerr << "Time expired" << std::endl;

		/*
		** В цикле прохожу по всем сокетам(1 - для прослушивания, остальные - для чтения),
		** если сокет ещё в наборе, то для прослушивающего сокета, я принимаю соединение,
		** то есть создается новый сокет, я добавляю его в ассоциативный массив с пустыми
		** данными и увеличиваю максимальный прослушиваемый сокет, иначе, если сокет для
		** чтения я читаю из него данные, если valread != 0, значит прочитались данные,
		** обновляем массив, если 0, значит соединение закрылось(все данные передались),
		** закрываем сокет и отправляем данные на обработку, потом клиента из текущих.
		*/
		for (Client::_clientIt clientIt = _clients.begin(); clientIt != _clients.end(); clientIt++) {
			static int socket;
			static int flag;

			socket = (*clientIt)->getSocket();
			flag = (*clientIt)->getFlag();

			if (FD_ISSET(socket, &_readSet)) {
				if (socket == listen_sd) {
					if ((new_sd = accept(listen_sd, (struct sockaddr *) &_address, &addrlen)) == -1) {
						strerror(errno);
						continue;
					}
					else {
						_clients.push_back(new Client(new_sd, e_recvHeaders));
						max_sd = std::max(new_sd, max_sd);
                        fcntl(new_sd, F_SETFL, O_NONBLOCK);
					}
				}
				else
					(this->*_funcMap.find(flag)->second)(clientIt);
			}
			if (FD_ISSET(socket, &_writeSet)) {
				if (socket != listen_sd)
					(this->*_funcMap.find(flag)->second)(clientIt);
			}

		}
		if (getSignal() == SIGINT)
			break;
	}
	return (0);
}

