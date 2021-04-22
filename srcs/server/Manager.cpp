#include "server/Manager.hpp"

/*
** Буффер размером 1 мб, с помощью reserve, выделяю память,
** в ассоциативный массив добавляю пару - (флаг, функция)
*/
Manager::Manager(Data* data) {
	_data = data;
	_request = new Request();
	_response = new Response(_data);
	_funcMap.insert(std::make_pair(e_recvHeaders, &Manager::recvHeaders));
	_funcMap.insert(std::make_pair(e_recvContentBody, &Manager::recvContentBody));
	_funcMap.insert(std::make_pair(e_recvChunkBody, &Manager::recvChunkBody));
	_funcMap.insert(std::make_pair(e_sendResponse, &Manager::sendResponse));
	_funcMap.insert(std::make_pair(e_closeConnection, &Manager::closeConnection));
}

/*
** В случае закрытия сервера, при Ctrl-C очищаю всех клиентов
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

Response *Manager::getRequest() {
    return (_response);
}

void Manager::setData(Data* data) {
	_data = data;
}

/*
** Закрываю сокет, удаляю Client* и удаляю из списка
*/
void Manager::closeConnection(Client::_clientIt &clientIt) {
	close((*clientIt)->getSocket());
	delete *clientIt;
	_clients.erase(clientIt++);
	std::cout << "_clients.size() = " << _clients.size() << std::endl;
}

/*
** Читаю заголовки по 1 символу, если это конец заголовков "\r\n\r\n",
** то обрабатываю их, если что-то не так, то бросается\ловится исключение,
** и сохраняется значение кода ошибки
*/
void Manager::recvHeaders(Client::_clientIt &clientIt) {
	_request->recvHeaders(clientIt);
}

/*
** Читаю тело с заголовком content-length, потом распечатываю
*/
void Manager::recvContentBody(Client::_clientIt &clientIt) {
	_request->recvContentBody(clientIt);
}

void Manager::recvChunkBody(Client::_clientIt &clientIt) {
	_request->recvChunkBody(clientIt);
}

void Manager::sendResponse(Client::_clientIt &clientIt) {
    _response->sendResponse(clientIt);
}

void Manager::initSet(Client::_clientIt &clientIt) {
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

int Manager::runManager() {
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
						continue;
					}
					else {
						_clients.push_back(new Client(new_sd, e_recvHeaders));
						max_sd = std::max(new_sd, max_sd);
                        fcntl(new_sd, F_SETFL, O_NONBLOCK);
					}
				}
				else {
					try {
						(this->*_funcMap.find(flag)->second)(clientIt);
					}
					catch (HttpStatusCode& httpStatusCode) {
						static Client* client;

						client = (*clientIt);
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
				if (socket != listen_sd)
					(this->*_funcMap.find(flag)->second)(clientIt);
			}

		}
		if (getSignal() == SIGINT)
			break;
	}
	return (0);
}
