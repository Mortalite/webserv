#include "utils/utils.hpp"

/*
** Ищет символ в строке
*/
int inSet(const char &character, const std::string &delim) {
	static std::string::size_type i;

	for (i = 0; i < delim.size(); i++)
		if (character == delim[i])
			return (1);
	return (0);
}

/*
** Убирает все символы delim с начала и с конца строки string
*/
std::string trim(const std::string &string, const std::string &delim) {
	static std::string::const_iterator begin;
	static std::string::const_reverse_iterator rbegin;

	begin = string.begin();
	rbegin = string.rbegin();
	while (begin != string.end() && inSet(*begin, delim))
		begin++;

	while (rbegin.base() != begin && inSet(*rbegin, delim))
		rbegin++;

	return (std::string(begin, rbegin.base()));
}

/*
** Split, находит первый символ отличный от разделителя, потом
** следующий - разделитель, извелекает строку, если последняя
** строка пуста, значит тело запроса - пустое
*/
std::vector<std::string> split(const std::string& input, const std::string& delim) {
	std::vector<std::string> result;
	static std::string::size_type prevPos;
	static std::string::size_type pos;
	static std::string headerDelim(" \t");

	prevPos = 0;
	pos = 0;
	while (true) {
		if ((prevPos = input.find_first_not_of(delim, pos)) == std::string::npos ||\
		(pos = input.find_first_of(delim, prevPos+1)) == std::string::npos)
			break;
		result.push_back(trim(input.substr(prevPos, pos - prevPos), headerDelim));
	}

	if (prevPos != std::string::npos) {
		static std::string last;

		if (!(last = input.substr(prevPos)).empty())
			result.push_back(last);
	}
	return (result);
}

/*
** Приводит строку к нижнему регистру
*/
std::string &toLower(std::string& string) {
	static std::string::size_type i;

	for (i = 0; i < string.size(); i++)
		string[i] = tolower(string[i]);
	return (string);
}

/*
**  Проверяет последние символы строки
*/
int isLastEqual(const std::string& string, const std::string& extension) {
	static std::string::size_type strSize;
	static std::string::size_type extSize;

	strSize = string.size();
	extSize = extension.size();
	if (strSize > extSize) {
		for (std::string::size_type i = 0; i < extSize; i++) {
			if (string[strSize - 1 - i] != extension[extSize - 1 - i])
				return (0);
		}
		return (1);
	}
	return (0);
}

/*
** Если последний символ строки не \r или \n, то читаем 4 байта из сокета
*/
int readHeaderSize(const std::string& string) {
	static std::string::size_type strSize;

	strSize = string.size();
	if (!string.empty()) {
		if (!inSet(string[strSize - 1], "\r\n"))
			return (4);
	}
	return (1);
}

/*
** GNL, читает из файлов с CRLF и LF.
*/
int getNextLine(int fd, std::string &line) {
	static size_t BUFFER_SIZE = 8192;
	static std::map<int, std::string> lineBuffer;
	static std::map<int, std::string>::iterator lineBufferIt;
	static std::string::size_type pos;
	static std::string::size_type offset;
	static std::vector<char> buffer;
	static int ret;

	if (fd < 0)
		return (-1);

	buffer.reserve(BUFFER_SIZE);
	if (lineBuffer.find(fd) == lineBuffer.end())
		lineBuffer[fd];

	lineBufferIt = lineBuffer.find(fd);
	do {
		pos = lineBufferIt->second.find_first_of("\n");
		if (pos != std::string::npos) {
			offset = 0;
			if (pos > 0 && lineBufferIt->second[pos - 1] == '\r')
				offset = 1;
			line = lineBufferIt->second.substr(0, pos-offset);
			lineBufferIt->second = lineBufferIt->second.substr(pos+1);
			return (1);
		}
		else {
			ret = read(fd, &buffer[0], BUFFER_SIZE);
			buffer[ret] = '\0';
			lineBufferIt->second.append(&buffer[0]);
		}
	} while (ret);

	if (ret == 0) {
		line.swap(lineBufferIt->second);
		lineBuffer.erase(lineBufferIt);
	}
	return (ret);
}

std::string convertTime(const time_t& time) {
	static struct tm* tm;
	static std::vector<char> buffer;

	tm = gmtime(&time);
	buffer.reserve(100);
	strftime(&buffer[0], 100, "%a, %d %b %Y %H:%M:%S GMT", tm);
	return (&buffer[0]);
}

std::string currentTime() {
	static struct timeval timeval;
	static int ret;

	ret = gettimeofday(&timeval, NULL);
	if (ret == -1)
		throw std::runtime_error("gettimeofday failed");
	return (convertTime(timeval.tv_sec));
}

/*
** Читаю файл.
*/
std::string readFile(const std::string &filename) {
	static size_t BUFFER_SIZE = 8192;
	static std::vector<char> buffer;
	static int fd;
	std::string data;
	int ret = 0;

	fd = open(filename.c_str(), O_RDONLY);
	if (fd < 0)
		throw std::runtime_error("open fail");

	buffer.reserve(BUFFER_SIZE);
	do {
		data.append(&buffer[0], ret);
		ret = read(fd, &buffer[0], BUFFER_SIZE);
		if (ret == -1)
			throw std::runtime_error("read fail");
	} while (ret > 0);

	close(fd);
	return (data);
}

int parseLine(int fd, std::string &buffer) {
	static std::string::size_type cmdEnd;
	static int ret;

	ret = getNextLine(fd, buffer);
	if (ret == -1) {
		std::cout << "Critical error - ret negative - parseLine" << std::endl;
		exit(1);
	}
	else if (ret == 0)
		return (0);
	cmdEnd = buffer.find_last_of(';');
	if (cmdEnd != std::string::npos)
		buffer.erase(cmdEnd);
	return (1);
}

