#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>
#include <fcntl.h>
#include "utils/utils.hpp"

class Server {

public:
	typedef std::map<std::string, std::string> _serverMapType;
	typedef _serverMapType::const_iterator _serverMapIt;

	Server();
	Server(const Server& other);
	~Server();

	Server& operator=(const Server& other);


	Server parseServer(int fd);
	std::vector<Server> parseConfiguration(const std::string& config = "config/webserv.conf");


private:

	class Pattern {
	private:
		Pattern();
		~Pattern();

		template <typename T, typename M, size_t arrayLength>
		static bool isEqual(const T (&array)[arrayLength], M& vec) {
			static size_t patternSize;

			patternSize = sizeof(array)/sizeof(array[0]);
			if (patternSize == vec.size())
				return (std::equal(vec.begin(), vec.end(), array));
			return (false);
		}

	public:
		enum PATTERN_FLAG {
			e_server,
			e_location,
			e_end
		};

		static bool matchPattern(int flag, std::vector<std::string> vec) {
			static std::string serverPattern[] = {"server", "{"};
			static std::string locationPattern[] = {"location", "{"};
			static std::string endPattern[] = {"}"};

			switch (flag) {
				case e_server:
					return (isEqual(serverPattern, vec));
				case e_location:
					return (isEqual(locationPattern, vec));
				case e_end:
					return (isEqual(endPattern, vec));
				default:
					std::cerr << "Critical error - matchPattern failed" << std::endl;
					exit(1);
			}
		}

	};

	std::vector<std::string> _splitBuffer;
	std::string _buffer;
	std::string _delim;

	long _maxFileSize;
	std::string _host;
	std::vector<std::string> _serverName;
	std::string _root;
	std::string _autoindex;
	_serverMapType _serverMap;

};

#endif