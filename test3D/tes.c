
#include <mlx.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define WIN_WIDTH 1200 
#define WIN_HEIGHT 800 

#define MAP_NUM_ROWS    12
#define MAP_NUM_COLS    20
#define TILE_SIZE       22
#define WINDOW_WIDTH    (MAP_NUM_COLS * TILE_SIZE)
#define WINDOW_HEIGHT   (MAP_NUM_ROWS * TILE_SIZE)
// #define WIN_WIDTH       WINDOW_WIDTH
// #define WIN_HEIGHT      WINDOW_HEIGHT

#define SCALE           10
#define ZOOM_Z          10
#define OFFSET_X        400
#define OFFSET_Y        200
#define BLOCK_SIZE      8

#define WALL_STRIP_WIDTH 4
#define NUM_RAYS        (WINDOW_WIDTH / WALL_STRIP_WIDTH)
#define FOV_ANGLE       (60 * (M_PI / 180))

typedef struct s_vars {
    void *mlx;
    void *win;
} t_vars;

typedef struct s_dataplyer {
   double x;  
   double y;
   int radius;
   int turnDirection;
   int walkDirection;
   int sideDirection;
   double rotationtangle;
   double movespeed;
   double rotationspeed;
   void *mlx_ptr;
   void *win_ptr;
   char **map;
   void *img;
   char *addr;
   int bits_per_pixel;
   int line_length;
   int endian;
} t_dataplyer;


double normalize_angle(double angle)
{
    double two_pi = 2.0 * M_PI;
    while (angle < 0) angle += two_pi;
    while (angle >= two_pi) angle -= two_pi;
    return angle;
}

double distance_between_points(double x1, double y1, double x2, double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}
int has_wall_at(char **map, double x, double y)
{
    int map_x = (int)(x / TILE_SIZE);
    int map_y = (int)(y / TILE_SIZE);

    if (map_y < 0 || map_y >= MAP_NUM_ROWS || map_x < 0 || map_x >= MAP_NUM_COLS)
        return 1;

    return (map[map_y][map_x] == '1');
}

void my_mlx_pixel_put(t_dataplyer *data, int x, int y, int color)
{
    char *dst;

    if (x >= 0 && x < WIN_WIDTH && y >= 0 && y < WIN_HEIGHT)
    {
        dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
        *(unsigned int*)dst = color;
    }
}

void draw_line(t_dataplyer *data, int x0, int y0, int x1, int y1, int color)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;

    while (1)
    {
        my_mlx_pixel_put(data, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

void draw_circle(t_dataplyer *data, int center_x, int center_y, int radius, int color)
{
    for (int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++)
        {
            if (x * x + y * y <= radius * radius)
            {
                int px = center_x + x;
                int py = center_y + y;
                my_mlx_pixel_put(data, px, py, color);
            }
        }
    }
}



void draw_square(t_dataplyer *data, int x, int y, int size, int color)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            int px = x + i;
            int py = y + j;
            my_mlx_pixel_put(data, px, py, color);
        }
    }
}
void cast_and_draw_ray(t_dataplyer *data, double ray_angle, int color)
{
    ray_angle = normalize_angle(ray_angle);

    double ray_x = data->x;
    double ray_y = data->y;
    double step_size = 1.0; 
    double dx = cos(ray_angle) * step_size;
    double dy = sin(ray_angle) * step_size;

    int max_depth = WINDOW_WIDTH;
    for (int i = 0; i < max_depth; i++)
    {
        ray_x += dx;
        ray_y += dy;

   
        if (has_wall_at(data->map, ray_x, ray_y))
        {
         
            draw_line(data, (int)data->x, (int)data->y, (int)ray_x, (int)ray_y, color);
            break;
        }
      
    }
}
 void calleaye_wall(t_dataplyer *data)
 {
    double start_angle = data->rotationtangle - (FOV_ANGLE / 2.0);
    double ray_angle;
    double angle_step = FOV_ANGLE / (double)NUM_RAYS;

    for (int i = 0; i < NUM_RAYS; i++)
    {
        ray_angle = start_angle + i * angle_step;
        cast_and_draw_ray(data, ray_angle, 0xFFFF00);
    }

 }
