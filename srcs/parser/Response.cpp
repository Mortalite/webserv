#include "parser/Response.hpp"

Data &Response::getData() {
	static Data data;
	return (data);
}
