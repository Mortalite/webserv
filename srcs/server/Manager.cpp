#include "server/Manager.hpp"

/*
** Создаю объекты для обработки запроса, в ассоциативный массив добавляю пару - (флаг, функция)
*/
Manager::Manager(Data *data) {
	_data = data;
	_request = new Request();
	_response = new Response(_data);
	_server = &data->getServers();
	_funcMap.insert(std::make_pair(e_recvHeaders, &Manager::recvHeaders));
	_funcMap.insert(std::make_pair(e_recvContentBody, &Manager::recvContentBody));
	_funcMap.insert(std::make_pair(e_recvChunkBody, &Manager::recvChunkBody));
	_funcMap.insert(std::make_pair(e_sendResponse, &Manager::sendResponse));
}

Manager::Manager(const Manager &other): _funcMap(other._funcMap),
										_address(other._address),
										_readSet(other._readSet),
										_writeSet(other._writeSet),
										_data(other._data),
										_request(other._request),
										_response(other._response),
										_clients(other._clients) {}

Manager &Manager::operator=(const Manager &other) {
	if (this != &other) {
		_funcMap = other._funcMap;
		_address = other._address;
		_readSet = other._readSet;
		_writeSet = other._writeSet;
		_data = other._data;
		_request = other._request;
		_response = other._response;
		_clients = other._clients;
	}
	return (*this);
}

/*
** В случае закрытия сервера, при Ctrl-C очищаю объекты и
** закрываю соединения со всеми клиентами
*/
Manager::~Manager() {
	delete _request;
	delete _response;
	for (Client::_clientIt clientIt = _clients.begin(); clientIt != _clients.end();)
		closeConnection(clientIt);
}

/*
** Статическая переменная, в случае когда несколько серверов,
** надо все их закрыть, когда перехвачен сигнал
*/
int& Manager::getSignal() {
	static int signal = 0;
	return (signal);
}

/*
** Закрываю сокет, удаляю Client* и удаляю из списка
*/
void Manager::closeConnection(Client::_clientIt &clientIt) {
	std::cout << "close = " << (*clientIt)->_socket << std::endl;
	close((*clientIt)->_socket);
	delete *clientIt;
	_clients.erase(clientIt++);
}

/*
** Читаю заголовки
*/
void Manager::recvHeaders(Client *client) {
	_request->recvHeaders(client);
}

/*
** Читаю тело с заголовком content-length
*/
void Manager::recvContentBody(Client *client) {
	_request->recvContentBody(client);
}

/*
** Читаю тело с заголовком transfer-encoding = chunk
*/
void Manager::recvChunkBody(Client *client) {
	_request->recvChunkBody(client);
}

/*
** Отправляю ответ
*/
void Manager::sendResponse(Client *client) {
	_response->sendResponse(client);
}

/*
** В зависимости от флага закрываю соединение, либо добавляю
** сокет в набор, который при выполнении select
** проверяет готовность сокета на чтение/запись.
*/
void Manager::initSet(Client *client) {
	static int flag;
	static int socket;

	flag = client->_flag;
	socket = client->_socket;
	if (flag == e_sendResponse)
		FD_SET(socket, &_writeSet);
	else
		FD_SET(socket, &_readSet);
}

int Manager::launchManager() {
	int newSocket;
	int maxSocket = -1;
	int tmp;

	socklen_t addrlen = sizeof(_address);

	static timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	for (Server::_serversType::const_iterator it = _server->begin(); it != _server->end(); it++) {
		int listenSocket;
		int reuse = 1;
		long port = (*it)._listenPort;
		const std::string &host = (*it)._host;

		_address.sin_family = AF_INET;
		if (host == "localhost")
			_address.sin_addr.s_addr = INADDR_ANY;
		else
			_address.sin_addr.s_addr = inet_addr(host.c_str());
		_address.sin_port = htons(port);
		memset(_address.sin_zero, '\0', sizeof(_address.sin_zero));

		if (((listenSocket) = socket(AF_INET, SOCK_STREAM, 0)) == -1 ||
			((fcntl(listenSocket, F_SETFL, O_NONBLOCK)) == -1) ||
			(setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) ||
			(bind(listenSocket, (struct sockaddr *)&_address, sizeof(_address)) < 0) ||
			(listen(listenSocket, SOMAXCONN) < 0)) {
			strerror(errno);
		}
		_clients.push_back(new Client(&*it, listenSocket, e_listen));
	}

//	size_t counter = 0;
//	for (typename Client::_clientIt it = _clients.begin(); it != _clients.end(); it++)
//		std::cout << "client[" << counter++ << "] = " << **it << std::endl;

	while (true)
	{
		FD_ZERO(&_readSet);
		FD_ZERO(&_writeSet);

		for (Client::_clientIt clientIt = _clients.begin(); clientIt != _clients.end();) {
			if ((*clientIt)->_flag == e_closeConnection)
				closeConnection(clientIt);
			else clientIt++;
		}

		for (Client::_clientIt clientIt = _clients.begin(); clientIt != _clients.end(); clientIt++) {
			tmp = (*clientIt)->_socket;
			maxSocket = std::max(tmp, maxSocket);
			initSet(*clientIt);
		}
		
		if ((tmp = select(maxSocket + 1, &_readSet, &_writeSet, NULL, &timeout)) == -1)
			continue;

		for (Client::_clientIt clientIt = _clients.begin(); clientIt != _clients.end(); clientIt++) {
			static Client* client;
			static int socket;
			static int flag;

			client = (*clientIt);
			socket = client->_socket;
			flag = client->_flag;

			if (FD_ISSET(socket, &_readSet)) {
				if (flag == e_listen) {
					if ((newSocket = accept(socket, (struct sockaddr *)&_address, &addrlen)) == -1)
						continue;
					else {
						_clients.push_back(new Client(client->_acceptServer, newSocket, e_recvHeaders));
						maxSocket = std::max(newSocket, maxSocket);
						if (fcntl(newSocket, F_SETFL, O_NONBLOCK) == -1)
							continue;
					}
				}
				else {
					try {
						(this->*_funcMap.find(flag)->second)(*clientIt);
					}
					catch (HttpStatusCode& httpStatusCode) {
						client->_httpStatusCode = httpStatusCode;
						client->_flag = e_sendResponse;
					}
					catch (std::runtime_error& error) {
						std::cout << "functionFlag = " << flag << ", runtime_error = " << error.what() << std::endl;
						closeConnection(clientIt);
					}
				}
			}
			if (FD_ISSET(socket, &_writeSet)) {
				if (flag == e_sendResponse)
					(this->*_funcMap.find(flag)->second)(*clientIt);
			}
		}
		if (getSignal() == SIGINT)
			break;
	}
	return (0);
}