void render(t_dataplyer *data, char **map)
{

    for (int y = 0; y < WIN_HEIGHT; y++)
    {
        for (int x = 0; x < WIN_WIDTH; x++)
        {
            my_mlx_pixel_put(data, x, y, 0x000000); 
    }


    for (int i = 0; i < MAP_NUM_ROWS; i++)
    {
        for (int j = 0; j < MAP_NUM_COLS; j++)
        {
            int tilex = j * TILE_SIZE;
            int tiley = i * TILE_SIZE;
            int color = (map[i][j] == '1') ? 0x22dd22 : 0x444444;
            draw_square(data, tilex, tiley, TILE_SIZE - 2, color);
        }
    }
	}
calleaye_wall(data);
    // رسم اللاعب
    draw_circle(data, (int)data->x, (int)data->y, data->radius, 0xFF0000);
    
    // رسم اتجاه اللاعب
    int endX = (int)(data->x + cos(data->rotationtangle) * 30);
    int endY = (int)(data->y + sin(data->rotationtangle) * 30);
    draw_line(data, (int)data->x, (int)data->y, endX, endY, 0x0000FF);

    // عرض الصورة
    mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->img, 0, 0);
}

void play_player(t_dataplyer *data, t_vars *vars)
{
    data->x = WINDOW_WIDTH / 2.0;
    data->y = WINDOW_HEIGHT / 2.0;
    data->radius = 8;
    data->turnDirection = 0;
    data->walkDirection = 0;
    data->sideDirection = 0;
    data->rotationtangle = M_PI / 2;
    data->movespeed = 2.5;
    data->rotationspeed = 30 * (M_PI / 180);
    data->mlx_ptr = vars->mlx;
    data->win_ptr = vars->win;

    data->img = mlx_new_image(data->mlx_ptr, WIN_WIDTH, WIN_HEIGHT);
    data->addr = mlx_get_data_addr(data->img, &data->bits_per_pixel, 
                                   &data->line_length, &data->endian);
}

int key_press(int keycode, t_dataplyer *data)
{
    if (keycode == 13 || keycode == 126) 
        data->walkDirection = 1;
    else if (keycode == 1 || keycode == 125) 
        data->walkDirection = -1;
    else if (keycode == 0) 
        data->sideDirection = -1;
    else if (keycode == 2)
        data->sideDirection = 1;
    else if (keycode == 123) 
        data->turnDirection = -1;
    else if (keycode == 124) 
        data->turnDirection = 1;
    else if (keycode == 53) 
    {
        mlx_destroy_window(data->mlx_ptr, data->win_ptr);
        exit(0);
    }
    return (0);
}


int key_release(int keycode, t_dataplyer *data)
{
    if ((keycode == 13 || keycode == 126) && data->walkDirection == 1)
        data->walkDirection = 0;
    else if ((keycode == 1 || keycode == 125) && data->walkDirection == -1)
        data->walkDirection = 0;
    else if (keycode == 0 && data->sideDirection == -1)
        data->sideDirection = 0;
    else if (keycode == 2 && data->sideDirection == 1)
        data->sideDirection = 0;
    else if (keycode == 123 && data->turnDirection == -1)
        data->turnDirection = 0;
    else if (keycode == 124 && data->turnDirection == 1)
        data->turnDirection = 0;
    return (0);
}

void update_player(t_dataplyer *data)
{

    data->rotationtangle += data->turnDirection * data->rotationspeed;


    double moveStep = data->walkDirection * data->movespeed;
    double sideStep = data->sideDirection * data->movespeed;


    double newX = data->x + cos(data->rotationtangle) * moveStep;
    double newY = data->y + sin(data->rotationtangle) * moveStep;


    newX += cos(data->rotationtangle + M_PI_2) * sideStep;
    newY += sin(data->rotationtangle + M_PI_2) * sideStep;


    if (!has_wall_at(data->map, newX, data->y))
        data->x = newX;
    if (!has_wall_at(data->map, data->x, newY))
        data->y = newY;


    if (data->x < data->radius)
        data->x = data->radius;
    if (data->x > WINDOW_WIDTH - data->radius)
        data->x = WINDOW_WIDTH - data->radius;
    if (data->y < data->radius)
        data->y = data->radius;
    if (data->y > WINDOW_HEIGHT - data->radius)
        data->y = WINDOW_HEIGHT - data->radius;
}

int loop_hook(void *param)
{
    t_dataplyer *data = (t_dataplyer *)param;
    update_player(data);
    render(data, data->map);
    return (0);
}

