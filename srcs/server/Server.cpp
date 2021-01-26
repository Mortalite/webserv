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

	content.insert(std::make_pair(max_sd, ""));

	while (true)
	{
		std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n" << std::endl;
		int ret;
//		struct timeval time;
//		time.tv_sec = 500;
//		time.tv_usec = 0;

		if ((ret = select(max_sd + 1, &read_set, NULL, NULL, 0)) == -1)
			strerror(errno);
		else if (ret == 0)
			std::cerr << "Time expired" << std::endl;

		for (std::map<int, std::string >::iterator i = content.begin(); i != content.end();) {
			const int& current_socket = (*i).first;
			std::string& current_str = (*i).second;

			std::cout << "current_socket = " << current_socket << std::endl;
			std::cout << "current_str = " << current_str << std::endl;

			if (FD_ISSET(current_socket, &read_set)) {
				if (current_socket == listen_sd) {
					std::cout << "Read socket" << std::endl;

					while (true) {
						if ((new_socket = accept(listen_sd, (struct sockaddr *) &address, &addrlen)) == -1) {
							strerror(errno);
							break;
						}
						else {
							FD_SET(new_socket, &read_set);
							content.insert(std::make_pair(new_socket, ""));
						}
					}
					i++;
				} else {
					while (true) {
						char buffer[4096] = {0};
						valread = read(current_socket, buffer, 10);
						std::cout << "valread = " << valread << std::endl;
						if (valread > 0)
							current_str.append(buffer);
						if (valread == 0) {
							Request request;

							FD_CLR(current_socket, &read_set);
							close(current_socket);

							request.parseRequest(current_str.c_str());
							content.erase(i++);
							break;
						}
					}
				}
			}
		}
	}
	return (0);
}
