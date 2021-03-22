#include "server/Server.hpp"

/*
** Буффер размером 1 мб, с помощью reserve, выделяю память,
** в ассоциативный массив добавляю пару - (флаг, функция)
*/
Server::Server(Data* data) {
	_data = data;
	BODY_BUFFER = 1024*1024*1024;
	_buffer.reserve(BODY_BUFFER + 1);
	_funcMap.insert(std::make_pair(ft::e_closeConnection, &Server::closeConnection));
	_funcMap.insert(std::make_pair(ft::e_recvHeaders, &Server::recvHeaders));
	_funcMap.insert(std::make_pair(ft::e_recvContentBody, &Server::recvContentBody));
	_funcMap.insert(std::make_pair(ft::e_recvChunkBody, &Server::recvChunkBody));
	_funcMap.insert(std::make_pair(ft::e_sendResponse, &Server::sendResponse));
}

/*
** В случае закрытия сервера, при Ctrl-C очищаю всех клиентов
*/
Server::~Server() {
	for (Client::_clientsType::iterator it = _clients.begin(); it != _clients.end(); it++)
		closeConnection(it);
}

/*
** Статическая переменная, в случае когда несколько серверов,
** надо все их закрыть, когда перехвачен сигнал
*/
int& Server::getSignal() {
	static int signal = 0;
	return (signal);
}

void Server::setData(Data* data) {
	_data = data;
}

/*
** Закрываю сокет, удаляю Client* и удаляю из списка
*/
void Server::closeConnection(Client::_clientsType::iterator& client_it) {
	std::cout << "Close socket " << (*client_it)->getSocket() << std::endl;
	close((*client_it)->getSocket());
	delete *client_it;
	_clients.erase(client_it++);
}

/*
** Читаю заголовки по 1 символу, если это конец заголовков "\r\n\r\n",
** то обрабатываю их, если что-то не так, то бросается\ловится исключение,
** и сохраняется значение кода ошибки
*/
void Server::recvHeaders(Client::_clientsType::iterator& it) {
	static Client* client;

	client = (*it);

	long valread = recv(client->getSocket(), &_buffer[0], 1, 0);
	if (valread > 0) {
		_buffer[valread] = '\0';
		client->appendHeader(&_buffer[0]);
	}
	if (ft::isLastEqual(client->getHeader(), "\r\n\r\n")) {
		try {
			client->parseHeaders();
			std::pair<int, long> pairType = client->getRequest()->getBodyType();
			client->setFlag(pairType.first);
			client->setSize(pairType.second);
			client->getHttpStatusCode()->setStatusCode("200");
		}
		catch (HttpStatusCode& httpStatusCode) {
			std::cout << "$" << _data->getMessage(&httpStatusCode) << "$" << std::endl;
			client->getHttpStatusCode()->setStatusCode(httpStatusCode.getStatusCode());
//			client->setFlag(ft::e_closeConnection);
			client->setFlag(ft::e_sendResponse);
		}
		client->getHttpStatusCode()->setStatusCode("200");

		std::cout << "RecvSocket = " << client->getSocket() << std::endl;
//		send(client->getSocket(), "HelloRecv\n", 11, MSG_DONTWAIT);
	}
}

/*
** Читаю тело с заголовком content-length, потом распечатываю
*/
void Server::recvContentBody(Client::_clientsType::iterator& it) {
	static Client* client;
	static long size;
	static long valread;

	client = (*it);
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
	else if (valread == size) {
		client->parseBody();
//		client->setFlag(ft::e_closeConnection);
		client->setFlag(ft::e_sendResponse);
	}
}

void Server::recvChunkBody(Client::_clientsType::iterator& it) {
	static Client* client;
	static long chunkMod, valread;

	client = (*it);
	chunkMod = client->getChunkMod();

	if (chunkMod == ft::e_recvChunkData) {
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

		client->setChunkMod(ft::e_recvChunkHex);
	}
	else {
		static std::string headers_delim = "\r\n";

		valread = recv(client->getSocket(), &_buffer[0], 1, 0);
		if (valread > 0) {
			_buffer[valread] = '\0';
			client->appendHexNum(&_buffer[0]);
		}

		if (ft::isLastEqual(client->getHexNum(), "\r\n")) {
 			static std::string trim;
			static char *ptr;
			static size_t result;

			trim = ft::trim(client->getHexNum(), headers_delim);
			result = strtol(&(trim[0]), &ptr, 16);
			if (!(*ptr))
				client->setSize(result);
			else {
				/*
				** Ошибка
				*/
			}
			client->setChunkMod(ft::e_recvChunkData);
			client->setHexNum("");
		}
	}

	if (valread == 0) {
		client->parseBody();
//		client->setFlag(ft::e_closeConnection);
		client->setFlag(ft::e_sendResponse);
	}
}

