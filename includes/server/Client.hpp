#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <list>
#include "utils/Data.hpp"
#include "parser/Request.hpp"

class Client {

	public:
		typedef std::list<Client*> _clientsType;

		Client(Data* data, int socket, int flag);
		~Client();

		int getSocket() const;
		int getFlag() const;
		const std::string &getHeader() const;
		const std::string &getBody() const;
		Data *getData() const;
		Request* getRequest() const;
		HttpStatusCode* getHttpStatusCode() const;
		int getChunkMod() const;
		const std::string &getHexNum() const;
		long getSize() const;

		void setSocket(int socket);
		void setFlag(int flag);
		void setHeader(const std::string &header);
		void setBody(const std::string &body);
		void setChunkMod(int count);
		void setHexNum(const std::string &hexNum);
		void setSize(long size);

		void appendHeader(std::string str);
		void appendBody(std::string str);
		void appendHexNum(std::string str);
		void parseHeaders();
		void parseBody();

	private:
		int _socket;
		int _flag;
		int _chunkMod;
		long _size;
		std::string _header;
		std::string _body;
		std::string _hexNum;
		Data* _data;
		HttpStatusCode* _httpStatusCode;
		Request* _request;
};

#endif
