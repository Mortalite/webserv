NAME = webserv

SRCS_MAIN_DIR = ./srcs/
SRCS_MAIN = main.cpp

SRCS_SERVER_DIR = ./srcs/server/
SRCS_SERVER = Server.cpp

SRCS_UTILS_DIR = ./srcs/utils/
SRCS_UTILS = utils.cpp

RES_SRCS =  $(addprefix $(SRCS_MAIN_DIR), $(SRCS_MAIN))\
            $(addprefix $(SRCS_SERVER_DIR), $(SRCS_SERVER))\
            $(addprefix $(SRCS_UTILS_DIR), $(SRCS_UTILS))

OBJS = $(RES_SRCS:.cpp=.o)
DEPS = $(RES_SRCS:.cpp=.d)

COMPILER = clang++
FLAGS = -Wall -Werror -Wextra -std=c++98
INCLUDES = -Iincludes

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
	$(COMPILER) $(FLAGS) $(INCLUDES) -fsanitize=address -g3 $(RES_SRCS) -o $(NAME)
	ASAN_OPTIONS=detect_leaks=1 ASAN_OPTIONS=atexit=1 ./${NAME}

leak_valgrind:
	$(MAKE) re
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./${NAME}

.PHONY: all clean fclean re

-include $(DEPS)