#include "utils/utils.hpp"

namespace ft {

	std::string tolower(std::string string) {
		std::transform(string.begin(), string.end(), string.begin(), ::tolower);
		return (string);
	}

	std::string toupper(std::string string) {
		std::transform(string.begin(), string.end(), string.begin(), ::toupper);
		return (string);
	}

	std::string trim(const std::string &string, const std::string &delim) {
		std::string result = string;
		result.erase(result.find_last_not_of(delim) + 1);
		result.erase(0, result.find_first_not_of(delim));
		return (result);
	}

	std::vector<std::string> split(const std::string &input, const std::string &delim) {
		std::vector<std::string> result;
		static std::string::size_type prevPos;
		static std::string::size_type pos;

		prevPos = 0;
		pos = 0;
		while (true) {
			if ((prevPos = input.find_first_not_of(delim, pos)) == std::string::npos ||
				(pos = input.find_first_of(delim, prevPos + 1)) == std::string::npos)
				break;
			result.push_back(trim(input.substr(prevPos, pos - prevPos), delimConfig));
		}

		if (prevPos != std::string::npos) {
			static std::string last;

			if (!(last = input.substr(prevPos)).empty())
				result.push_back(last);
		}
		return (result);
	}

	bool isStartWith(const std::string &string, const std::string &extension) {
		if (extension.size() > string.size())
			return (false);
		return (std::equal(extension.begin(), extension.end(), string.begin()));
	}

	bool isEndWith(const std::string &string, const std::string &extension) {
		if (extension.size() > string.size())
			return (false);
		return (std::equal(extension.rbegin(), extension.rend(), string.rbegin()));
	}

	int readHeaderSize(const std::string &string) {
		if (!isEndWith(string, "\r") && !isEndWith(string, "\n"))
			return (4);
		return (1);
	}

	int getNextLine(int fd, std::string &line) {
		static size_t BUFFER_SIZE = 8192;
		static std::map<int, std::string> lineBuffer;
		static std::map<int, std::string>::iterator lineBufferIt;
		static std::string::size_type pos;
		static std::string::size_type offset;
		static std::vector<char> buffer;
		static int ret;

		if (fd == -1)
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
				line = lineBufferIt->second.substr(0, pos - offset);
				lineBufferIt->second = lineBufferIt->second.substr(pos + 1);
				return (1);
			} else {
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

	std::string convertTime(const time_t &time) {
		static struct tm *tm;
		static std::vector<char> buffer;

		tm = gmtime(&time);
		buffer.reserve(200);
		strftime(&buffer[0], 100, "%a, %d %b %Y %H:%M:%S GMT", tm);
		return (&buffer[0]);
	}

	std::string currentTime() {
		static struct timeval timeval;
		static int ret;

		ret = gettimeofday(&timeval, NULL);
		if (ret == -1)
			TraceException("gettimeofday failed");
		return (convertTime(timeval.tv_sec));
	}

	std::string readFile(int fd) {
		std::vector<char> buffer;
		std::string data;
		int ret = 0;

		buffer.reserve(bufferSize);
		do {
			data.append(&buffer[0], ret);
			if ((ret = read(fd, &buffer[0], bufferSize)) == -1)
				TraceException("read failed");
		} while (ret > 0);
		if (close(fd) == -1)
			TraceException("close failed");
		return (data);
	}

	std::string readFile(const std::string &filename) {
		int fd;

		if ((fd = open(filename.c_str(), O_RDONLY, S_IRUSR)) == -1)
			TraceException("open failed");
		return (readFile(fd));
	}

	int parseLine(int fd, std::string &buffer) {
		static std::string::size_type cmdEnd;
		static int ret;

		ret = getNextLine(fd, buffer);
		if (ret == -1)
			TraceException("getNextLine failed");
		else if (ret == 0)
			return (0);
		cmdEnd = buffer.find_last_of(';');
		if (cmdEnd != std::string::npos)
			buffer.erase(cmdEnd);
		return (1);
	}

	bool matchPattern(int flag, std::vector<std::string> vec) {
		static std::string serverPattern[] = {"server", "{"};
		static std::string locationPattern[] = {"location", "*", "{"};
		static std::string mimeTypesPattern[] = {"types", "{"};
		static std::string endPattern[] = {"}"};

		switch (flag) {
			case pattern::e_server:
				return (isEqual(serverPattern, vec));
			case pattern::e_location:
				return (isEqual(locationPattern, vec));
			case pattern::e_mime:
				return (isEqual(mimeTypesPattern, vec));
			case pattern::e_end:
				return (isEqual(endPattern, vec));
			default:
				TraceException("matchPattern failed");
		}
	}

	long strToLong(const std::string &string, int base) {
		long result;
		char *ptr;

		result = std::strtol(string.c_str(), &ptr, base);
		if (string.c_str() == ptr || (*ptr) || result == LONG_MAX || result == LONG_MIN)
			TraceException("strtol failed");
		return (result);
	}

	int &getDebug() {
		static int debug = 0;
		return (debug);
	}

	void getFileInfo(const std::string &filename, TargetInfo &targetInfo) {
		static int ret;
		static int type;
		static struct stat fileStat;

		ret = stat(filename.c_str(), &fileStat);
		if (ret == -1) {
			targetInfo._type = filetype::e_file_type_error;
			targetInfo._stat = fileStat;
			targetInfo._size = "0";
			targetInfo._lstMod = "0";
			return;
		}
		switch (fileStat.st_mode & S_IFMT) {
			case S_IFBLK:
				type = filetype::e_invalid;
				break; // Block device
			case S_IFCHR:
				type = filetype::e_invalid;
				break; // Character device
			case S_IFDIR:
				type = filetype::e_directory;
				break; // Directory
			case S_IFIFO:
				type = filetype::e_valid;
				break; // FIFO/PIPE
			case S_IFLNK:
				type = filetype::e_valid;
				break; // Symlink
			case S_IFREG:
				type = filetype::e_valid;
				break; // Regular file
			case S_IFSOCK:
				type = filetype::e_valid;
				break; // Socket
			default:
				type = filetype::e_invalid;
				break; // Unknown
		}
		targetInfo._type = type;
		targetInfo._stat = fileStat;
		targetInfo._size = valueToString(fileStat.st_size);
		targetInfo._lstMod = convertTime(fileStat.st_mtime);
	}

	void getStringInfo(const std::string &string, TargetInfo &targetInfo) {
		targetInfo._type = filetype::e_valid;
		targetInfo._size = valueToString(string.size());
		targetInfo._body = string;
	}

	std::string getcwd() {
		char buffer[PATH_MAX + 1];

		if (!::getcwd(buffer, PATH_MAX + 1))
			TraceException("getcwd failed");
		return (buffer);
	}

}
