#include "server/Server.hpp"

Server::Server() {}

Server::~Server() {}

int Server::recv_headers(clients_type::iterator& it) {
	char buffer[2] = {0};
	Client* client = (*it);

	long valread = recv(client->getSocket(), buffer, 1, 0);
	if (valread > 0)
		client->setHeader(client->getHeader() + buffer);

	if (client->getHeader().rfind("\r\n\r\n") != std::string::npos) {
		client->getRequest()->parse_headers(client->getHeader());
		client->setFlag(e_content_body);
	}
	return (0);
}

int Server::recv_body(clients_type::iterator& it) {
	char buffer[BODY_BUFFER] = {0};
	Client* client = (*it);

	long valread = recv(client->getSocket(), buffer, BODY_BUFFER, 0);
	if (valread > 0)
		client->setBody(client->getBody() + buffer);
	else if (valread == 0) {
		close(client->getSocket());
		client->getRequest()->parse_body(client->getBody());
		_clients.erase(it++);
		return (1);
	}
	return (0);
}

int Server::run_server() {
	int reuse = 1, listen_sd, new_socket, max_sd = -1;
	socklen_t addrlen = sizeof(address);
	std::string hello("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!");

	/*
	** Записываем в структуру следующие параметры:
	** семейство адресов (AF_INET - IPv4)
	** адрес сокета (INADDR_ANY - 0.0.0.0)
	** адрес порта в сетевом виде (8080 - 36895)
	*/
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = ft_htons( PORT );
	memset(address.sin_zero, '\0', sizeof(address.sin_zero));

	/*
	** Создаю сокет, ставлю режим прослушивания на неблокирующий,
	** так же ставлю флаг для переиспользования сокета, связываю со структурой,
	** и начинаю прослушивать
	*/
	if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ||\
		((fcntl(listen_sd, F_SETFL, O_NONBLOCK)) == -1) ||\
		(setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) ||\
		(bind(listen_sd, (struct sockaddr *)&address, sizeof(address)) < 0) ||\
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
		int tmp;

		/*
		** В цикле каждый раз, перед вызовом select его нужно заново инициализировать,
		** так как он затирает данные после вызова, timeout означает, что select
		** будет ожидать готовности одного из сокетов(типа как поток ожидает mutex_lock)
		*/
		FD_ZERO(&read_set);
		for (clients_type::iterator current_it = _clients.begin(); current_it != _clients.end(); current_it++) {
			tmp = (*current_it)->getSocket();
			max_sd = std::max(tmp, max_sd);
			FD_SET(tmp, &read_set);
		}

		if ((tmp = select(max_sd + 1, &read_set, NULL, NULL, NULL)) == -1)
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
		for (clients_type::iterator it = _clients.begin(); it != _clients.end(); it++) {
			const int& socket = (*it)->getSocket();
			const int& flag = (*it)->getFlag();

			if (FD_ISSET(socket, &read_set)) {
				if (socket == listen_sd) {
					if ((new_socket = accept(listen_sd, (struct sockaddr *) &address, &addrlen)) == -1) {
						strerror(errno);
						continue;
					}
					else {
						_clients.push_back(new Client(new_socket, e_headers, "", ""));
						max_sd = std::max(new_socket, max_sd);
					}
				}
				else {
					if (flag == e_headers)
						recv_headers(it);
					else if (flag == e_content_body) {
						if (recv_body(it))
							break;
					}
				}
			}
		}
	}
	return (0);
}

