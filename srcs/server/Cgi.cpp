#include "../../includes/server/Cgi.hpp"


int Cgi::make_env_var(Response & resp){
//	std::vector<Server> serv = resp._data->getServers();
	const Data * data = resp.getData();
	std::vector<Server> serv = data->getServers();
	//как выбрать какой сервер сейчас работает?
	serv[0]._serverName;
	return 0;
}