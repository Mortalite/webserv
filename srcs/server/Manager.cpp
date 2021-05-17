#include "server/Manager.hpp"

/*
** Создаю объекты для обработки запроса, в ассоциативный массив добавляю пару - (флаг, функция)
*/
Manager::Manager(Data *data) {
	_data = data;
	_request = new Request(_data);
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
										_server(other._server),
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
	if (ft::getDebug())
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
	if (client->_flag == e_sendResponse)
		FD_SET(client->_socket, &_writeSet);
	else
		FD_SET(client->_socket, &_readSet);
}

int Manager::launchManager() {
	static Client* client;
	const std::string *host;
	socklen_t addrlen = sizeof(_address);
	static int 	acceptSocket,
				maxSocket = -1,
				clientSocket,
				clientFlag,
				tmp,
				listenSocket,
				reuse = 1;
	static long port;
	static timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	for (Server::_svrsType::const_iterator it = _server->begin(); it != _server->end(); it++) {
		port = (*it)._listenPort;
		host = &(*it)._host;

		_address.sin_family = AF_INET;
		_address.sin_addr.s_addr = *host == "localhost" ? INADDR_ANY : inet_addr((*host).c_str());
		_address.sin_port = htons(port);
		memset(_address.sin_zero, '\0', sizeof(_address.sin_zero));

		if (((listenSocket) = socket(AF_INET, SOCK_STREAM, 0)) == -1 ||
			((fcntl(listenSocket, F_SETFL, O_NONBLOCK)) == -1) ||
			(setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) ||
			(bind(listenSocket, (struct sockaddr *)&_address, sizeof(_address)) < 0) ||
			(listen(listenSocket, SOMAXCONN) < 0)) {
			strerror(errno);
		}
		_clients.push_back(new Client(&*it, listenSocket, e_listenSocket));
	}

	while (true)
	{
		FD_ZERO(&_readSet);
		FD_ZERO(&_writeSet);

		for (Client::_clientIt clientIt = _clients.begin(); clientIt != _clients.end();) {
			if ((*clientIt)->_flag == e_closeConnection)
				closeConnection(clientIt);
			else
				clientIt++;
		}

		for (Client::_clientIt clientIt = _clients.begin(); clientIt != _clients.end(); clientIt++) {
			tmp = (*clientIt)->_socket;
			maxSocket = std::max(tmp, maxSocket);
			initSet(*clientIt);
		}
		
		if ((tmp = select(maxSocket + 1, &_readSet, &_writeSet, NULL, &timeout)) == -1)
			continue;

		for (Client::_clientIt clientIt = _clients.begin(); clientIt != _clients.end(); clientIt++) {
			client = (*clientIt);
			clientSocket = client->_socket;
			clientFlag = client->_flag;

			if (clientFlag == e_listenSocket) {
				if (FD_ISSET(clientSocket, &_readSet)) {
					if ((acceptSocket = accept(clientSocket, (struct sockaddr *)&_address, &addrlen)) == -1)
						continue;
					else {
						_clients.push_back(new Client(client->_acptSvr, acceptSocket, e_recvHeaders));
						if (fcntl(acceptSocket, F_SETFL, O_NONBLOCK) == -1)
							_clients.back()->_flag = e_closeConnection;
					}
				}
			}
			else {
				if (FD_ISSET(clientSocket, &_readSet) ||
					FD_ISSET(clientSocket, &_writeSet)) {
					try {
						(this->*_funcMap.find(clientFlag)->second)(*clientIt);
					}
					catch (HttpStatusCode &httpStatusCode) {
						client->_httpStatusCode = httpStatusCode;
						client->_flag = e_sendResponse;
					}
					catch (std::exception &exception) {
						std::cout << RED << "EXCEPTION: " << exception.what() << RESET << std::endl;
						std::cout << RED <<  strerror(errno) << RESET << std::endl;
						client->_flag = e_closeConnection;
					}
				}
			}
		}
		if (getSignal() == SIGINT)
			break;
	}
	return (0);
}