void handle_event(t_dataplyer *data)
{
    mlx_hook(data->win_ptr, 2, 1L << 0, key_press, data);
    mlx_hook(data->win_ptr, 3, 1L << 1, key_release, data);
}

int main()
{
    t_vars vars;
    t_dataplyer data;
    
    char *map[] = {
        "11111111111111111111",
        "10000000010000000001",
        "10000000010000000001",
        "10000000000000000001",
        "10000000000000011111",
        "10000000000000000001",
        "10000000000000000001",
        "10000000000000000001",
        "11111111100000000001",
        "10000000000000000001",
        "10000000000000000001",
        "11111111111111111111",
        NULL
    };
    
    data.map = map;
    vars.mlx = mlx_init();
    vars.win = mlx_new_window(vars.mlx, WIN_WIDTH, WIN_HEIGHT, "2D Map - Fixed Movement");

    play_player(&data, &vars);
    render(&data, data.map);
    handle_event(&data);
    mlx_loop_hook(vars.mlx, loop_hook, &data);
    mlx_loop(vars.mlx);
    
    return 0;
}
#include <mlx.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


// #define M_PI 3.14159265358979323846
#define WIN_WIDTH 1200
#define WIN_HEIGHT 800

#define SCALE 30
#define ZOOM_Z 10
#define OFFSET_X 400
#define TILE_SIZE 32
#define MAP_NUM_ROWS 12
#define MAP_NUM_COLS 20
#define WINDOW_WIDTH  (MAP_NUM_COLS * TILE_SIZE)
#define WINDOW_HEIGHT (MAP_NUM_ROWS * TILE_SIZE)

#define OFFSET_Y 200
#define BLOCK_SIZE 8 // حجم كل مربع

typedef struct s_vars {
    void *mlx;
    void *win;

} t_vars;

typedef struct s_dataplyer {
   int x;
   int y;
   int radius;
   int turnDirection;
   int walkDirection;
   int sideDirection;
   double rotationtangle;
   double movespeed;
   double rotationspeed;
    void *mlx_ptr;
    void *win_ptr;
	char **map;
} t_dataplyer;

void draw_line(t_dataplyer *data, int x0, int y0, float angle, int length)
{
	int i;
	int x;
	int y;

	i = 0;
	while (i < length)
	{
		// Calculate the next point on the line using trigonometry
		x = x0 + cos(angle) * i;
		y = y0 + sin(angle) * i;
		// Draw the pixel if it's within bounds
		if (x >= 0 && x < WINDOW_WIDTH  && y >= 0 && y < WINDOW_HEIGHT)
			my_mlx_pixel_put(data, x, y,0xFF0000 ); // Player direction line color
		i++;
	}
}

// void draw_line(void *mlx, void *win, int x1, int y1, int x2, int y2, int color)
// {
//     int dx = abs(x2 - x1);
//     int dy = abs(y2 - y1);
//     int sx = x1 < x2 ? 1 : -1;
//     int sy = y1 < y2 ? 1 : -1;
//     int err = (dx > dy ? dx : -dy) / 2;
//     int e2;

//     while (1)
//     {
//         mlx_pixel_put(mlx, win, x1, y1, color);
//         if (x1 == x2 && y1 == y2) break;
//         e2 = err;
//         if (e2 > -dx) { err -= dy; x1 += sx; }
//         if (e2 < dy) { err += dx; y1 += sy; }
//     }
// }




void draw_circle(void *mlx, void *win, int center_x, int center_y, int radius, int color)
{
    for (int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++)
        {
            if (x * x + y * y <= radius * radius)
            {
                int px = center_x + x;
                int py = center_y + y;
			
                if (px >= 0 && px < WIN_WIDTH && py >= 0 && py < WIN_HEIGHT)
                    mlx_pixel_put(mlx, win, px, py, color);
            }
        }
    }
}
int has_wall_at(char **map, double x, double y)
{
    int map_x = (int)(x / TILE_SIZE);
    int map_y = (int)(y / TILE_SIZE);

    if (map_y < 0 || map_y >= MAP_NUM_ROWS || map_x < 0 || map_x >= MAP_NUM_COLS)
        return 1; // اعتبره جداراً خارج الخريطة

    return (map[map_y][map_x] == '1');
}

