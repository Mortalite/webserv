#include "../../includes/server/Cgi.hpp"

int Cgi::make_env_var(Response & resp){
	std::map<std::string, std::string> env_var;
	const Data * data = resp.getData();
	std::vector<Server> serv = data->getServers();
	//как выбрать какой сервер сейчас работает?
	env_var["SERVER_NAME"] = serv[0]._serverName[0];
	const Client * client = resp.getClient();
	if (client->_hdrMap.find("content-length") != client->_hdrMap.end()){
		env_var["CONTENT_LENGTH"] = client->_hdrMap.find("content-length")->second;
	}
	return 0;
}

void Cgi::start_cgi(Response &resp) {
	make_env_var(resp);
}
