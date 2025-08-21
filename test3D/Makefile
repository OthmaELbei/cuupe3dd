CC      = cc
CFLAGS  = -Wall -Wextra -Werror #-fsanitize=address
RM      = rm -f
FILES=cup3d.c get_next_line.c  libft/ft_putstr_fd.c libft/ft_strdup.c libft/ft_strjoin.c libft/ft_strlcpy.c \
          libft/ft_strlen.c libft/ft_substr.c libft/ft_strchr.c libft/ft_split.c libft/free_2d_array.c \
          libft/ft_memset.c libft/ft_atoi.c libft/ft_count_2d_array.c libft/ft_strcmp.c libft/ft_isdigit.c \
		  libft/ft_strtrim.c libft/ft_lstnew.c libft/ft_lstaddback.c libft/ft_lstlast.c readmap.c 
  

OBJECT  = $(FILES:.c=.o)
NAME    = Cub3d

MLX_FLAGS = -lmlx -framework OpenGL -framework AppKit

all: $(NAME)

$(NAME): $(MLX) $(OBJECT)
	$(CC) $(CFLAGS) $(OBJECT) $(MLX_FLAGS) -o $(NAME)

mandatory/%.o: mandatory/%.c include/cub3d.h
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

libft/%.o: libft/%.c
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

$(MLX):
	$(MAKE) -C $(MLX_DIR)

clean:
	$(RM) $(OBJECT)

fclean: clean
	$(RM) $(NAME)

re: fclean all