void draw_square(void *mlx, void *win, int x, int y, int size, int color)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            int px = x + i;
            int py = y + j;
            if (px >= 0 && px < WIN_WIDTH && py > 0 && py < WIN_HEIGHT)
                mlx_pixel_put(mlx, win, px, py, color);
        }
    }
}
void render(t_dataplyer *data, char **map)
{
     mlx_clear_window(data->mlx_ptr, data->win_ptr);
    for (int i = 0; i < MAP_NUM_ROWS; i++)
    {
        for (int j = 0; j < MAP_NUM_COLS; j++)
        {
            int tilex = j * TILE_SIZE;
            int tiley = i * TILE_SIZE;
            int color = (map[i][j] == '1') ? 0x22dd22 : 0xFFFFFF;
			
            draw_square(data->mlx_ptr, data->win_ptr, tilex, tiley, TILE_SIZE - 2, color);
			
        }
    }

  
    draw_circle(data->mlx_ptr, data->win_ptr, data->x, data->y, data->radius, 0xFF0000);
	    // int lineLength = 20;
		//  
    int endX = data->x + cos(data->rotationtangle) * 30;
    int endY = data->y + sin(data->rotationtangle) * 30;
    draw_line(data->mlx_ptr, data->win_ptr, data->x, data->y, endX, endY, 0x0000FF);
}




// رسم مربع بلون معين


void 	play_palyer( t_dataplyer *data, t_vars *vars)
{
	data->x = WINDOW_WIDTH / 2;
	data->y = WINDOW_HEIGHT / 2;
	data->radius = 8;
	data->turnDirection = 0;
	data->walkDirection = 0;
	data->sideDirection = 0;
	data->rotationspeed = 0;
	data->rotationtangle = M_PI /2;
	data->movespeed =3.0;
	data->rotationspeed = 4 * (M_PI /180);
	data->mlx_ptr = vars->mlx;
    data->win_ptr = vars->win;

}

int key_press(int keycode, t_dataplyer *data)
{
    if ( keycode == 126) // W أو سهم ↑
        data->walkDirection = +1;
    else if ( keycode == 125) // S أو سهم ↓
        data->walkDirection = -1;
    else if (keycode == 123) // A أو سهم ←
        data->turnDirection = +1;
    else if ( keycode == 124) // D أو سهم →
        data->turnDirection = -1;
    else if (keycode == 53) // ESC
    {
        mlx_destroy_window(data->mlx_ptr, data->win_ptr);
        exit(0);
    }
    return (0);
}

int key_release(int keycode, t_dataplyer *data)
{
    if ( keycode == 126 && data->walkDirection == 1)
        data->walkDirection = 0;
    else if ( keycode == 125 && data->walkDirection == -1)
        data->walkDirection = 0;
    else if ( keycode == 123 && data->turnDirection == 1)
        data->sideDirection = 0;
    else if ( keycode == 124 && data->turnDirection == -1)
        data->sideDirection = 0;
    return (0);
}

void update_player(t_dataplyer *data)
{
	data->rotationtangle += data->turnDirection * data->rotationspeed; 
    double moveStep = data->walkDirection * data->movespeed * 2.0;
    double sideStep = data->sideDirection * data->movespeed ;

    double newX = data->x + cos(data->rotationtangle) * moveStep;
    double newY = data->y + sin(data->rotationtangle) * moveStep;

    // حركة جانبية (Strafing)
    newX += cos(data->rotationtangle + M_PI_2) * sideStep;
    newY += sin(data->rotationtangle + M_PI_2) * sideStep;

	  if (!has_wall_at(data->map, newX, data->y))
    	data->x = (int)newX;
	if (!has_wall_at(data->map, data->x, newY))
    	data->y = (int)newY;
}
int loop_hook(void *param)
{
    t_dataplyer *data = (t_dataplyer *)param;
    update_player(data);             // تحديث موقع اللاعب
    render(data,data->map);              // إعادة الرسم (سنرسل الخريطة قريبًا)
    return (0);
}

