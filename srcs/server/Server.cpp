#include "server/Server.hpp"

Server::Server() {
}

Server::~Server() {

}

int Server::runServer() {
	int reuse = 1, listen_sd, new_socket, max_sd = -1;
	socklen_t addrlen = sizeof(address);
	long valread;
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
	content.insert(std::make_pair(listen_sd, ""));

	int count = 0;
	while (true)
	{
		std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n" << std::endl;
		int tmp;

		/*
		** В цикле каждый раз, перед вызовом select его нужно заново инициализировать,
		** так как он затирает данные после вызова, timeout означает, что select
		** будет ожидать готовности одного из сокетов(типа как поток ожидает mutex_lock)
		*/
		FD_ZERO(&read_set);
		for (std::map<int, std::string >::iterator i = content.begin(); i != content.end(); i++) {
			tmp = (*i).first;
			if (tmp > max_sd)
				max_sd = tmp;
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
		for (std::map<int, std::string >::iterator i = content.begin(); i != content.end(); i++) {
			const int& current_socket = (*i).first;
			std::string& current_str = (*i).second;

			std::cout << "current_socket = " << current_socket << std::endl;
			std::cout << "current_str = " << current_str << std::endl;

			if (FD_ISSET(current_socket, &read_set)) {
				if (current_socket == listen_sd) {
					std::cout << "Read socket" << std::endl;

						if ((new_socket = accept(listen_sd, (struct sockaddr *) &address, &addrlen)) == -1) {
							strerror(errno);
							continue;
						}
						else {
							content.insert(std::make_pair(new_socket, ""));
							if (new_socket > max_sd)
								max_sd = new_socket;
						}
				}
				else {
						char buffer[4096] = {0};
						valread = read(current_socket, buffer, 10);
						if (valread > 0)
							current_str.append(buffer);
						if (valread == 0) {
							count++;
							std::cout << "count = " << count << std::endl;
							Request request;
							close(current_socket);

							request.parseRequest(current_str.c_str());
							content.erase(i++);

							break;
						}
				}
			}
		}
	}
	return (0);
}
