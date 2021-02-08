#include "server/Server.hpp"

int Server::_signal = 0;

static void	signal_ctrl_c(int signal)
{
	if (signal == SIGINT)
	{
		Server::setSignal(signal);
		std::cout << "\b\bClosing webserv!" << std::endl;
	}
}

void signal_main() {
	signal(SIGINT, signal_ctrl_c);
}

int main(int argc, char **argv) {
	if (argc == 2) {
		if (!strcmp(argv[1], "server")) {
			signal_main();
			Server init;
			init.runServer();
		}
	}
	return (0);
}