void handle_event( t_dataplyer *data)
{
	mlx_hook(data->win_ptr, 2, 1L << 0, key_press, data);
	mlx_hook(data->win_ptr, 3, 1L << 1, key_release, data);

}
int main()
{
    t_vars vars;
    t_dataplyer data;
    // الخريطة كسلسلة من النصوص
    char *map[] = {
        "11111111111111111111",
        "10000000010000000001",
        "10000000010000000001",
        "10000000000000000001",
        "10000000000000011111",
        "10000000000000000001",
        "10000000000000000001",
        "10000000000000000001",
        "11111111100000000001",
        "10000000000000000001",
        "10000000000000000001",
        "11111111111111111111",
        NULL
    };
		data.map = map;
    vars.mlx = mlx_init();
    vars.win = mlx_new_window(vars.mlx, WIN_WIDTH, WIN_HEIGHT, "2D Map");

    // رسم الخريطة
    // for (int i = 0; i < MAP_NUM_ROWS; i++)
    // {
    //     for (int j = 0; j < MAP_NUM_COLS; j++)
    //     {
    //         int tilex = j * TILE_SIZE;
    //         int tiley = i * TILE_SIZE;
    //         int color = (map[i][j] == '1') ? 0x22dd22 : 0xFFFFFF; // لون كقيمة int
    //         draw_square(vars.mlx, vars.win, tilex, tiley, TILE_SIZE -  2, color);
    //     }
    // }
	// draw_circle(vars.mlx, vars.win, data.x,data.y, data.radius ,  0xFF0000);
	play_palyer(&data,&vars);
	render(&data,data.map);
	handle_event( &data );
	mlx_loop_hook(vars.mlx, loop_hook, &data);
    mlx_loop(vars.mlx);
	printf("%f",M_PI);
    return 0;
}
// #include <mlx.h>
// #include <math.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <string.h>

// #define WIN_WIDTH 1200
// #define WIN_HEIGHT 800

// #define SCALE 30
// #define ZOOM_Z 10
// #define OFFSET_X 400
// #define TILE_SIZE 32
// #define MAP_NUM_ROWS 12
// #define MAP_NUM_COLS 20
// #define WINDOW_WIDTH (MAP_NUM_COLS * TILE_SIZE)
// #define WINDOW_HEIGHT (MAP_NUM_ROWS * TILE_SIZE)

// #define OFFSET_Y 200
// #define BLOCK_SIZE 8

// typedef struct s_vars {
//     void *mlx;
//     void *win;
// } t_vars;

// typedef struct s_dataplyer {
//     int x;
//     int y;
//     int radius;
//     int turnDirection;
//     int walkDirection;
//     double rotationtangle;
//     double movespeed;
//     double rotationspeed;
//     void *mlx_ptr;
//     void *win_ptr;
//     char **map;
// } t_dataplyer;

// void draw_circle(void *mlx, void *win, int center_x, int center_y, int radius, int color)
// {
//     for (int y = -radius; y <= radius; y++)
//     {
//         for (int x = -radius; x <= radius; x++)
//         {
//             if (x * x + y * y <= radius * radius)
//             {
//                 int px = center_x + x;
//                 int py = center_y + y;
//                 if (px >= 0 && px < WIN_WIDTH && py >= 0 && py < WIN_HEIGHT)
//                     mlx_pixel_put(mlx, win, px, py, color);
//             }
//         }
//     }
// }

// void draw_square(void *mlx, void *win, int x, int y, int size, int color)
// {
//     for (int i = 0; i < size; i++)
//     {
//         for (int j = 0; j < size; j++)
//         {
//             int px = x + i;
//             int py = y + j;
//             if (px >= 0 && px < WIN_WIDTH && py > 0 && py < WIN_HEIGHT)
//                 mlx_pixel_put(mlx, win, px, py, color);
//         }
//     }
// }

// void draw_line(void *mlx, void *win, int x1, int y1, int x2, int y2, int color)
// {
//     int dx = abs(x2 - x1);
//     int dy = abs(y2 - y1);
//     int sx = x1 < x2 ? 1 : -1;
//     int sy = y1 < y2 ? 1 : -1;
//     int err = (dx > dy ? dx : -dy) / 2;
//     int e2;

//     while (1)
//     {
//         mlx_pixel_put(mlx, win, x1, y1, color);
//         if (x1 == x2 && y1 == y2) break;
//         e2 = err;
//         if (e2 > -dx) { err -= dy; x1 += sx; }
//         if (e2 < dy) { err += dx; y1 += sy; }
//     }
// }

// void render(t_dataplyer *data, char **map)
// {
//     mlx_clear_window(data->mlx_ptr, data->win_ptr);
//     for (int i = 0; i < MAP_NUM_ROWS; i++)
//     {
//         for (int j = 0; j < MAP_NUM_COLS; j++)
//         {
//             int tilex = j * TILE_SIZE;
//             int tiley = i * TILE_SIZE;
//             int color = (map[i][j] == '1') ? 0x22dd22 : 0xFFFFFF;
//             draw_square(data->mlx_ptr, data->win_ptr, tilex, tiley, TILE_SIZE - 2, color);
//         }
//     }

