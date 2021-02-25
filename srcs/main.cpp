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
			Server init;

			signalMain();
			
			std::cout << "Enter Ctrl-C to exit" << std::endl;
			init.runServer();
		}
	}
	return (0);
}