void Server::sendResponse(std::list<Client *>::iterator &it) {
	static Client* client;
	static Request* request;

	client = (*it);
	request = client->getRequest();

//	std::string response = request->sendResponse();
//	std::cout << "response:\n" << response.c_str() << std::endl;
	long valread;
	std::string response = "HTTP/1.1 404 Not Found\n"
						   "Server: nginx/1.18.0 (Ubuntu)\n"
						   "Date: Fri, 19 Mar 2021 10:19:33 GMT\n"
						   "Content-Type: text/html\n"
						   "Content-Length: 162\n"
						   "Connection: close\n"
						   "\r\n\r\n"
						   "<html>\n"
						   "<head><title>404 Not Found</title></head>\n"
						   "<body>\n"
						   "<center><h1>404 Not Found</h1></center>\n"
						   "<hr><center>nginx/1.18.0 (Ubuntu)</center>\n"
						   "</body>\n"
						   "</html>\r\n\r\n";
	std::cout << "response:\n" << response << std::endl;
	valread = send(client->getSocket(), response.c_str(), response.size(), MSG_DONTWAIT);
	std::cout << "valread = " << valread << ", flag = " << request->keepAlive() << std::endl;
	client->setFlag(request->keepAlive());
}

void Server::initSet(std::list<Client *>::iterator &client_it) {
	static int flag;

	flag = (*client_it)->getFlag();
	if (flag == ft::e_closeConnection) {
		closeConnection(client_it);
	}
	else if (flag == ft::e_recvHeaders ||\
		flag == ft::e_recvContentBody ||\
		flag == ft::e_recvChunkBody) {
		FD_SET((*client_it)->getSocket(), &_readSet);
	}
	else {
		FD_SET((*client_it)->getSocket(), &_writeSet);
	}
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
	_address.sin_port = ft::htons(PORT);
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
	_clients.push_back(new Client(_data, listen_sd, 0));

	while (true)
	{
		/*
		** В цикле каждый раз, перед вызовом select его нужно заново инициализировать,
		** так как он затирает данные после вызова.
		*/
		FD_ZERO(&_readSet);
		FD_ZERO(&_writeSet);
		for (Client::_clientsType::iterator client_it = _clients.begin(); client_it != _clients.end(); client_it++) {
			tmp = (*client_it)->getSocket();
			max_sd = std::max(tmp, max_sd);
			initSet(client_it);
		}

		/*
		** timeout означает, что select будет ожидать готовности одного из сокетов(типа как поток ожидает mutex_lock)
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
		** закрываем сокет и отправляем данные на обработку, потом удаляем сокет и
		** данные из текущих.
		*/
		for (Client::_clientsType::iterator client_it = _clients.begin(); client_it != _clients.end(); client_it++) {
			static int socket;
			static int flag;

			socket = (*client_it)->getSocket();
			flag = (*client_it)->getFlag();

			if (FD_ISSET(socket, &_readSet)) {
				if (socket == listen_sd) {
					if ((new_sd = accept(listen_sd, (struct sockaddr *) &_address, &addrlen)) == -1) {
						strerror(errno);
						continue;
					}
					else {
						_clients.push_back(new Client(_data, new_sd, ft::e_recvHeaders));
						max_sd = std::max(new_sd, max_sd);
					}
				}
				else
					(this->*_funcMap.find(flag)->second)(client_it);
			}
			if (FD_ISSET(socket, &_writeSet)) {
				if (socket != listen_sd)
					(this->*_funcMap.find(flag)->second)(client_it);
			}

		}
		if (getSignal() == SIGINT)
			break;
	}
	return (0);
}

void *funcTest(void *it) {

}

void test(std::list<Client *>::iterator &it) {
	pthread_t thread;

	static std::map<int, void* (*)(void *)> funMap;
	funMap.insert(std::make_pair(0, &funcTest));

	pthread_create(&thread, NULL, (*funMap.find(0)).second, reinterpret_cast<void*>(*it));
}

