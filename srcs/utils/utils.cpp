#include "utils/utils.hpp"

/*
** Тип in_port_t - uint16_t - 2 байта - беззнаковый - от 0 до 65535
** Меняем местами байты
*/
in_port_t ft_htons(in_port_t port) {
	return ((((((unsigned short)(port) & 0xFF)) << 8) | (((unsigned short)(port) & 0xFF00) >> 8)));
}

/*
** Split, в цикле ищем разделитель, подстроку записываем в массив
** строк, запоминаем текущее значение + сдвиг на размер разделителя,
** если последняя строка пуста, значит тело запроса - пустое
*/

std::vector<std::string> split(const std::string& input, const std::string& delim) {
	std::vector<std::string> result;
	std::string::size_type prev_pos = 0, pos = 0;

	while ((pos = input.find(delim, pos)) != std::string::npos) {
		result.push_back(input.substr(prev_pos, pos - prev_pos));
		prev_pos = pos++ + delim.length();
	}

	std::string last;
	if (!(last = input.substr(prev_pos)).empty())
		result.push_back(last);
	return (result);
}