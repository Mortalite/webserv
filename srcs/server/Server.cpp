#include "server/Server.hpp"

Server::Server() {
}

Server::~Server() {

}

int Server::runServer() {
	int reuse = 1, listen_sd, new_socket;
	socklen_t addrlen = sizeof(address);
	long valread;
	std::string hello("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!");

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = ft_htons( PORT );
	memset(address.sin_zero, '\0', sizeof(address.sin_zero));

	// Creating socket file descriptor
	if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ||\
		((fcntl(listen_sd, F_SETFL, O_NONBLOCK)) == -1) ||\
		(setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) ||\
		(bind(listen_sd, (struct sockaddr *)&address, sizeof(address)) < 0) ||\
		(listen(listen_sd, 10) < 0)) {
		strerror(errno);
	}

	FD_ZERO(&read_set);
	max_sd = listen_sd;
	FD_SET(listen_sd, &read_set);

	while (true)
	{
		std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n" << std::endl;
		struct timeval time;
		int ret;
		time.tv_sec = 500;
		time.tv_usec = 0;

		if ((ret = select(max_sd + 1, &read_set, NULL, NULL, &time)) == -1)
			strerror(errno);
		else if (ret == 0)
			std::cerr << "Time expired" << std::endl;

		for (int i = 0; i <= max_sd; i++) {
			if (FD_ISSET(i, &read_set)) {
				if (i == listen_sd) {
					std::cout << "Read socket" << std::endl;

					if ((new_socket = accept(listen_sd, (struct sockaddr *)&address, &addrlen)) == -1) {
						strerror(errno);
					}
					else {
						Request request;
						char buffer[30000] = {0};
						valread = read(new_socket, buffer, 30000);
						request.parseRequest(buffer);
						write(new_socket, hello.c_str(), hello.length());
						printf("------------------Hello message sent-------------------\n");
						close(new_socket);
					}
				}
			}
		}

	}
	return (0);
}
