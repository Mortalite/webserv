#include <cstring>
#include "server/Server.hpp"

int main(int argc, char **argv) {
	if (argc == 2) {
		if (!strcmp(argv[1], "server")) {
			Server init;
			init.runServer();
		}
	}

/*	for (int i = 40; i < 46; i++) {
		std::cout << "![Socket-" << i << "](images/" << i << ".png)" << std::endl;
	}*/
	return (0);
}
