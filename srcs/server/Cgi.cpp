#include "server/Cgi.hpp"

/*
** Добавил указатель на отвечающий сервер и вывод его данных
*/
int Cgi::make_env_var(Response &resp) {
	const Data * data = resp.getData();
	const Server* server = resp.getClient()->_respSvr;

	std::vector<Server> serv = data->getServers();
	std::cout << *server << std::endl;
	return 0;
}