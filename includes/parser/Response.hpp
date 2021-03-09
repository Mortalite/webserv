#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <vector>
#include <sys/time.h>
#include "utils/Data.hpp"

class Response {

	private:
		std::vector<char> timeBuffer;


	public:
		Response();
		~Response();

		size_t isLearYear(int year);
		size_t yearSize(int year);
		std::string getDate();
};

#endif
