#include "server/Server.hpp"

/*
** Буффер размером 1 мб, с помощью reserve, выделяем память
*/
Server::Server() {
	BODY_BUFFER = 1024*1024*1024;
	_buffer.reserve(BODY_BUFFER + 1);
}

Server::~Server() {
	for (Client::_clientsType::iterator it = _clients.begin(); it != _clients.end(); it++)
		removeClient(it);
}

int& Server::getSignal() {
	static int signal = 0;
	return (signal);
}

void Server::removeClient(Client::_clientsType::iterator& it) {
	close((*it)->getSocket());
	delete *it;
	_clients.erase(it++);
}

int Server::recvHeaders(Client::_clientsType::iterator& it) {
	Client* client = (*it);

	long valread = recv(client->getSocket(), &_buffer[0], 1, 0);
	if (valread > 0) {
		_buffer[valread] = '\0';
		client->appendHeader(&_buffer[0]);
	}

	if (ft::isLastEqual(client->getHeader(), "\r\n\r\n")) {
		client->parseHeaders();

		std::pair<int, long> pairType = client->getRequest()->getBodyType();
		client->setFlag(pairType.first);
		client->setSize(pairType.second);
	}
	return (0);
}

int Server::recvBody(Client::_clientsType::iterator& it) {
	Client* client = (*it);
	static size_t size;

	size = client->getSize() + 2;
	if (size > BODY_BUFFER) {
		_buffer.resize(size + 1);
		BODY_BUFFER = size;
	}

	long valread = recv(client->getSocket(), &_buffer[0], size, 0);
	if (valread > 0) {
		_buffer[valread] = '\0';
		client->appendBody(&_buffer[0]);
	}
	else if (valread == 0) {
		client->parseBody();
		removeClient(it);
	}
	return (0);
}

int Server::recvChunkedBody(Client::_clientsType::iterator& it) {
	Client* client = (*it);
	int chunkMod = client->getChunkMod();
	long valread;

	if (chunkMod == ft::e_chunk_data) {
		static size_t size = client->getSize() + 2;
		if (size > BODY_BUFFER) {
			_buffer.resize(size + 1);
			BODY_BUFFER = size;
		}

		valread = recv(client->getSocket(), &_buffer[0], size, 0);
		if (valread > 0) {
			_buffer[valread] = '\0';
			client->appendBody(&_buffer[0]);
		}

		client->setChunkMod(ft::e_chunk_hex);
	}
	else {
		std::string headers_delim = "\r\n";

		valread = recv(client->getSocket(), &_buffer[0], 1, 0);
		if (valread > 0) {
			_buffer[valread] = '\0';
			client->appendHexNum(&_buffer[0]);
		}

		if (ft::isLastEqual(client->getHexNum(), "\r\n")) {
 			std::string trim = ft::trim(client->getHexNum(), headers_delim);

 			char *ptr;
			size_t result = strtol(&(trim[0]), &ptr, 16);
			if (!(*ptr))
				client->setSize(result);
			else {
				/*
				** Ошибка
				*/
			}
			client->setChunkMod(ft::e_chunk_data);
			client->setHexNum("");
		}
	}

	if (valread == 0) {
		client->parseBody();
		removeClient(it);
	}

	return (0);
}

int Server::runServer() {
	int reuse = 1, listen_sd, new_socket, max_sd = -1, tmp;
	socklen_t addrlen = sizeof(_address);
	std::string hello("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!");

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
	_clients.push_back(new Client(listen_sd, 0, "", ""));

	while (true)
	{
		/*
		** В цикле каждый раз, перед вызовом select его нужно заново инициализировать,
		** так как он затирает данные после вызова, timeout означает, что select
		** будет ожидать готовности одного из сокетов(типа как поток ожидает mutex_lock)
		*/
		FD_ZERO(&_readSet);
		for (Client::_clientsType::iterator current_it = _clients.begin(); current_it != _clients.end(); current_it++) {
			tmp = (*current_it)->getSocket();
			max_sd = std::max(tmp, max_sd);
			FD_SET(tmp, &_readSet);
		}

		if ((tmp = select(max_sd + 1, &_readSet, NULL, NULL, 0)) == -1)
			strerror(errno);
		else if (tmp == 0)
			std::cerr << "Time expired" << std::endl;

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
		for (Client::_clientsType::iterator it = _clients.begin(); it != _clients.end(); it++) {
			const int& socket = (*it)->getSocket();
			const int& flag = (*it)->getFlag();

			if (FD_ISSET(socket, &_readSet)) {
				if (socket == listen_sd) {
					if ((new_socket = accept(listen_sd, (struct sockaddr *) &_address, &addrlen)) == -1) {
						strerror(errno);
						continue;
					}
					else {
						_clients.push_back(new Client(new_socket, ft::e_headers, "", ""));
						max_sd = std::max(new_socket, max_sd);
					}
				}
				else {
/*					int flags[3] = {ft::e_headers, ft::e_content_body, ft::e_chunked_body};
					int (Server::*fcnPtr[3])(_clientsType::iterator&) = { 	&Server::recvHeaders,\
																			&Server::recvBody,\
																			&Server::recvChunkedBody};*/
					switch (flag) {
						case ft::e_headers:
							recvHeaders(it);
							break;
						case ft::e_content_body:
							recvBody(it);
							break;
						case ft::e_chunked_body:
							recvChunkedBody(it);
							break;
					}
				}
			}
		}
		if (getSignal() == SIGINT)
			break;
	}
	return (0);
}