//     // رسم اللاعب (الدائرة)
//     draw_circle(data->mlx_ptr, data->win_ptr, data->x, data->y, data->radius, 0xFF0000);
    
//     // رسم خط اتجاه اللاعب
//     int lineLength = 20;
//     int endX = data->x + cos(data->rotationtangle) * lineLength;
//     int endY = data->y + sin(data->rotationtangle) * lineLength;
//     draw_line(data->mlx_ptr, data->win_ptr, data->x, data->y, endX, endY, 0x0000FF);
// }

// void update_player(t_dataplyer *data)
// {
//     // تدوير اللاعب
//     data->rotationtangle += data->turnDirection * data->rotationspeed;

//     // حساب الاتجاه الجديد
//     double moveStep = data->walkDirection * data->movespeed;
//     double newX = data->x + cos(data->rotationtangle) * moveStep;
//     double newY = data->y + sin(data->rotationtangle) * moveStep;

//     // تحديث الموقع (بدون تصادم الآن)
//     data->x = (int)newX;
//     data->y = (int)newY;
// }

// int loop_hook(void *param)
// {
//     t_dataplyer *data = (t_dataplyer *)param;
//     update_player(data);
//     render(data, data->map);
//     return (0);
// }

// void play_palyer(t_dataplyer *data, t_vars *vars)
// {
//     data->x = WINDOW_WIDTH / 2;
//     data->y = WINDOW_HEIGHT / 2;
//     data->radius = 8;
//     data->turnDirection = 0;
//     data->walkDirection = 0;
//     data->rotationtangle = M_PI / 2;
//     data->movespeed = 2.0;
//     data->rotationspeed = 2 * (M_PI / 180);
//     data->mlx_ptr = vars->mlx;
//     data->win_ptr = vars->win;
// }

// int key_press(int keycode, t_dataplyer *data)
// {
//     printf("Key Pressed: %d\n", keycode);
//     if (keycode == 13 || keycode == 126) // W or UP
//         data->walkDirection = 1;
//     else if (keycode == 1 || keycode == 125) // S or DOWN
//         data->walkDirection = -1;
//     else if (keycode == 124) // RIGHT
//         data->turnDirection = 1;
//     else if (keycode == 123) // LEFT
//         data->turnDirection = -1;
//     else if (keycode == 53) // ESC
//     {
//         mlx_destroy_window(data->mlx_ptr, data->win_ptr);
//         exit(0);
//     }
//     return (0);
// }

// int key_release(int keycode, t_dataplyer *data)
// {
//     printf("Key Released: %d\n", keycode);
//     if ((keycode == 13 || keycode == 126) && data->walkDirection == 1)
//         data->walkDirection = 0;
//     else if ((keycode == 1 || keycode == 125) && data->walkDirection == -1)
//         data->walkDirection = 0;
//     else if (keycode == 124 && data->turnDirection == 1)
//         data->turnDirection = 0;
//     else if (keycode == 123 && data->turnDirection == -1)
//         data->turnDirection = 0;
//     return (0);
// }

// void handle_event(t_dataplyer *data)
// {
//     mlx_hook(data->win_ptr, 2, 1L << 0, key_press, data);
//     mlx_hook(data->win_ptr, 3, 1L << 1, key_release, data);
// }

// int main()
// {
//     t_vars vars;
//     t_dataplyer data;
    
//     char *map[] = {
//         "11111111111111111111",
//         "10000000010000000001",
//         "10000000010000000001",
//         "10000000000000000001",
//         "10000000000000011111",
//         "10000000000000000001",
//         "10000000000000000001",
//         "10000000000000000001",
//         "11111111100000000001",
//         "10000000000000000001",
//         "10000000000000000001",
//         "11111111111111111111",
//         NULL
//     };
    
//     data.map = map;
//     vars.mlx = mlx_init();
//     vars.win = mlx_new_window(vars.mlx, WIN_WIDTH, WIN_HEIGHT, "2D Map");

//     play_palyer(&data, &vars);
//     render(&data, data.map);
//     handle_event(&data);
//     mlx_loop_hook(vars.mlx, loop_hook, &data);
//     mlx_loop(vars.mlx);
    
//     return 0;
// }