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
    std::cout << argv[0] << std::endl;
    if (argc == 1) {
        Data data;
        Server server(&data);

        signalMain();

        std::cout << "Enter Ctrl-C to exit" << std::endl;
        server.runServer();
    }
	else if (argc == 2) {

	}
	else {
	    std::cout << "Invalid arguments count. Try again" << std::endl;
	}
	return (0);
}
