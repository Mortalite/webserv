#pragma once

#include <iostream>
#include <vector>
#include <stdint.h>
#include "utils/TraceException.hpp"

class Base64 {
public:
	static std::string encode(const std::string &data);
	static void encodeInPlace(std::string &data);
	static std::string decode(const std::string &input);
	static void decodeInPlace(std::string &data);
};