#include <cstring>
#include "server/Server.hpp"

int main(int argc, char **argv) {
	if (argc == 2) {
		if (!strcmp(argv[1], "server")) {
			Server init;
			init.run_server();
		}
	}
	return (0);
}
