#include "parser/Server.hpp"

Server::Server() {

}

Server::~Server() {

}

void Server::parseConfiguration(const std::string &config) {
	static std::string buffer;
	static std::string serverBegin[] = {"server", "{"};
	static std::string locationBegin[] = {"location", "{"};
	static std::string end[] = {"}"};
	static std::string delim(" \t");
	static std::vector<std::string> serverVecBegin(serverBegin, serverBegin+sizeof(serverBegin)/sizeof(serverBegin[0]));
	static std::vector<std::string> locationVecBegin(locationBegin, locationBegin+sizeof(locationBegin)/sizeof(locationBegin[0]));
	static std::vector<std::string> endVec(end, end+sizeof(end)/sizeof(end[0]));
	static std::vector<std::string> splitBuffer;
	static int fd;

	fd = open(config.c_str(), O_RDONLY);
	if (fd < 0) {
		std::cerr << "Critical error - fd negative - parseMimeTypes" << std::endl;
		exit(1);
	}

	while (parseLine(fd, buffer) > 0) {
		splitBuffer = split(buffer, delim);
		if (splitBuffer == serverVecBegin) {
			while (parseLine(fd, buffer) > 0 && splitBuffer != endVec) {
				splitBuffer = split(buffer, delim);
				for (size_t i = 1; i < splitBuffer.size(); i++)
					_serverMap[splitBuffer[0]] = splitBuffer[i];
			}
		}
	}

	for (_serverMapIt it = _serverMap.begin(); it != _serverMap.end(); it++)
		std::cout << "first = " << it->first << ", second = " << it->second << std::endl;
}
