NAME = webserv

SRCS_DIR = ./srcs

SRCS_MAIN_DIR =
SRCS_MAIN = main.cpp

SRCS_PARSER_DIR = parser/
SRCS_PARSER =   Request.cpp\
				Response.cpp\
				Server.cpp

SRCS_SERVER_DIR = server/
SRCS_SERVER =   Manager.cpp\
                Client.cpp

SRCS_UTILS_DIR = utils/
SRCS_UTILS =    Data.cpp\
				HttpStatusCode.cpp\
                utils.cpp

SRCS =  $(addprefix $(SRCS_MAIN_DIR), $(SRCS_MAIN))\
		$(addprefix $(SRCS_PARSER_DIR), $(SRCS_PARSER))\
		$(addprefix $(SRCS_SERVER_DIR), $(SRCS_SERVER))\
		$(addprefix $(SRCS_UTILS_DIR), $(SRCS_UTILS))

RES_SRCS = $(addprefix $(SRCS_DIR), $(SRCS))

BUILD_DIR = ./build_dir
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.o))
DEPS = $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.d))

COMPILER = clang++
INCLUDES = -Iincludes
ifdef STRICT
FLAGS = -Wall -Werror -Wextra -std=c++98 -g3
else
FLAGS = -std=c++98 -g3
endif

$(BUILD_DIR)/%.o : $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(COMPILER) $(FLAGS) $(INCLUDES) -MMD -c $< -o $@

all:
	$(MAKE) $(NAME) -j

$(NAME): $(OBJS)
	$(COMPILER) $(FLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -rf $(BUILD_DIR)

fclean:
	$(MAKE) clean
	rm -f $(NAME)

re:
	$(MAKE) fclean
	$(MAKE) all

leak_sanitizer_address:
	$(MAKE) fclean
	$(COMPILER) $(FLAGS) $(INCLUDES) -fsanitize=address -g3 $(RES_SRCS) -o $(NAME)
	ASAN_OPTIONS=detect_leaks=1 ./${NAME}

leak_valgrind:
	$(MAKE) re
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./${NAME}

.PHONY: all clean fclean re

-include $(DEPS)
