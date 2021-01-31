#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

class Client {

	private:
public:


private:
		int _socket;
		int _flag;
		std::string _header;
		std::string _body;


public:
		Client(int socket, int flag, std::string header, std::string body);
		~Client();

		int getSocket() const;
		int getFlag() const;
		const std::string &getHeader() const;
		const std::string &getBody() const;

		void setSocket(int socket);
		void setFlag(int flag);
		void setHeader(const std::string &header);
		void setBody(const std::string &body);


};

#endif
