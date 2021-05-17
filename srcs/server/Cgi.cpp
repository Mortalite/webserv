#include "server/Cgi.hpp"

/*
** Добавил указатель на отвечающий сервер и вывод его данных
*/
std::string Cgi::make_query_string(const Client * client){
	size_t found_pos;
	if (client->_hdrMap.find("request_target") != client->_hdrMap.end()){
		if ((found_pos = client->_hdrMap.find("request_target")->second.find_last_of("?")) != std::string::npos)
			return (std::string(client->_hdrMap.find("request_target")->second, found_pos));
	}
	return (std::string(""));
}

int Cgi::make_env_var(Response & resp){
	std::map<std::string, std::string> env_var;
	const Data * data = resp.getData();
	const Server* server = resp.getClient()->_respSvr;

//	std::vector<Server> serv = data->getServers();
	std::cout << *server << std::endl;
	const Client * client = resp.getClient();
	if (client->_hdrMap.find("authorization") != client->_hdrMap.end()){
		env_var["AUTH_TYPE"] = client->_hdrMap.find("authorization")->second;
	}
	//тут скорее всего нужно смотреть не заголовок, а длину сообщения, проверить
	if (client->_hdrMap.find("content-length") != client->_hdrMap.end()){
		env_var["CONTENT_LENGTH"] = client->_hdrMap.find("content-length")->second;
	}
//	CONTENT_TYPE
	env_var["GATEWAY_INTERFACE"] = "CGI/1.1";
	env_var["SERVER_NAME"] = server->_serverName[0];
//	тут проверить абсолютный/относительный путь
	if (client->_hdrMap.find("request_target") != client->_hdrMap.end()){
		env_var["PATH_INFO"] = env_var["REQUEST_URI"] = client->_hdrMap.find("request_target")->second;
	}
//	PATH_TRANSLATED
	env_var["QUERY_STRING"] = make_query_string(client);
	return 0;
}

void Cgi::start_cgi(Response &resp) {
	make_env_var(resp);
}
