/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   copy.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelbied <oelbied@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 11:48:02 by oelbied           #+#    #+#             */
/*   Updated: 2025/08/21 09:10:30 by oelbied          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


// #include <mlx.h>
// #include <math.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <string.h>

// #define WIDIH 1200
// #define HEIGHT 800
// #define NUM_RAYS         WIDIH
// #define WINDOW_WIDTH    (MAP_NUM_COLS * TILE_SIZE)
// #define WINDOW_HEIGHT   (MAP_NUM_ROWS * TILE_SIZE)
// #define EPSILON 1e-6
// #define MAP_NUM_ROWS    12
// #define MAP_NUM_COLS    39
// #define TILE_SIZE       64
// #define PLAYER_SIZE     8
// #define FOV_ANGLE       (60 * (M_PI / 180))
// typedef struct s_dataplayer
// {
// 	double x;
// 	double y;
// 	double rotation_angle;
// 	double move_speed;
// 	double rotation_speed;
// 	int turn_direction;
// 	int walk_direction;
// 	int side_direction;

// } t_dataplayer;
// typedef struct s_image
// {
// 	void *image;
// 	char *addr;
// 	int bits_per_pixel;
// 	int line_length;
// 	int endian;
// } t_image;
// typedef struct s_ray {
//     double ray_angle;
//     double wall_hit_x;
//     double wall_hit_y;
//     double distance;
//     int was_hit_vertical;
//     int wall_hit_content;
// } t_ray;
// typedef struct s_datagame
// {
// 	void *mlx;
// 	void *window;
// 	t_image img;
// 	char map[MAP_NUM_ROWS][MAP_NUM_COLS];
// 	int show_minimap;
// 	t_dataplayer player;
// 	t_ray rays[NUM_RAYS];
// } t_datagame;




// void 	my_mlx_pixel_put(t_image *img,int x,int y,int color)
// {
// 	char *dst;
// 	  if (x >= 0 && x < WIDIH && y >= 0 && y < HEIGHT) 
// 	{
//        dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
//         *(unsigned int*)dst = color;
//     }
// }
// void draw_pixel(t_image *img, int x, int y, int width, int height, int color)
// {
// 	int i = 0;
// 	int j = 0;
// 	while (i < width)
// 	{
// 		j =0;
// 		while (j < height)
// 		{
// 			if(x + i >= 0 && x + i < WIDIH && y + j >= 0 && y + j < HEIGHT)
// 			my_mlx_pixel_put(img,x+i,y+j, color);
// 			j++;
// 		}
// 		i++;
// 	}
	
// }

// void draw_sercel_player(t_image *img,int conter_x,int conter_y, int radius , int color)
// {
// 	int x = -radius;
// 	int y = -radius;
// 	int px;
// 	int py;
// 	while (y <= radius)
// 	{
// 		x = -radius;
// 		while (x <= radius)
// 		{
// 			if((x * x) + (y * y) <= radius * radius)
// 			{
// 				px = conter_x + x;
// 				py = conter_y + y;
// 				if(px >= 0 && px < WIDIH && py >= 0 && py < HEIGHT)
// 				{
// 					my_mlx_pixel_put(img,px,py, color);
// 				}
// 			}	
// 			x++;
// 		}
// 		y++;
// 	}
	
// }

// void draw_line(t_image *img,int x0,int y0,int x1,int y1,int color)
// {
// 	int dx = abs(x1 - x0);
// 	int dy = -abs(y1 - y0);
// 	int sx = (x0 < x1) ? 1 : -1;
//     int sy = (y0 < y1) ? 1 : -1;
//     int err = dx + dy;
//     int e2;

//     while (1) {
//         my_mlx_pixel_put(img, x0, y0, color);
//         if (x0 == x1 && y0 == y1) break;
//         e2 = 2 * err;
//         if (e2 >= dy) {
//             err += dy;
//             x0 += sx;
//         }
//         if (e2 <= dx) {
//             err += dx;
//             y0 += sy;
//         }
//     }
// }
// double (double x1, double y1, double x2, double y2) {
//     return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
// }
// double normalize_angle(double angle) {
//     angle = fmod(angle, 2 * M_PI);
//     if (angle < 0) angle += (2 * M_PI);
//     return angle;
// }
// int has_wall_at(t_datagame *game, double x, double y) {
//     if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
//         return 1;

//     int map_x = (int)(x / TILE_SIZE);
//     int map_y = (int)(y / TILE_SIZE);

//     if (map_y < 0 || map_y >= MAP_NUM_ROWS || map_x < 0 || map_x >= MAP_NUM_COLS)
//         return 1;

//     return (game->map[map_y][map_x] == '1');
// }
// void cast_ray(t_datagame *game, double ray_angle, int ray_id) {
//     ray_angle = normalize_angle(ray_angle);
    
//     t_ray *ray = &game->rays[ray_id];
//     ray->ray_angle = ray_angle;
//     ray->distance = INFINITY;
//     ray->was_hit_vertical = 0;
//     ray->wall_hit_content = '0';
    
//     double horizontal_distance = INFINITY;
//     double vertical_distance = INFINITY;
//     double horz_hit_x = 0, horz_hit_y = 0;
//     double vert_hit_x = 0, vert_hit_y = 0;
//     int horz_wall_content = '0', vert_wall_content = '0';
    
//     // === Horizontal intersections ===
//     double is_ray_facing_down = sin(ray_angle) > 0;
//     double is_ray_facing_up = !is_ray_facing_down;
//     double is_ray_facing_right = cos(ray_angle) > 0;
//     double is_ray_facing_left = !is_ray_facing_right;
    
//     double yintercept = floor(game->player.y / TILE_SIZE) * TILE_SIZE;
//     if (is_ray_facing_down)
//         yintercept += TILE_SIZE;
    
//     double xintercept = game->player.x + (yintercept - game->player.y) / tan(ray_angle);
    
//     double ystep = TILE_SIZE;
//     if (is_ray_facing_up)
//         ystep *= -1;
    
//     double xstep = TILE_SIZE / tan(ray_angle);
//     if ((is_ray_facing_left && xstep > 0) || (is_ray_facing_right && xstep < 0))
//         xstep *= -1;
    
//     double next_horz_x = xintercept;
//     double next_horz_y = yintercept;
    
//     // تحسين حلقة البحث عن التقاطعات الأفقية
//     while (next_horz_x >= 0 && next_horz_x < WINDOW_WIDTH && 
//            next_horz_y >= 0 && next_horz_y < WINDOW_HEIGHT) {
//         double x_to_check = next_horz_x;
//         double y_to_check = next_horz_y + (is_ray_facing_up ? -1 : 0);
        
//         if (has_wall_at(game, x_to_check, y_to_check)) {
//             horz_hit_x = next_horz_x;
//             horz_hit_y = next_horz_y;
//             horizontal_distance = distance_between_points(game->player.x, game->player.y, 
//                                                          horz_hit_x, horz_hit_y);
//             // التحقق من صحة الإحداثيات قبل الوصول للخريطة
//             int map_x = (int)(x_to_check / TILE_SIZE);
//             int map_y = (int)(y_to_check / TILE_SIZE);
//             if (map_y >= 0 && map_y < MAP_NUM_ROWS && map_x >= 0 && map_x < MAP_NUM_COLS) {
//                 horz_wall_content = game->map[map_y][map_x];
//             }
//             break;
//         }
        
//         next_horz_x += xstep;
//         next_horz_y += ystep;
//     }

//     // === Vertical intersections ===
//     xintercept = floor(game->player.x / TILE_SIZE) * TILE_SIZE;
//     if (is_ray_facing_right)
//         xintercept += TILE_SIZE;
    
//     yintercept = game->player.y + (xintercept - game->player.x) * tan(ray_angle);
    
//     xstep = TILE_SIZE;
//     if (is_ray_facing_left)
//         xstep *= -1;
    
//     ystep = TILE_SIZE * tan(ray_angle);
//     if ((is_ray_facing_up && ystep > 0) || (is_ray_facing_down && ystep < 0))
//         ystep *= -1;
    
//     double next_vert_x = xintercept;
//     double next_vert_y = yintercept;
    
//     // تحسين حلقة البحث عن التقاطعات العمودية
//     while (next_vert_x >= 0 && next_vert_x < WINDOW_WIDTH && 
//            next_vert_y >= 0 && next_vert_y < WINDOW_HEIGHT) {
//         double x_to_check = next_vert_x + (is_ray_facing_left ? -1 : 0);
//         double y_to_check = next_vert_y;
        
//         if (has_wall_at(game, x_to_check, y_to_check)) {
//             vert_hit_x = next_vert_x;
//             vert_hit_y = next_vert_y;
//             vertical_distance = (game->player.x, game->player.y, 
//                                                        vert_hit_x, vert_hit_y);
//             // التحقق من صحة الإحداثيات قبل الوصول للخريطة
//             int map_x = (int)(x_to_check / TILE_SIZE);
//             int map_y = (int)(y_to_check / TILE_SIZE);
//             if (map_y >= 0 && map_y < MAP_NUM_ROWS && map_x >= 0 && map_x < MAP_NUM_COLS) {
//                 vert_wall_content = game->map[map_y][map_x];
//             }
//             break;
//         }
        
//         next_vert_x += xstep;
//         next_vert_y += ystep;
//     }
    
//     // اختيار أقرب تقاطع
//     if (horizontal_distance < vertical_distance) {
//         ray->wall_hit_x = horz_hit_x;
//         ray->wall_hit_y = horz_hit_y;
//         ray->distance = horizontal_distance;
//         ray->was_hit_vertical = 0;
//         ray->wall_hit_content = horz_wall_content;
//     } else {
//         ray->wall_hit_x = vert_hit_x;
//         ray->wall_hit_y = vert_hit_y;
//         ray->distance = vertical_distance;
//         ray->was_hit_vertical = 1;
//         ray->wall_hit_content = vert_wall_content;
//     }
// }
// void cast_all_rays(t_datagame *game) {
//     double ray_angle = game->player.rotation_angle - (FOV_ANGLE / 2);
    
//     for (int i = 0; i < NUM_RAYS; i++) {
//         cast_ray(game, normalize_angle(ray_angle), i);
//         ray_angle += FOV_ANGLE / NUM_RAYS;
//     }
// }
// // void cast_all_rays(t_datagame *game) {
// //     double ray_angle ;
// //     int i = 0; 
	
// // 	ray_angle = game->player.rotation_angle - (FOV_ANGLE / 2);
// //     while (i < NUM_RAYS)
// // 	{
// //         cast_ray(game, normalize_angle(ray_angle), i);
// //         ray_angle += FOV_ANGLE / NUM_RAYS;
// // 		i++;
// //     }
// // }
// void make_recst_troiud(t_datagame *game)
// {
// 	draw_pixel(&game->img ,0,0,WIDIH,HEIGHT /2 ,0x87CEEB);
// 	draw_pixel(&game->img ,0,HEIGHT /2,WIDIH,HEIGHT /2 ,0x8B4513);
// 	t_ray *ray;
// 	double corrected_disn;
// 	double tall_wall;
// 	int i = 0;
// 	int wall_top;
// 	int wall_bottom;
// 	int base_color;
// 	double max_distanc;

// 	double shade ;
// 	int color;
// 	int strip_width;
// 	while (i < NUM_RAYS)
// 	{
// 		ray = &game->rays[i];
// 		if(ray->distance == INFINITY)
// 			continue;
// 		 corrected_disn = ray->distance * cos(ray->ray_angle - game->player.rotation_angle);
// 		if(corrected_disn < EPSILON)
// 			corrected_disn = EPSILON;
// 		tall_wall = (TILE_SIZE / corrected_disn) * ((WIDIH / 2) / tan(FOV_ANGLE /2));
// 		 wall_top = (HEIGHT /2) - (tall_wall /2);
// 		if(wall_top < 0)
// 			wall_top = 0;
// 		wall_bottom = (HEIGHT /2) + (tall_wall /2);
// 		if(wall_bottom >HEIGHT)
// 			wall_bottom = HEIGHT;
// 		base_color = 0;
// 		if(base_color == ray->was_hit_vertical)
// 			base_color = 0xFF6BBB;	
// 		else
// 			base_color = 0x4ECDC4;
			
// 		max_distanc = TILE_SIZE * 10;
// 		shade = 1.0 - fmin(ray->distance / max_distanc,0.7);
// 		shade = fmax(shade,0.2);
// 		color = ((int)((base_color >> 16 & 0xFF)* shade) << 16) |
// 					((int)((base_color >> 8 & 0xFF)* shade) << 8)|
// 					((int)((base_color  & 0xFF)* shade));
// 		strip_width = (WIDIH / NUM_RAYS) > 0 ? (WIDIH / NUM_RAYS) : 1;
		
// 	 draw_pixel(&game->img ,i * strip_width,wall_top,strip_width,wall_bottom - wall_top ,color);
// 		i++;
// 	}
	
	
// }
// void  makein_mine_maps(t_datagame *game)
// {
// 	if(!game->show_minimap)
// 		return;
// 	int menimaps = 200;
// 	int minimapx = WIDIH - menimaps - 20;
// 	int minimapy = 20;
// 	int player_x ;
// 	int player_y ;
	
// 	float scale = (float)menimaps / WINDOW_WIDTH ;
// 	draw_pixel(&game->img ,minimapx - 2, minimapy - 2,menimaps + 4,menimaps + 4,0x20ff00);
// 	draw_pixel(&game->img ,minimapx - 2, minimapy - 2,menimaps + 4,menimaps + 4,0x1a1a1a);
// 	int x = 0;
// 	int y = 0;
	
// 	while (y < MAP_NUM_ROWS)
// 	{
// 		x = 0;
// 		while (x < MAP_NUM_COLS)
// 		{
// 			  int tile_x = minimapx + x * TILE_SIZE * scale;
//             int tile_y = minimapy + y * TILE_SIZE * scale;
//             int tile_size = TILE_SIZE * scale;
// 			if(game->map[y][x] == '1')
// 			draw_pixel(&game->img ,tile_x, tile_y,tile_size,tile_size,0xFFFFFF);
// 			else
// 			draw_pixel(&game->img ,tile_x, tile_y,tile_size,tile_size,0x0000000);
// 			x++;
// 		}	
// 		y++;	
// 	}

// 	player_x = minimapx + game->player.x * scale;
// 	player_y = minimapy + game->player.y * scale;
// 	draw_sercel_player(&game->img,player_x,player_y,PLAYER_SIZE *scale,0xFF000FF);
	
// 	int line_end_x = player_x + cos(game->player.rotation_angle) * 30;
//     int line_end_y = player_y + sin(game->player.rotation_angle) * 30;
// 	draw_line(&game->img,player_x,player_y,line_end_x,line_end_y,0x0fafa1);

// 	int i = 0;
// 	while(i < NUM_RAYS)
// 	{  t_ray *ray = &game->rays[i];
// 		if (ray->distance != INFINITY) {
//             int ray_end_x = minimapx + (ray->wall_hit_x) * scale;
//             int ray_end_y = minimapy + (ray->wall_hit_y) * scale;
            
        
//             if (ray_end_x >= minimapx && ray_end_x < minimapx + menimaps &&
//                 ray_end_y >= minimapy && ray_end_y < minimapy + menimaps) {
//                 draw_line(&game->img, player_x, player_y, ray_end_x, ray_end_y, 0xFFFF00);
//             }
//         }
// 		i += 10;
// 	}
	
// }
// void render(t_datagame *game)
// {
// 	  memset(game->img.addr, 0, HEIGHT * game->img.line_length);
// 	  make_recst_troiud(game);
//    makein_mine_maps(game);
//      mlx_put_image_to_window(game->mlx, game->window, game->img.image, 0, 0);
// }

//  void movemone_plyr(t_datagame *game)
//  {  
// 	double moveing_step;
//  	double oder_step;
//  	double oder_x;
//  	double oder_y;

//  		moveing_step = game->player.walk_direction * game->player.move_speed;
//  		oder_step = game->player.side_direction * game->player.move_speed;
// 		oder_x =game->player.x;
// 		oder_y =game->player.y;

// 	if(moveing_step != 0)
// 	{
// 		oder_x += cos(game->player.rotation_angle ) * moveing_step;
// 		oder_y += sin(game->player.rotation_angle) * moveing_step;
// 	}
// 	if(oder_step != 0)
// 	{
// 		oder_x += cos(game->player.rotation_angle + (M_PI / 2)) * oder_step;
// 		oder_y += sin(game->player.rotation_angle * (M_PI / 2))* oder_step;
// 	}
// 	double sonter_player = PLAYER_SIZE /2;

// 	if(!has_wall_at(game,oder_x + sonter_player,game->player.y) && 
// 	!has_wall_at(game,oder_x - sonter_player,game->player.y))
// 	{	
// 		game->player.x = oder_x;
// 	}
// 	if(!has_wall_at(game,game->player.x,oder_y + sonter_player) && 
// 	!has_wall_at(game,game->player.x,oder_y - sonter_player))
// 	{
		
// 	game->player.y = oder_y;
// 	}

// 	if(game->player.turn_direction != 0)
// 	{
// 		game->player.rotation_angle += game->player.turn_direction * game->player.rotation_speed;
// 		game->player.rotation_angle = normalize_angle(game->player.rotation_angle);
// 	}
// 	//     double move_step = game->player.walk_direction * game->player.move_speed;
//     // double side_step = game->player.side_direction * game->player.move_speed;
    
//     // double new_player_x = game->player.x;
//     // double new_player_y = game->player.y;
    
//     // // حساب الموقع الجديد للحركة للأمام/الخلف
//     // if (move_step != 0) {
//     //     new_player_x += cos(game->player.rotation_angle) * move_step;
//     //     new_player_y += sin(game->player.rotation_angle) * move_step;
//     // }
    
//     // // حساب الموقع الجديد للحركة الجانبية
//     // if (side_step != 0) {
//     //     new_player_x += cos(game->player.rotation_angle + M_PI_2) * side_step;
//     //     new_player_y += sin(game->player.rotation_angle + M_PI_2) * side_step;
//     // }
    
//     // // فحص التصادمات بشكل منفصل للمحور x و y مع هامش أمان
//     // double collision_margin = PLAYER_SIZE / 2;
//     // if (!has_wall_at(game, new_player_x + collision_margin, game->player.y) && 
//     //     !has_wall_at(game, new_player_x - collision_margin, game->player.y)) {
//     //     game->player.x = new_player_x;
//     // }
//     // if (!has_wall_at(game, game->player.x, new_player_y + collision_margin) &&
//     //     !has_wall_at(game, game->player.x, new_player_y - collision_margin)) {
//     //     game->player.y = new_player_y;
//     // }
    
//     // // تحديث الدوران
//     // if (game->player.turn_direction != 0) {
//     //     game->player.rotation_angle += game->player.turn_direction * game->player.rotation_speed;
//     //     game->player.rotation_angle = normalize_angle(game->player.rotation_angle);
//     // }
//  }


// int key_walking( int key,t_datagame *game)
// {
// 	if(key == 53)
// 	{
// 		mlx_destroy_image(game->mlx,game->img.image);
// 		mlx_destroy_window(game->mlx,game->window);
// 		exit(0);
// 	}
// 	else if(key == 13 || key == 126)
// 		game->player.walk_direction = 1;
// 	else if(key == 1 || key == 125)
// 		game->player.walk_direction = -1;
// 	else if(key == 0)
// 		game->player.side_direction = -1;
// 	else if(key == 2)
// 		game->player.side_direction = 1;
// 	else if(key == 123)
// 		game->player.turn_direction = -1;
// 	else if(key == 124)
// 		game->player.turn_direction = 1;
// 	else if(key == 46)
// 		game->show_minimap = !game->show_minimap;
// 	return 0;
// }
// int key_relass(int key, t_datagame *game)
// {
// 	if(key == 13 || key == 126)
// 		game->player.walk_direction = 0;
// 	else if(key == 1 || key == 125)
// 		game->player.walk_direction = 0;
// 	else if(key == 0)
// 		game->player.side_direction = 0;
// 	else if(key == 2)
// 		game->player.side_direction = 0;
// 	else if(key == 123)
// 		game->player.turn_direction = 0;
// 	else if(key == 124)
// 		game->player.turn_direction = 0;

// 	return 0;
// }
// int game_loop(t_datagame *game)
// {
// 	 movemone_plyr(game);
// 	   cast_all_rays(game);
// 	  render(game);
// 	  return 0;
// }


// void declare_game(t_datagame *allgame)
// {

// 	int x = 0;
// 	int y = 0;
// 	allgame->mlx = mlx_init();
// 	allgame->window = mlx_new_window(allgame->mlx, WIDIH, HEIGHT, "Cup3D");
// 	allgame->img.image = mlx_new_image(allgame->mlx, WIDIH, HEIGHT);
// 	allgame->img.addr = mlx_get_data_addr(allgame->img.image, &allgame->img.bits_per_pixel, &allgame->img.line_length, &allgame->img.endian);

// 	allgame->player.x = WIDIH / 2;
// 	allgame->player.y = HEIGHT / 2;
// 	allgame->player.rotation_angle = M_PI / 2;
// 	allgame->player.move_speed = 3.0;
// 	allgame->player.rotation_speed = 3 * (M_PI_2 / 180);
// 	allgame->player.turn_direction = 0;
// 	allgame->player.walk_direction = 0;
// 	allgame->player.side_direction = 0;

// 	char *maps[MAP_NUM_ROWS] =
// 		{
// 			"111111111111111111111111111111111111111",
// 			"100000000100000000010000000010000000001",
// 			"100000000100000000010000000010000000001",
// 			"100000111100000000010000000010000000001",
// 			"100000000100000111110000000010000000001",
// 			"100000000111100110000000000010000000001",
// 			"100000000000000010000000000100000000001",
// 			"100000000000000010000000000100000000001",
// 			"111111111000000010000000000010000000001",
// 			"100000000000001010010000000010000000001",
// 			"100000000000001000000000000100000000011",
// 			"111111111111111111111111111111111111111"};
// 	while (y < MAP_NUM_ROWS)
// 	{
// 		x = 0;
// 		while (x < MAP_NUM_COLS)
// 		{
// 			allgame->map[y][x] = maps[y][x];
// 			x++;
// 		}
// 		y++;
// 	}
// 	allgame->show_minimap = 1;
// }
// int close_window(t_datagame *game) {
//     mlx_destroy_image(game->mlx, game->img.image);
//     mlx_destroy_window(game->mlx, game->window);
//     exit(0);
//     return 0;
// }

// int main()
// {
//   t_datagame allgame ;
//   declare_game(&allgame);
//   mlx_hook(allgame.window,2,1L<< 0,key_walking,&allgame);
//   mlx_hook(allgame.window,3,1L<< 1,key_relass,&allgame);
//  mlx_hook(allgame.window, 17, 0, close_window, &allgame);
//       mlx_loop_hook(allgame.mlx, game_loop, &allgame);
//   mlx_loop(allgame.mlx);
// }


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
// double distance_between_points(double x1, double y1, double x2, double y2) {
//     return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
// }
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

    // حساب اتجاه الشعاع
    double rayDirX = cos(ray_angle);
    double rayDirY = sin(ray_angle);

    // موقع اللاعب كبداية
    double posX = game->player.x / TILE_SIZE;
    double posY = game->player.y / TILE_SIZE;

    // إحداثيات الخلية الحالية
    int mapX = (int)posX;
    int mapY = (int)posY;

    // المسافة من خطوة إلى أخرى (delta)
    double deltaDistX = fabs(1 / rayDirX);
    double deltaDistY = fabs(1 / rayDirY);

    double sideDistX;
    double sideDistY;

    int stepX;
    int stepY;

    // حساب الاتجاه الأولي
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

    // خوارزمية DDA
    while (!hit) {
        if (sideDistX < sideDistY) {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = 0; // تقاطع عمودي
        } else {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = 1; // تقاطع أفقي
        }
        // تحقق من الجدار
        if (mapY >= 0 && mapY < game->row &&
            mapX >= 0 && mapX < game->colone &&
            game->map[mapY][mapX] == '1') {
            hit = 1;
        }
    }

    // احسب المسافة النهائية (تُصحح لاحقاً في render)
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

// void cast_ray(t_datagame *game, double ray_angle, int ray_id) {
//     ray_angle = normalize_angle(ray_angle);
    
//     t_ray *ray = &game->rays[ray_id];
//     ray->ray_angle = ray_angle;
//     ray->distance = INFINITY;
//     ray->was_hit_vertical = 0;
//     ray->wall_hit_content = '0';
    
//     double horizontal_distance = INFINITY;
//     double vertical_distance = INFINITY;
//     double horz_hit_x = 0, horz_hit_y = 0;
//     double vert_hit_x = 0, vert_hit_y = 0;
//     int horz_wall_content = '0', vert_wall_content = '0';
    
//     // === Horizontal intersections ===
//     double is_ray_facing_down = sin(ray_angle) > 0;
//     double is_ray_facing_up = !is_ray_facing_down;
//     double is_ray_facing_right = cos(ray_angle) > 0;
//     double is_ray_facing_left = !is_ray_facing_right;
    
//     double yintercept = floor(game->player.y / TILE_SIZE) * TILE_SIZE;
//     if (is_ray_facing_down)
//         yintercept += TILE_SIZE;
    
//     double xintercept = game->player.x + (yintercept - game->player.y) / tan(ray_angle);
    
//     double ystep = TILE_SIZE;
//     if (is_ray_facing_up)
//         ystep *= -1;
    
//     double xstep = TILE_SIZE / tan(ray_angle);
//     if ((is_ray_facing_left && xstep > 0) || (is_ray_facing_right && xstep < 0))
//         xstep *= -1;
    
//     double next_horz_x = xintercept;
//     double next_horz_y = yintercept;
    
//     // تحسين حلقة البحث عن التقاطعات الأفقية
//     while (next_horz_x >= 0 && next_horz_x < WINDOW_WIDTH && 
//            next_horz_y >= 0 && next_horz_y < WINDOW_HEIGHT) {
//         double x_to_check = next_horz_x;
//         double y_to_check = next_horz_y + (is_ray_facing_up ? -1 : 0);
        
//         if (has_wall_at(game, x_to_check, y_to_check)) {
//             horz_hit_x = next_horz_x;
//             horz_hit_y = next_horz_y;
//             horizontal_distance = distance_between_points(game->player.x, game->player.y, 
//                                                          horz_hit_x, horz_hit_y);
//             // التحقق من صحة الإحداثيات قبل الوصول للخريطة
//             int map_x = (int)(x_to_check / TILE_SIZE);
//             int map_y = (int)(y_to_check / TILE_SIZE);
//             if (map_y >= 0 && map_y < MAP_NUM_ROWS && map_x >= 0 && map_x < MAP_NUM_COLS) {
//                 horz_wall_content = game->map[map_y][map_x];
//             }
//             break;
//         }
        
//         next_horz_x += xstep;
//         next_horz_y += ystep;
//     }

//     // === Vertical intersections ===
//     xintercept = floor(game->player.x / TILE_SIZE) * TILE_SIZE;
//     if (is_ray_facing_right)
//         xintercept += TILE_SIZE;
    
//     yintercept = game->player.y + (xintercept - game->player.x) * tan(ray_angle);
    
//     xstep = TILE_SIZE;
//     if (is_ray_facing_left)
//         xstep *= -1;
    
//     ystep = TILE_SIZE * tan(ray_angle);
//     if ((is_ray_facing_up && ystep > 0) || (is_ray_facing_down && ystep < 0))
//         ystep *= -1;
    
//     double next_vert_x = xintercept;
//     double next_vert_y = yintercept;
    
//     // تحسين حلقة البحث عن التقاطعات العمودية
//     while (next_vert_x >= 0 && next_vert_x < WINDOW_WIDTH && 
//            next_vert_y >= 0 && next_vert_y < WINDOW_HEIGHT) {
//         double x_to_check = next_vert_x + (is_ray_facing_left ? -1 : 0);
//         double y_to_check = next_vert_y;
        
//         if (has_wall_at(game, x_to_check, y_to_check)) {
//             vert_hit_x = next_vert_x;
//             vert_hit_y = next_vert_y;
//             vertical_distance = distance_between_points(game->player.x, game->player.y, 
//                                                        vert_hit_x, vert_hit_y);
//             // التحقق من صحة الإحداثيات قبل الوصول للخريطة
//             int map_x = (int)(x_to_check / TILE_SIZE);
//             int map_y = (int)(y_to_check / TILE_SIZE);
//             if (map_y >= 0 && map_y < MAP_NUM_ROWS && map_x >= 0 && map_x < MAP_NUM_COLS) {
//                 vert_wall_content = game->map[map_y][map_x];
//             }
//             break;
//         }
        
//         next_vert_x += xstep;
//         next_vert_y += ystep;
//     }
    
//     // اختيار أقرب تقاطع
//     if (horizontal_distance < vertical_distance) {
//         ray->wall_hit_x = horz_hit_x;
//         ray->wall_hit_y = horz_hit_y;
//         ray->distance = horizontal_distance;
//         ray->was_hit_vertical = 0;
//         ray->wall_hit_content = horz_wall_content;
//     } else {
//         ray->wall_hit_x = vert_hit_x;
//         ray->wall_hit_y = vert_hit_y;
//         ray->distance = vertical_distance;
//         ray->was_hit_vertical = 1;
//         ray->wall_hit_content = vert_wall_content;
//     }
// }
// void cast_all_rays(t_datagame *game) {
//     double ray_angle = game->player.rotation_angle - (FOV_ANGLE / 2);
    
//     for (int i = 0; i < NUM_RAYS; i++) {
//         cast_ray(game, normalize_angle(ray_angle), i);
//         ray_angle += FOV_ANGLE / NUM_RAYS;
//     }
// }
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
void  makein_mine_maps(t_datagame *game)
{
	if(!game->show_minimap)
		return;
	int menimaps = 200;
	int minimapx = WIDIH - menimaps - 20;
	int minimapy = 20;
	int player_x ;
	int player_y ;
	
	float scale = (float)menimaps / game->windo_with;
	draw_pixel(&game->img ,minimapx - 2, minimapy - 2,menimaps + 4,menimaps + 4,0x20ff00);
	draw_pixel(&game->img ,minimapx - 2, minimapy - 2,menimaps + 4,menimaps + 4,0x1a1a1a);
	int x = 0;
	int y = 0;
	
	while (y < game->row)
	{
		x = 0;
		while (x <game->colone)
		{
			  int tile_x = minimapx + x * TILE_SIZE * scale;
            int tile_y = minimapy + y * TILE_SIZE * scale;
            int tile_size = TILE_SIZE * scale;
			if(game->map[y][x] == '1')
			draw_pixel(&game->img ,tile_x, tile_y,tile_size,tile_size,0xFFFFFF);
			else
			draw_pixel(&game->img ,tile_x, tile_y,tile_size,tile_size,0x0000000);
			x++;
		}	
		y++;	
	}

	player_x = minimapx + game->player.x * scale;
	player_y = minimapy + game->player.y * scale;
	draw_sercel_player(&game->img,player_x,player_y,PLAYER_SIZE *scale,0xFF000FF);
	
	int line_end_x = player_x + cos(game->player.rotation_angle) * 30;
    int line_end_y = player_y + sin(game->player.rotation_angle) * 30;
	draw_line(&game->img,player_x,player_y,line_end_x,line_end_y,0x0fafa1);

	int i = 0;
	while(i < NUM_RAYS)
	{  t_ray *ray = &game->rays[i];
		if (ray->distance != INFINITY) {
            int ray_end_x = minimapx + (ray->wall_hit_x) * scale;
            int ray_end_y = minimapy + (ray->wall_hit_y) * scale;
            
        
            if (ray_end_x >= minimapx && ray_end_x < minimapx + menimaps &&
                ray_end_y >= minimapy && ray_end_y < minimapy + menimaps) {
                draw_line(&game->img, player_x, player_y, ray_end_x, ray_end_y, 0xFFFF00);
            }
        }
		i += 10;
	}
	
}
void render(t_datagame *game)
{
	  memset(game->img.addr, 0, HEIGHT * game->img.line_length);
	  make_recst_troiud(game);
   makein_mine_maps(game);
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
	//     double move_step = game->player.walk_direction * game->player.move_speed;
    // double side_step = game->player.side_direction * game->player.move_speed;
    
    // double new_player_x = game->player.x;
    // double new_player_y = game->player.y;
    
    // // حساب الموقع الجديد للحركة للأمام/الخلف
    // if (move_step != 0) {
    //     new_player_x += cos(game->player.rotation_angle) * move_step;
    //     new_player_y += sin(game->player.rotation_angle) * move_step;
    // }
    
    // // حساب الموقع الجديد للحركة الجانبية
    // if (side_step != 0) {
    //     new_player_x += cos(game->player.rotation_angle + M_PI_2) * side_step;
    //     new_player_y += sin(game->player.rotation_angle + M_PI_2) * side_step;
    // }
    
    // // فحص التصادمات بشكل منفصل للمحور x و y مع هامش أمان
    // double collision_margin = PLAYER_SIZE / 2;
    // if (!has_wall_at(game, new_player_x + collision_margin, game->player.y) && 
    //     !has_wall_at(game, new_player_x - collision_margin, game->player.y)) {
    //     game->player.x = new_player_x;
    // }
    // if (!has_wall_at(game, game->player.x, new_player_y + collision_margin) &&
    //     !has_wall_at(game, game->player.x, new_player_y - collision_margin)) {
    //     game->player.y = new_player_y;
    // }
    
    // // تحديث الدوران
    // if (game->player.turn_direction != 0) {
    //     game->player.rotation_angle += game->player.turn_direction * game->player.rotation_speed;
    //     game->player.rotation_angle = normalize_angle(game->player.rotation_angle);
    // }
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

// void free_2d_array(char **arr)
// {
//     int i = 0;
//     if (!arr)
//         return;
//     while (arr[i])
//     {
//         free(arr[i]);  // حرر كل صف
//         i++;
//     }
//     free(arr);  // حرر المؤشر الرئيسي
// }

void declare_game(t_datagame *allgame)
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
    
	char **maps	= men(); ;
		allgame->map=maps;
		// {
		// 	"111111111111111111111111111111111111111",
		// 	"100000000100000000010000000010000000001",
		// 	"100000000100000000010000000010000000001",
		// 	"100000111100000000010000000010000000001",
		// 	"100000000100000111110000000010000000001",
		// 	"100000000111100110000000000010000000001",
		// 	"100000000000000010000000000100000000001",
		// 	"100000000000000010000000000100000000001",
		// 	"111111111000000010000W00000010000000001",
		// 	"100000000000001010010000000010000000001",
		// 	"100000000000001000000000000100000000011",
		// 	"111111111111111111111111111111111111111"};

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

	// allgame->map = allgame->map[allgame->colone][allgame->row];
		printf("%d+++++++++++\n",allgame->row);
		printf("%d+++++++++++\n",allgame->colone);
		printf("%d+++++++++++\n",allgame->windo_hithe);
		printf("%d+++++++++++\n",allgame->windo_with);
		// printf("%d+++++++++++\n",WINDOW_WIDTH)
		// printf("%d+++++++++++\n",WINDOW_HEIGHT);
	

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
  declare_game(&allgame);
  mlx_hook(allgame.window,2,1L<< 0,key_walking,&allgame);
  mlx_hook(allgame.window,3,1L<< 1,key_relass,&allgame);
 mlx_hook(allgame.window, 17, 0, close_window, &allgame);
      mlx_loop_hook(allgame.mlx, game_loop, &allgame);
  mlx_loop(allgame.mlx);
  	
}