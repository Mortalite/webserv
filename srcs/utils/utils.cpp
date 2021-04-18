#include "utils/utils.hpp"

int		ft_strcmp(const char *s1, const char *s2)
{
	static size_t i;

	i = 0;
	while (s1[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

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
** Split, в цикле ищем разделитель, подстроку записываем в массив
** строк, запоминаем текущее значение + сдвиг на размер разделителя,
** если последняя строка пуста, значит тело запроса - пустое
*/
std::vector<std::string> split(const std::string& input, const std::string& delim) {
	std::vector<std::string> result;
	static std::string::size_type prevPos;
    static std::string::size_type pos;
	static std::string headerDelim(" \t");

	prevPos = 0;
    pos = 0;
	while ((pos = input.find(delim, pos)) != std::string::npos) {
		result.push_back(trim(input.substr(prevPos, pos - prevPos), headerDelim));
		prevPos = pos++ + delim.length();
	}

	std::string last;
	if (!(last = input.substr(prevPos)).empty())
		result.push_back(last);
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
** Исключения:
** substr - out_of_range, bad_alloc
** append - out_of_range, length_error, bad_alloc
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

size_t isLearYear(int year) {
	return (!(year % 4) && (year % 100 || !(year % 400)));
}

size_t yearSize(int year) {
	if (isLearYear(year))
		return (366);
	return (365);
}

struct tm *ft_gmtime(const time_t *timer) {
	static int year0 = 1900;
	static int epoch_year = 1970;
	static size_t secs_day = 24*60*60;
	static size_t _ytab[2][12] =
			{
					{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
					{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
			};
	static int year;
	static size_t dayclock;
	static size_t dayno;
	static struct tm *gmtime;

	gmtime = new struct tm;
	if (!gmtime)
		throw std::runtime_error("Allocation Failed: ft_gmtime");

	year = epoch_year;
	dayclock = (*timer) % secs_day;
	dayno = (*timer) / secs_day;

	gmtime->tm_sec = dayclock % 60;
	gmtime->tm_min = (dayclock % 3600) / 60;
	gmtime->tm_hour = dayclock / 3600;
	gmtime->tm_wday = (dayno + 4) % 7;
	while (dayno >= yearSize(year))
	{
		dayno -= yearSize(year);
		year++;
	}
	gmtime->tm_year = year - year0;
	gmtime->tm_yday = dayno;
	gmtime->tm_mon = 0;
	while (dayno >= _ytab[isLearYear(year)][gmtime->tm_mon])
	{
		dayno -= _ytab[isLearYear(year)][gmtime->tm_mon];
		gmtime->tm_mon++;
	}
	gmtime->tm_mday = dayno + 1;
	gmtime->tm_isdst = 0;
	return (gmtime);
}

std::string ft_ctime(const time_t* timer) {
	struct tm *gmtime;
	std::vector<char> result;

	result.reserve(100);
	gmtime = ft_gmtime(timer);
	strftime(&result[0], 100, "Date: %a, %d %b %Y %H:%M:%S GMT", gmtime);
	delete gmtime;
	return (&result[0]);
}

std::string currentTime() {
	static struct timeval timeval;
	static int ret;

	ret = gettimeofday(&timeval, NULL);
	if (ret == -1)
		throw std::runtime_error("Function gettimeofday failed: currentTime");
	return (ft_ctime(&timeval.tv_sec));
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
		if (ret == - 1)
			throw std::runtime_error("read fail");
	} while (ret > 0);

    close(fd);
    return (data);
}

/*
** Проверяю есть ли файл, потом типы
*/
bool isValidFile(const std::string &filename) {
	static struct stat filestat;
	static int ret;

	ret = stat(filename.c_str(), &filestat);
	if (ret == -1)
		return (false);
	switch (filestat.st_mode & S_IFMT) {
		case S_IFBLK:  return (false); // Block device
		case S_IFCHR:  return (false); // Character device
		case S_IFDIR:  return (false); // Directory
		case S_IFIFO:  return (true); // FIFO/PIPE
		case S_IFLNK:  return (true); // Symlink
		case S_IFREG:  return (true); // Regular file
		case S_IFSOCK: return (true); // Socket
		default:       return (false); // Unknown
	}
	return (false);
}