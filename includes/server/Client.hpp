#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include "parser/Request.hpp"

class Client {

	private:
		int _socket;
		int _flag;
		int _chunkMod;
		size_t _size;
		std::string _header;
		std::string _hexNum;
		std::string _body;
		Request* _request;

public:
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

		void parseHeaders();
		void parseBody();

};

#endif
