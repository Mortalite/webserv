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

//	Server sl;
//	sl.parseConfiguration();

	Data data;
	data.parseMimeTypes();
	Manager manager(&data);

	signalMain();

	std::cout << "Enter Ctrl-C to exit" << std::endl;
	manager.runManager();

	return (0);
}
