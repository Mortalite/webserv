#include "server/Manager.hpp"

/*
** Создаю объекты для обработки запроса, в ассоциативный массив добавляю пару - (флаг, функция)
*/
Manager::Manager(Data* data) {
	_data = data;
	_request = new Request();
	_response = new Response(_data);
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
	std::cout << "close = " << (*clientIt)->getSocket() << std::endl;
	close((*clientIt)->getSocket());
	delete *clientIt;
	_clients.erase(clientIt++);
}

/*
** Читаю заголовки
*/
void Manager::recvHeaders(Client::_clientIt &clientIt) {
	_request->recvHeaders(clientIt);
}

/*
** Читаю тело с заголовком content-length
*/
void Manager::recvContentBody(Client::_clientIt &clientIt) {
	_request->recvContentBody(clientIt);
}

/*
** Читаю тело с заголовком transfer-encoding = chunk
*/
void Manager::recvChunkBody(Client::_clientIt &clientIt) {
	_request->recvChunkBody(clientIt);
}

/*
** Отправляю ответ
*/
void Manager::sendResponse(Client::_clientIt &clientIt) {
	_response->sendResponse(clientIt);
}

/*
** В зависимости от флага закрываю соединение, либо добавляю
** сокет в набор, который при выполнении select
** проверяет готовность сокета на чтение/запись.
*/
void Manager::initSet(Client::_clientIt &clientIt) {
	static int flag;

	flag = (*clientIt)->getFlag();
	if (flag == e_recvHeaders ||
		flag == e_recvContentBody ||
		flag == e_recvChunkBody)
		FD_SET((*clientIt)->getSocket(), &_readSet);
	else if (flag == e_sendResponse)
		FD_SET((*clientIt)->getSocket(), &_writeSet);

}

int Manager::runManager() {
	int reuse = 1;
	int listenSocket;
	int newSocket;
	int maxSocket = -1;
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
	** адрес порта в сетевом виде
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
	if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1 ||
		((fcntl(listenSocket, F_SETFL, O_NONBLOCK)) == -1) ||
		(setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) ||
		(bind(listenSocket, (struct sockaddr *)&_address, sizeof(_address)) < 0) ||
		(listen(listenSocket, SOMAXCONN) < 0)) {
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
	_clients.push_back(new Client(listenSocket, e_recvHeaders));
	while (true)
	{
		/*
		** В цикле каждый раз, перед вызовом select его нужно заново инициализировать,
		** так как он затирает данные после вызова.
		*/
		FD_ZERO(&_readSet);
		FD_ZERO(&_writeSet);

		/*
		** Убираю клиентов, которым послал ответ
		*/
		for (Client::_clientIt clientIt = _clients.begin(); clientIt != _clients.end();) {
			if ((*clientIt)->getFlag() == e_closeConnection)
				closeConnection(clientIt);
			else clientIt++;
		}

		/*
		** Оставшихся клиентов добавляю в наборы
		*/
		for (Client::_clientIt clientIt = _clients.begin(); clientIt != _clients.end(); clientIt++) {
			tmp = (*clientIt)->getSocket();
			maxSocket = std::max(tmp, maxSocket);
			initSet(clientIt);
		}
		/*
		** timeout означает, что select будет ожидать указанное время (я поставил (0,0) он будет постоянно
		** пробегаться по всем сокетам и проверять их)
		*/
		if ((tmp = select(maxSocket + 1, &_readSet, &_writeSet, NULL, &timeout)) == -1)
			strerror(errno);
//		else if (tmp == 0)
//			std::cerr << "Time expired" << std::endl;

		/*
		** В цикле прохожу по всем сокетам(1 - для приёма соединений, остальные - для получения/отправки),
		** если сокет готов для чтения, то для 1 сокета я принимаю соединение, то есть создается новый сокет,
		** я добавляю его в список с пустыми данными и вычисляю максимальный сокет, иначе получаю данные
		** из сокета, если сокет готов к отправке данных - отправляю.
		*/
		for (Client::_clientIt clientIt = _clients.begin(); clientIt != _clients.end(); clientIt++) {
			static Client* client;
			static int socket;
			static int flag;

			client = (*clientIt);
			socket = client->getSocket();
			flag = client->getFlag();

			if (FD_ISSET(socket, &_readSet)) {
				if (socket == listenSocket) {
					if ((newSocket = accept(listenSocket, (struct sockaddr *) &_address, &addrlen)) == -1) {
						continue;
					}
					else {
						_clients.push_back(new Client(newSocket, e_recvHeaders));
						maxSocket = std::max(newSocket, maxSocket);
						fcntl(newSocket, F_SETFL, O_NONBLOCK);
					}
				}
				else {
					try {
						(this->*_funcMap.find(flag)->second)(clientIt);
					}
					catch (HttpStatusCode& httpStatusCode) {
						client->setHttpStatusCode(httpStatusCode);
						client->setFlag(e_sendResponse);
					}
					catch (std::runtime_error& error) {
						std::cout << "functionFlag = " << flag << ", runtime_error = " << error.what() << std::endl;
						closeConnection(clientIt);
					}
				}
			}
			if (FD_ISSET(socket, &_writeSet)) {
				if (socket != listenSocket)
					(this->*_funcMap.find(flag)->second)(clientIt);
			}
		}
		if (getSignal() == SIGINT)
			break;
	}
	return (0);
}
