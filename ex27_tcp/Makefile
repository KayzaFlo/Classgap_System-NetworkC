#####	CONFIG		############################################################
SERVER		:= server
CLIENT		:= client
CC			:= gcc
AR			:= ar -rcs
RM			:= rm -f
CFLAGS		= -Wall -Wextra -Werror

#####	SOURCES		############################################################
SRC_SERVER	=	server.c
SRC_CLIENT	=	client.c
OBJ_SERVER	= $(SRC_SERVER:.c=.o)
OBJ_CLIENT	= $(SRC_CLIENT:.c=.o)

#####	Makefile  objs		###################################################
WHITE		:= \033[0m
RED			:= \033[1;31m
GREEN		:= \033[1;32m
YELLOW		:= \033[1;33m
BLUE		:= \033[1;34m
PINK		:= \033[1;35m
CYAN 		:= \033[1;36m

.c.o:
	@ echo "$(YELLOW)Compiling: $(WHITE)$<"
	@ ${CC} ${CFLAGS} -c $< -o ${<:.c=.o}

all: $(SERVER) $(CLIENT)

$(SERVER): $(OBJ_SERVER)
	@ echo "$(GREEN)Compilation ${WHITE}of ${CYAN}$(SERVER) ${WHITE}..."
	@ $(CC) -o $(SERVER) $(OBJ_SERVER) -lm
	@ echo "$(CYAN)$(SERVER) $(GREEN)created$(WHITE) ✔️"

$(CLIENT): $(OBJ_CLIENT)
	@ echo "$(GREEN)Compilation ${WHITE}of ${CYAN}$(CLIENT) ${WHITE}..."
	@ $(CC) -o $(CLIENT) $(OBJ_CLIENT) -lm
	@ echo "$(CYAN)$(CLIENT) $(GREEN)created$(WHITE) ✔️"

clean:
	@ ${RM} $(OBJ_SERVER) $(OBJ_CLIENT) $(OBJ_BONUS)
	@ echo "$(RED)Deleting $(CYAN)$(NAME) $(WHITE)objs ✔️"

fclean: clean
	@ ${RM} $(SERVER) $(CLIENT)
	@ echo "$(RED)Deleting $(CYAN)$(NAME) $(WHITE)binary ✔️"

re: fclean all

rebonus: fclean bonus

.PHONY:		all clean fclean re