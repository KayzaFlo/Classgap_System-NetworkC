#####	CONFIG		############################################################
SERVER		:= server
CLIENT		:= client
CC			:= gcc
AR			:= ar -rcs
RM			:= rm -f
CFLAGS		= -Wall -Wextra -Werror

#####	SOURCES		############################################################
SRC_SERVER	=	exmp1_server.c
OBJ_SERVER	= $(SRC_SERVER:.c=.o)

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

all: $(SERVER)

$(SERVER): $(OBJ_SERVER)
	@ echo "$(GREEN)Compilation ${WHITE}of ${CYAN}$(SERVER) ${WHITE}..."
	@ $(CC) -o $(SERVER) $(OBJ_SERVER) -lm
	@ echo "$(CYAN)$(SERVER) $(GREEN)created$(WHITE) ✔️"

clean:
	@ ${RM} $(OBJ_SERVER) $(OBJ_BONUS)
	@ echo "$(RED)Deleting $(CYAN)$(NAME) $(WHITE)objs ✔️"

fclean: clean
	@ ${RM} $(SERVER)
	@ echo "$(RED)Deleting $(CYAN)$(NAME) $(WHITE)binary ✔️"

re: fclean all

rebonus: fclean bonus

.PHONY:		all clean fclean re