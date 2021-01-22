#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "utils/utils.hpp"

#define PORT 8080

int serverSide() {
	struct sockaddr_in address;
	int server_fd, new_socket, addrlen = sizeof(address);
	long valread;
	std::string hello("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!");

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("In socket");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = ft_htons( PORT );
	memset(address.sin_zero, '\0', sizeof(address.sin_zero));

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("In bind");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 10) < 0)
	{
		perror("In listen");
		exit(EXIT_FAILURE);
	}
	while(1)
	{
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
		{
			perror("In accept");
			exit(EXIT_FAILURE);
		}

		char buffer[30000] = {0};
		valread = read(new_socket, buffer,30000);
		printf("%s\n",buffer);
		write(new_socket , hello.c_str(), strlen(hello.c_str()));
		printf("------------------Hello message sent-------------------\n");
		close(new_socket);
	}
	return (0);
}

int clientSide() {
	int sock = 0;
	long valread;
	struct sockaddr_in serv_addr;
	std::string hello("Hello from client");
	char buffer[1024] = {0};

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return (-1);
	}

	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if ((serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1")) <= 0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return (-1);
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return (-1);
	}
	send(sock, hello.c_str(), strlen(hello.c_str()), 0);
	printf("Hello message sent\n");
	valread = read(sock, buffer, 1024);
	printf("%s\n",buffer);
	return (0);
}

int main(int argc, char **argv) {
	if (argc == 2) {
		if (!strcmp(argv[1], "server"))
			serverSide();
		else if (!strcmp(argv[1], "client"))
			clientSide();
	}

/*	for (int i = 40; i < 46; i++) {
		std::cout << "![Socket-" << i << "](images/" << i << ".png)" << std::endl;
	}*/
	return (0);
}
