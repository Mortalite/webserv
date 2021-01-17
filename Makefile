NAME = webserv
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

COMPILER = clang++
FLAGS = -Wall -Werror -Wextra -std=c++98
INCLUDES = -I.

%.o : %.cpp
	$(COMPILER) $(FLAGS) $(INCLUDES) -MMD -c $< -o $@

$(NAME): $(OBJS)
	$(COMPILER) $(FLAGS) $(OBJS) -o $(NAME)

all: $(NAME)

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)

fclean:
	$(MAKE) clean
	rm -f $(NAME)

re:
	$(MAKE) fclean
	$(MAKE) all

leak_sanitizer_address:
	$(MAKE) fclean
	$(COMPILER) $(FLAGS) $(INCLUDES) -fsanitize=address -g3 $(SRCS) -o $(NAME)
	ASAN_OPTIONS=detect_leaks=1 ASAN_OPTIONS=atexit=1 ./${NAME}

leak_valgrind:
	$(MAKE) re
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./${NAME}

.PHONY: all clean fclean re

-include $(DEPS)