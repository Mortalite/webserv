#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <list>
#include "parser/Request.hpp"

class Client {

	public:
		typedef std::list<Client*> _clientsType;

		Client(int socket, int flag, std::string header, std::string body);
		~Client();

		int getSocket() const;
		int getFlag() const;
		const std::string &getHeader() const;
		const std::string &getBody() const;
		Request *getRequest() const;
		int getChunkMod() const;
		const std::string &getHexNum() const;
		size_t getSize() const;

		void setSocket(int socket);
		void setFlag(int flag);
		void setHeader(const std::string &header);
		void setBody(const std::string &body);
		void setChunkMod(int count);
		void setHexNum(const std::string &hexNum);
		void setSize(size_t size);

		void appendHeader(std::string str);
		void appendBody(std::string str);
		void appendHexNum(std::string str);
		void parseHeaders();
		void parseBody();

	private:
		int _socket;
		int _flag;
		int _chunkMod;
		size_t _size;
		std::string _header;
		std::string _body;
		std::string _hexNum;
		Request* _request;

};

#endif
