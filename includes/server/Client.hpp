#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <list>
#include "utils/Data.hpp"
#include "utils/utils.hpp"

class Client {

	public:
        typedef std::map<std::string, std::string> _headersType;
        typedef std::list<Client*> _clientsType;
        typedef _clientsType::iterator _clientIt;

		Client(int socket, int flag);
		~Client();

		int getSocket() const;
		int getFlag() const;
		const std::string &getHeaders() const;
        _headersType &getHeadersMap();
        const std::string &getBody() const;
		int getChunkMod() const;
		const std::string &getHexNum() const;
		long getSize() const;
		HttpStatusCode* getHttpStatusCode() const;

        void setSocket(int socket);
		void setFlag(int flag);
		void setHeaders(const std::string &header);
		void setBody(const std::string &body);
		void setChunkMod(int count);
		void setHexNum(const std::string &hexNum);
		void setSize(long size);
		void setHttpStatusCode(const HttpStatusCode &httpStatusCode);

		void appendHeader(std::string str);
		void appendBody(std::string str);
		void appendHexNum(std::string str);
		void wipeData();

	private:
		int _socket;
		int _flag;
		int _chunkMod;
		long _size;
		std::string _headers;
        _headersType _headersMap;
		std::string _body;
		std::string _hexNum;
		HttpStatusCode* _httpStatusCode;
};

#endif
