/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cup3d.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelbied <oelbied@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 11:48:02 by oelbied           #+#    #+#             */
/*   Updated: 2025/08/21 11:25:14 by oelbied          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/cub3d.h"

void 	my_mlx_pixel_put(t_image *img,int x,int y,int color)
{
	char *dst;
	  if (x >= 0 && x < WIDIH && y >= 0 && y < HEIGHT) 
	{
       dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
        *(unsigned int*)dst = color;
    }
}
void draw_pixel(t_image *img, int x, int y, int width, int height, int color)
{
	int i = 0;
	int j = 0;
	while (i < width)
	{
		j =0;
		while (j < height)
		{
			if(x + i >= 0 && x + i < WIDIH && y + j >= 0 && y + j < HEIGHT)
			my_mlx_pixel_put(img,x+i,y+j, color);
			j++;
		}
		i++;
	}
	
}

void draw_sercel_player(t_image *img,int conter_x,int conter_y, int radius , int color)
{
	int x = -radius;
	int y = -radius;
	int px;
	int py;
	while (y <= radius)
	{
		x = -radius;
		while (x <= radius)
		{
			if((x * x) + (y * y) <= radius * radius)
			{
				px = conter_x + x;
				py = conter_y + y;
				if(px >= 0 && px < WIDIH && py >= 0 && py < HEIGHT)
				{
					my_mlx_pixel_put(img,px,py, color);
				}
			}	
			x++;
		}
		y++;
	}
	
}

void draw_line(t_image *img,int x0,int y0,int x1,int y1,int color)
{
	int dx = abs(x1 - x0);
	int dy = -abs(y1 - y0);
	int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1) {
        my_mlx_pixel_put(img, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

double normalize_angle(double angle) {
    angle = fmod(angle, 2 * M_PI); // this function about if  angol ecoual negative number roturn this number  to a 2p you while a pestef number 
    if (angle < 0) 
		angle += (2 * M_PI);
    return angle;
}
int has_wall_at(t_datagame *game, double x, double y) {
    if (x < 0 || x >= game->windo_with || y < 0 || y >= game->windo_hithe)
        return 1;

    int map_x = (int)(x / TILE_SIZE);
    int map_y = (int)(y / TILE_SIZE);

    if (map_y < 0 || map_y >= game->row|| map_x < 0 || map_x >= game->colone)
        return 1;

    return (game->map[map_y][map_x] == '1');
}
void cast_ray(t_datagame *game, double ray_angle, int ray_id) {
    // ray_angle = normalize_angle(ray_angle);

    t_ray *ray = &game->rays[ray_id];
    ray->ray_angle = ray_angle;
    ray->distance = INFINITY;
    ray->was_hit_vertical = 0;
    ray->wall_hit_content = '0';

 
    double rayDirX = cos(ray_angle);
    double rayDirY = sin(ray_angle);

 
    double posX = game->player.x / TILE_SIZE;
    double posY = game->player.y / TILE_SIZE;


    int mapX = (int)posX;
    int mapY = (int)posY;

    double deltaDistX = fabs(1 / rayDirX);
    double deltaDistY = fabs(1 / rayDirY);

    double sideDistX;
    double sideDistY;

    int stepX;
    int stepY;

    if (rayDirX < 0) {
        stepX = -1;
        sideDistX = (posX - mapX) * deltaDistX;
    } else {
        stepX = 1;
        sideDistX = (mapX + 1.0 - posX) * deltaDistX;
    }
    if (rayDirY < 0) {
        stepY = -1;
        sideDistY = (posY - mapY) * deltaDistY;
    } else {
        stepY = 1;
        sideDistY = (mapY + 1.0 - posY) * deltaDistY;
    }

    int hit = 0;
    int side = 0;


    while (!hit) {
        if (sideDistX < sideDistY) {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = 0;
        } else {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = 1; 
        }
    
        if (mapY >= 0 && mapY < game->row &&
            mapX >= 0 && mapX < game->colone &&
            game->map[mapY][mapX] == '1') {
            hit = 1;
        }
    }

    double perpWallDist;
    if (side == 0)
        perpWallDist = (sideDistX - deltaDistX) * TILE_SIZE;
    else
        perpWallDist = (sideDistY - deltaDistY) * TILE_SIZE;

    ray->distance = perpWallDist;
    ray->wall_hit_x = game->player.x + cos(ray_angle) * perpWallDist;
    ray->wall_hit_y = game->player.y + sin(ray_angle) * perpWallDist;
    ray->was_hit_vertical = (side == 0);
    ray->wall_hit_content = '1';
}

void cast_all_rays(t_datagame *game) {
    double ray_angle ;
    int i = 0; 
	
	ray_angle = game->player.rotation_angle - (FOV_ANGLE / 2);
    while (i < NUM_RAYS)
	{
        cast_ray(game, normalize_angle(ray_angle), i);
        ray_angle += FOV_ANGLE / NUM_RAYS;
		i++;
    }
}
void make_recst_troiud(t_datagame *game)
{
	draw_pixel(&game->img ,0,0,WIDIH,HEIGHT /2 ,0x87CEEB);
	draw_pixel(&game->img ,0,HEIGHT /2,WIDIH,HEIGHT /2 ,0x8B4513);
	t_ray *ray;
	double corrected_disn;
	double tall_wall;
	int i = 0;
	int wall_top;
	int wall_bottom;
	int base_color;
	double max_distanc;

	double shade ;
	int color;
	int strip_width;
	while (i < NUM_RAYS)
	{
		ray = &game->rays[i];
		if(ray->distance == INFINITY)
			continue;
		 corrected_disn = ray->distance * cos(ray->ray_angle - game->player.rotation_angle);
		if(corrected_disn < EPSILON)
			corrected_disn = EPSILON;
		tall_wall = (TILE_SIZE / corrected_disn) * ((WIDIH / 2) / tan(FOV_ANGLE /2));
		 wall_top = (HEIGHT /2) - (tall_wall /2);
		if(wall_top < 0)
			wall_top = 0;
		wall_bottom = (HEIGHT /2) + (tall_wall /2);
		if(wall_bottom >HEIGHT)
			wall_bottom = HEIGHT;
		base_color = 0;
		if(base_color == ray->was_hit_vertical)
			base_color = 0xFF6BBB;	
		else
			base_color = 0x4ECDC4;
			
		max_distanc = TILE_SIZE * 10;
		shade = 1.0 - fmin(ray->distance / max_distanc,0.7);
		shade = fmax(shade,0.2);
		color = ((int)((base_color >> 16 & 0xFF)* shade) << 16) |
					((int)((base_color >> 8 & 0xFF)* shade) << 8)|
					((int)((base_color  & 0xFF)* shade));
		strip_width = (WIDIH / NUM_RAYS) > 0 ? (WIDIH / NUM_RAYS) : 1;
		
	 draw_pixel(&game->img ,i * strip_width,wall_top,strip_width,wall_bottom - wall_top ,color);
		i++;
	}
	
	
}

void render(t_datagame *game)
{
	  memset(game->img.addr, 0, HEIGHT * game->img.line_length);
	  make_recst_troiud(game);
     mlx_put_image_to_window(game->mlx, game->window, game->img.image, 0, 0);
}

 void movemone_plyr(t_datagame *game)
 {  
	double moveing_step;
 	double oder_step;
 	double oder_x;
 	double oder_y;

 		moveing_step = game->player.walk_direction * game->player.move_speed;
 		oder_step = game->player.side_direction * game->player.move_speed;
		oder_x =game->player.x;
		oder_y =game->player.y;

	if(moveing_step != 0)
	{
		oder_x += cos(game->player.rotation_angle ) * moveing_step;
		oder_y += sin(game->player.rotation_angle) * moveing_step;
	}
	if(oder_step != 0)
	{
		oder_x += cos(game->player.rotation_angle + (M_PI / 2)) * oder_step;
		oder_y += sin(game->player.rotation_angle * (M_PI / 2))* oder_step;
	}
	double sonter_player = PLAYER_SIZE /2;

	if(!has_wall_at(game,oder_x + sonter_player,game->player.y) && 
	!has_wall_at(game,oder_x - sonter_player,game->player.y))
	{	
		game->player.x = oder_x;
	}
	if(!has_wall_at(game,game->player.x,oder_y + sonter_player) && 
	!has_wall_at(game,game->player.x,oder_y - sonter_player))
	{
		
	game->player.y = oder_y;
	}

	if(game->player.turn_direction != 0)
	{
		game->player.rotation_angle += game->player.turn_direction * game->player.rotation_speed;
		game->player.rotation_angle = normalize_angle(game->player.rotation_angle);
	}

 }


int key_walking( int key,t_datagame *game)
{
	if(key == 53)
	{
		mlx_destroy_image(game->mlx,game->img.image);
		mlx_destroy_window(game->mlx,game->window);
		exit(0);
	}
	else if(key == 13 || key == 126)
		game->player.walk_direction = 1;
	else if(key == 1 || key == 125)
		game->player.walk_direction = -1;
	else if(key == 0)
		game->player.side_direction = -1;
	else if(key == 2)
		game->player.side_direction = 1;
	else if(key == 123)
		game->player.turn_direction = -1;
	else if(key == 124)
		game->player.turn_direction = 1;
	else if(key == 46)
		game->show_minimap = !game->show_minimap;
	return 0;
}
int key_relass(int key, t_datagame *game)
{
	if(key == 13 || key == 126)
		game->player.walk_direction = 0;
	else if(key == 1 || key == 125)
		game->player.walk_direction = 0;
	else if(key == 0)
		game->player.side_direction = 0;
	else if(key == 2)
		game->player.side_direction = 0;
	else if(key == 123)
		game->player.turn_direction = 0;
	else if(key == 124)
		game->player.turn_direction = 0;

	return 0;
}
int game_loop(t_datagame *game)
{
	 movemone_plyr(game);
	   cast_all_rays(game);
	  render(game);
	  return 0;
}


void declare_game(t_datagame *allgame,char **maps)
{

	int x = 0;
	int y = 0;
	allgame->mlx = mlx_init();
	allgame->window = mlx_new_window(allgame->mlx, WIDIH, HEIGHT, "Cup3D");
	allgame->img.image = mlx_new_image(allgame->mlx, WIDIH, HEIGHT);
	allgame->img.addr = mlx_get_data_addr(allgame->img.image, &allgame->img.bits_per_pixel, &allgame->img.line_length, &allgame->img.endian);

	allgame->player.x = WIDIH / 2;
	allgame->player.y = HEIGHT / 2;
	//  = M_PI / 2;
	allgame->player.move_speed = 3.0;
	allgame->player.rotation_speed = 3 * (M_PI_2 / 180);
	allgame->player.turn_direction = 0;
	allgame->player.walk_direction = 0;
	allgame->player.side_direction = 0;
    
	allgame->map=maps;


	    for (int k = 0; maps[k]; k++)
        printf("%s", maps[k]);
	
    
	
	int flags =0;
	while ( maps[y])
	{
		x = 0;
		while ( maps[y][x] != '\0' )
		{
			allgame->map[y][x] = maps[y][x];
			x++;
			
		}
		if(flags < x)
		{
			flags = x;
		}
		// printf("%s",allgame->map[y++]);
		y++;
	}
		// printf("%s++",allgame->map[1]);
	
	allgame->row = y;
	allgame->colone = flags;
	allgame->windo_with = allgame->colone * TILE_SIZE;
	allgame->windo_hithe = allgame->row  * TILE_SIZE;
	

		int i = 0;
		int  j = 0;
	while (maps[i])
	{
		j = 0;
		while (maps[i][j])
		{
			if (maps[i][j] == 'N' || maps[i][j] == 'S' || maps[i][j] == 'E' || maps[i][j] == 'W')
			{
				if (maps[i][j] == 'N')
					allgame->player.rotation_angle = 3 * M_PI / 2; // Facing North (up)
				else if (maps[i][j] == 'S')
					allgame->player.rotation_angle = M_PI / 2;    // Facing South (down)
				else if (maps[i][j] == 'E')
					allgame->player.rotation_angle = 0;           // Facing East (right)
				else if (maps[i][j] == 'W')
					allgame->player.rotation_angle = M_PI;        // Facing West (left)
				break;
			}
			j++;
		}
		i++;
	}
		// free(maps);
		// free_2d_array(allgame->map);
	allgame->show_minimap = 1;
}
int close_window(t_datagame *game) {
    mlx_destroy_image(game->mlx, game->img.image);
    mlx_destroy_window(game->mlx, game->window);
    exit(0);
    return 0;
}

int main()
{
  t_datagame allgame ;
  char **maps = men() ;
  if(!maps)
		return 0;
		
  declare_game(&allgame,maps);
  mlx_hook(allgame.window,2,1L<< 0,key_walking,&allgame);
  mlx_hook(allgame.window,3,1L<< 1,key_relass,&allgame);
 mlx_hook(allgame.window, 17, 0, close_window, &allgame);
      mlx_loop_hook(allgame.mlx, game_loop, &allgame);
  mlx_loop(allgame.mlx);
  	
}