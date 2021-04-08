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
** Split, в цикле ищем разделитель, подстроку записываем в массив
** строк, запоминаем текущее значение + сдвиг на размер разделителя,
** если последняя строка пуста, значит тело запроса - пустое
*/
std::vector<std::string> split(const std::string& input, const std::string& delim) {
	std::vector<std::string> result;
	static std::string::size_type prev_pos;
    static std::string::size_type pos;
	static std::string header_delim = " \t";

    prev_pos = 0;
    pos = 0;
	while ((pos = input.find(delim, pos)) != std::string::npos) {
		result.push_back(trim(input.substr(prev_pos, pos - prev_pos), header_delim));
		prev_pos = pos++ + delim.length();
	}

	std::string last;
	if (!(last = input.substr(prev_pos)).empty())
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
