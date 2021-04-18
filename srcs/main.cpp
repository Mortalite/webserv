#include "server/Manager.hpp"

static void	signalCtrlC(int signal)
{
	if (signal == SIGINT)
	{
		Manager::getSignal() = signal;
		std::cout << "\b\bClosing webserv!" << std::endl;
	}
}

void signalMain() {
	signal(SIGINT, signalCtrlC);
}

int main(int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-d"))
			Response::getDebug() = 1;
	}

	Data data;
	data.parseMimeTypes();
	Manager server(&data);

	signalMain();

	std::cout << "Enter Ctrl-C to exit" << std::endl;
	server.runManager();

	return (0);
}
