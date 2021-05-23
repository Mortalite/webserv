#include "server/Manager.hpp"

int main(int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-d"))
			ft::getDebug() = 1;
	}

	Manager::launchManager();
	return (0);
}
