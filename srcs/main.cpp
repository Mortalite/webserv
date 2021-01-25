#include <iostream>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "utils/utils.hpp"
#include "server/Server.hpp"

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
		if (!strcmp(argv[1], "server")) {
			Server init;
			init.runServer();
		}
		else if (!strcmp(argv[1], "client"))
			clientSide();
	}


/*	for (int i = 40; i < 46; i++) {
		std::cout << "![Socket-" << i << "](images/" << i << ".png)" << std::endl;
	}*/
	return (0);
}
