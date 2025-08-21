/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelbied <oelbied@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 09:59:39 by oelbied           #+#    #+#             */
/*   Updated: 2025/08/21 11:25:49 by oelbied          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CUB3D_H1
#define CUB3D_H1

#include <mlx.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>

// #include "../minilibx-linux/mlx.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1
#endif

#define WIDIH 1200
#define HEIGHT 800
#define NUM_RAYS         WIDIH
// #define WINDOW_WIDTH    (MAP_NUM_COLS * TILE_SIZE)
// #define WINDOW_HEIGHT   (MAP_NUM_ROWS * TILE_SIZE)
#define EPSILON 1e-6
// #define MAP_NUM_ROWS    16
// #define MAP_NUM_COLS    34
#define TILE_SIZE       64
#define PLAYER_SIZE     8
#define FOV_ANGLE       (60 * (M_PI / 180))
typedef struct s_dataplayer
{
	double x;
	double y;
	double rotation_angle;
	double move_speed;
	double rotation_speed;
	int turn_direction;
	int walk_direction;
	int side_direction;


} t_dataplayer;
typedef struct s_image
{
	void *image;
	char *addr;
	int bits_per_pixel;
	int line_length;
	int endian;
} t_image;
typedef struct s_ray {
    double ray_angle;
    double wall_hit_x;
    double wall_hit_y;
    double distance;
    int was_hit_vertical;
    int wall_hit_content;
} t_ray;
typedef struct s_datagame
{
	void *mlx;
	void *window;
	t_image img;
	int show_minimap;
	t_dataplayer player;
	t_ray rays[NUM_RAYS];
	int colone;
	int row;
	char **map;
	int windo_with;
	int windo_hithe;
} t_datagame;
typedef struct	s_vmap
{
	char	*path;
	char	*str;
	char	*color;
	char	*north;
	char	*south;
	char	*west;
	char	*east;
	char	*floor;
	char	*ceil;
	int		n_floor;
	int		n_ceil;
}	t_vmap;

char	*get_next_line(int fd);
char **men(void);
/*=============libft (Start)============================*/
void	ft_putstr_fd(char *s, int fd);
int		ft_strlen(const char *s);
char	*ft_strdup(char *s1);
char	*ft_substr(char *s, unsigned int start, size_t len);
char	*ft_strjoin(char *s1, char *s2);
size_t	ft_strlcpy(char *dst, const char *src, size_t size);
char	*ft_strchr(char *s, char c);
char	**ft_split(char const *s, char *sep);
void	free_2d_array(char **arr);
int		ft_count_2d_array(char **arr);
void	*ft_memset(void *b, int c, size_t len);
int		ft_strcmp(char *s1, char *s2);
long	ft_atoi(char *str);
int		ft_isdigit(int c);
char	*ft_strtrim(char *s1, char *set);
t_vmap	*ft_lstnew(char *path);
void	ft_lstadd_back(t_vmap **lst, t_vmap *new);
t_vmap	*ft_lstlast(t_vmap *lst);
#endif