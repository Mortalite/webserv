#include "utils/utils.hpp"

/*
** Тип in_port_t - uint16_t - 2 байта - беззнаковый - от 0 до 65535
** Меняем местами первые 8 бит и последние 8 бит
*/
in_port_t ft_htons(in_port_t port) {
	return ((((((unsigned short)(port) & 0xFF)) << 8) | (((unsigned short)(port) & 0xFF00) >> 8)));
}

