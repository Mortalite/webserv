#include "server/Server.hpp"

static void	signalCtrlC(int signal)
{
	if (signal == SIGINT)
	{
		Server::getSignal() = signal;
		std::cout << "\b\bClosing webserv!" << std::endl;
	}
}

void signalMain() {
	signal(SIGINT, signalCtrlC);
}

int main(int argc, char **argv) {
	if (argc == 2) {
		if (!strcmp(argv[1], "server")) {
			Data data;
			Server server;

			signalMain();
			
			std::cout << "Enter Ctrl-C to exit" << std::endl;
			server.setData(&data);
//			std::cout << ft::getDate() << std::endl;
			//server.runServer();
		}
	}
	return (0);
}
