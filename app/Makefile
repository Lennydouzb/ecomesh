C=c++
CFLAGS = -Wall -Werror -Wextra -g -std=c++20
NAME = EcoMesh
SRCS = srcs/main.cpp \
		srcs/parser.cpp
OBJ_DIR = .build/
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(C) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)%.o:%.cpp
	@mkdir -p $(dir $@)
	$(C) $(CFLAGS) $< -c -o $@

fclean: clean
	rm -f $(NAME)

clean:
	rm -rf $(OBJ_DIR)

re: fclean all

.PHONY: all fclean clean re