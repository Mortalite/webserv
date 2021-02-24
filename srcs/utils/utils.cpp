#include "utils/utils.hpp"

namespace ft {

	/*
	** Тип in_port_t - uint16_t - 2 байта - беззнаковый - от 0 до 65535
	** 8080 (10 десятичная) - 0001 1111 1001 0000 (двоичная)
	** (0001 1111 1001 0000 * 0000 0000 1111 1111 << 8) | (0001 1111 1001 0000 * 1111 1111 0000 0000 >> 8)
	** 1001 0000 0000 0000 | 0000 0000 0001 1111 = 1001 0000 0001 1111 =
	** = 2^15 + 2^12 + 2^4 + 2^3 + 2^2 + 2^1 + 2^0 = 36895
	*/
	in_port_t htons(in_port_t port) {
		return (((((port & 0xFF)) << 8) | ((port & 0xFF00) >> 8)));
	}

	int inSet(const char &character, const std::string &delim) {
		for (std::string::size_type i = 0; i < delim.size(); i++)
			if (character == delim[i])
				return (1);
		return (0);
	}

	std::string trim(const std::string &input, const std::string &delim) {
		std::string::const_iterator begin = input.begin();
		std::string::const_reverse_iterator rbegin = input.rbegin();

		while (begin != input.end() && inSet(*begin, delim))
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
	std::vector<std::string> split(const std::string &input, const std::string &delim) {
		std::vector<std::string> result;
		std::string::size_type prev_pos = 0, pos = 0;
		std::string header_delim = " \t";

		while ((pos = input.find(delim, pos)) != std::string::npos) {
			result.push_back(trim(input.substr(prev_pos, pos - prev_pos), header_delim));
			prev_pos = pos++ + delim.length();
		}

		std::string last;
		if (!(last = input.substr(prev_pos)).empty())
			result.push_back(last);
		return (result);
	}

	std::string &toLower(std::string &input) {
		for (std::string::size_type i = 0; i < input.size(); i++)
			input[i] = tolower(input[i]);
		return (input);
	}

	int isLastEqual(const std::string &str, const std::string &extension) {
		std::string::size_type str_size = str.size(), ext_size = extension.size();

		if (str_size > ext_size) {
			for (std::vector<char>::size_type i = 0; i < ext_size; i++) {
				if (str[str_size - 1 - i] != extension[ext_size - 1 - i])
					return (0);
			}
			return (1);
		}
		return (0);
	}
}