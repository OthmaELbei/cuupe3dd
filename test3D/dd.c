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
#define TILE_SIZE       64
#define WINDOW_WIDTH    (MAP_NUM_COLS * TILE_SIZE)
#define WINDOW_HEIGHT   (MAP_NUM_ROWS * TILE_SIZE)

#define PLAYER_SIZE     8
#define FOV_ANGLE       (60 * (M_PI / 180))
#define NUM_RAYS        WIN_WIDTH  // استخدام عدد أشعة يساوي عرض النافذة للحصول على جودة أفضل
#define WALL_STRIP_WIDTH (WIN_WIDTH / NUM_RAYS)

// إضافة epsilon للمقارنات العائمة
#define EPSILON 1e-6

typedef struct s_img {
    void *img_ptr;
    char *addr;
    int bits_per_pixel;
    int line_length;
    int endian;
} t_img;

typedef struct s_player {
    double x;
    double y;
    double rotation_angle;
    double move_speed;
    double rotation_speed;
    int turn_direction;
    int walk_direction;
    int side_direction;
} t_player;

typedef struct s_ray {
    double ray_angle;
    double wall_hit_x;
    double wall_hit_y;
    double distance;
    int was_hit_vertical;
    int wall_hit_content;
} t_ray;

typedef struct s_game {
    void *mlx;
    void *win;
    t_img img;
    t_player player;
    t_ray rays[NUM_RAYS];
    char map[MAP_NUM_ROWS][MAP_NUM_COLS];
    int show_minimap;
} t_game;

/* Utility Functions */
double normalize_angle(double angle) {
    angle = fmod(angle, 2 * M_PI);
    if (angle < 0) angle += (2 * M_PI);
    return angle;
}

double distance_between_points(double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

/* Drawing Functions */
void my_mlx_pixel_put(t_img *img, int x, int y, int color) {
    if (x >= 0 && x < WIN_WIDTH && y >= 0 && y < WIN_HEIGHT) {
        char *dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
        *(unsigned int*)dst = color;
    }
}

void draw_rectangle(t_img *img, int x, int y, int width, int height, int color) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (x + i >= 0 && x + i < WIN_WIDTH && y + j >= 0 && y + j < WIN_HEIGHT) {
                my_mlx_pixel_put(img, x + i, y + j, color);
            }
        }
    }
}

void draw_line(t_img *img, int x0, int y0, int x1, int y1, int color) {
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

void draw_circle(t_img *img, int center_x, int center_y, int radius, int color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                int px = center_x + x;
                int py = center_y + y;
                if (px >= 0 && px < WIN_WIDTH && py >= 0 && py < WIN_HEIGHT) {
                    my_mlx_pixel_put(img, px, py, color);
                }
            }
        }
    }
}

/* Game Functions */
int has_wall_at(t_game *game, double x, double y) {
    if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
        return 1;

    int map_x = (int)(x / TILE_SIZE);
    int map_y = (int)(y / TILE_SIZE);

    if (map_y < 0 || map_y >= MAP_NUM_ROWS || map_x < 0 || map_x >= MAP_NUM_COLS)
        return 1;

    return (game->map[map_y][map_x] == '1');
}

void cast_ray(t_game *game, double ray_angle, int ray_id) {
    ray_angle = normalize_angle(ray_angle);
    
    t_ray *ray = &game->rays[ray_id];
    ray->ray_angle = ray_angle;
    ray->distance = INFINITY;
    ray->was_hit_vertical = 0;
    ray->wall_hit_content = '0';
    
    double horizontal_distance = INFINITY;
    double vertical_distance = INFINITY;
    double horz_hit_x = 0, horz_hit_y = 0;
    double vert_hit_x = 0, vert_hit_y = 0;
    int horz_wall_content = '0', vert_wall_content = '0';
    
    // === Horizontal intersections ===
    double is_ray_facing_down = sin(ray_angle) > 0;
    double is_ray_facing_up = !is_ray_facing_down;
    double is_ray_facing_right = cos(ray_angle) > 0;
    double is_ray_facing_left = !is_ray_facing_right;
    
    double yintercept = floor(game->player.y / TILE_SIZE) * TILE_SIZE;
    if (is_ray_facing_down)
        yintercept += TILE_SIZE;
    
    double xintercept = game->player.x + (yintercept - game->player.y) / tan(ray_angle);
    
    double ystep = TILE_SIZE;
    if (is_ray_facing_up)
        ystep *= -1;
    
    double xstep = TILE_SIZE / tan(ray_angle);
    if ((is_ray_facing_left && xstep > 0) || (is_ray_facing_right && xstep < 0))
        xstep *= -1;
    
    double next_horz_x = xintercept;
    double next_horz_y = yintercept;
    
    // تحسين حلقة البحث عن التقاطعات الأفقية
    while (next_horz_x >= 0 && next_horz_x < WINDOW_WIDTH && 
           next_horz_y >= 0 && next_horz_y < WINDOW_HEIGHT) {
        double x_to_check = next_horz_x;
        double y_to_check = next_horz_y + (is_ray_facing_up ? -1 : 0);
        
        if (has_wall_at(game, x_to_check, y_to_check)) {
            horz_hit_x = next_horz_x;
            horz_hit_y = next_horz_y;
            horizontal_distance = distance_between_points(game->player.x, game->player.y, 
                                                         horz_hit_x, horz_hit_y);
            // التحقق من صحة الإحداثيات قبل الوصول للخريطة
            int map_x = (int)(x_to_check / TILE_SIZE);
            int map_y = (int)(y_to_check / TILE_SIZE);
            if (map_y >= 0 && map_y < MAP_NUM_ROWS && map_x >= 0 && map_x < MAP_NUM_COLS) {
                horz_wall_content = game->map[map_y][map_x];
            }
            break;
        }
        
        next_horz_x += xstep;
        next_horz_y += ystep;
    }

    // === Vertical intersections ===
    xintercept = floor(game->player.x / TILE_SIZE) * TILE_SIZE;
    if (is_ray_facing_right)
        xintercept += TILE_SIZE;
    
    yintercept = game->player.y + (xintercept - game->player.x) * tan(ray_angle);
    
    xstep = TILE_SIZE;
    if (is_ray_facing_left)
        xstep *= -1;
    
    ystep = TILE_SIZE * tan(ray_angle);
    if ((is_ray_facing_up && ystep > 0) || (is_ray_facing_down && ystep < 0))
        ystep *= -1;
    
    double next_vert_x = xintercept;
    double next_vert_y = yintercept;
    
    // تحسين حلقة البحث عن التقاطعات العمودية
    while (next_vert_x >= 0 && next_vert_x < WINDOW_WIDTH && 
           next_vert_y >= 0 && next_vert_y < WINDOW_HEIGHT) {
        double x_to_check = next_vert_x + (is_ray_facing_left ? -1 : 0);
        double y_to_check = next_vert_y;
        
        if (has_wall_at(game, x_to_check, y_to_check)) {
            vert_hit_x = next_vert_x;
            vert_hit_y = next_vert_y;
            vertical_distance = distance_between_points(game->player.x, game->player.y, 
                                                       vert_hit_x, vert_hit_y);
            // التحقق من صحة الإحداثيات قبل الوصول للخريطة
            int map_x = (int)(x_to_check / TILE_SIZE);
            int map_y = (int)(y_to_check / TILE_SIZE);
            if (map_y >= 0 && map_y < MAP_NUM_ROWS && map_x >= 0 && map_x < MAP_NUM_COLS) {
                vert_wall_content = game->map[map_y][map_x];
            }
            break;
        }
        
        next_vert_x += xstep;
        next_vert_y += ystep;
    }
    
    // اختيار أقرب تقاطع
    if (horizontal_distance < vertical_distance) {
        ray->wall_hit_x = horz_hit_x;
        ray->wall_hit_y = horz_hit_y;
        ray->distance = horizontal_distance;
        ray->was_hit_vertical = 0;
        ray->wall_hit_content = horz_wall_content;
    } else {
        ray->wall_hit_x = vert_hit_x;
        ray->wall_hit_y = vert_hit_y;
        ray->distance = vertical_distance;
        ray->was_hit_vertical = 1;
        ray->wall_hit_content = vert_wall_content;
    }
}

void cast_all_rays(t_game *game) {
    double ray_angle = game->player.rotation_angle - (FOV_ANGLE / 2);
    
    for (int i = 0; i < NUM_RAYS; i++) {
        cast_ray(game, normalize_angle(ray_angle), i);
        ray_angle += FOV_ANGLE / NUM_RAYS;
    }
}

void render_3d_view(t_game *game) {
    // مسح الشاشة بألوان محسنة
    draw_rectangle(&game->img, 0, 0, WIN_WIDTH, WIN_HEIGHT / 2, 0x87CEEB); // سماء أزرق فاتح
    draw_rectangle(&game->img, 0, WIN_HEIGHT / 2, WIN_WIDTH, WIN_HEIGHT / 2, 0x8B4513); // أرضية بنية
    
    // رسم الجدران
    for (int i = 0; i < NUM_RAYS; i++) {
        t_ray *ray = &game->rays[i];
        
        if (ray->distance == INFINITY) continue;
        
        // تصحيح تأثير عين السمكة
        double corrected_distance = ray->distance * cos(ray->ray_angle - game->player.rotation_angle);
        
        // منع القسمة على صفر
        if (corrected_distance < EPSILON) corrected_distance = EPSILON;
        
        // حساب ارتفاع الجدار
        double wall_height = (TILE_SIZE / corrected_distance) * ((WIN_WIDTH / 2) / tan(FOV_ANGLE / 2));
        
        // حساب البكسل العلوي والسفلي
        int wall_top = (WIN_HEIGHT / 2) - (wall_height / 2);
        wall_top = wall_top < 0 ? 0 : wall_top;
        
        int wall_bottom = (WIN_HEIGHT / 2) + (wall_height / 2);
        wall_bottom = wall_bottom > WIN_HEIGHT ? WIN_HEIGHT : wall_bottom;
        
        // اختيار لون الجدار بناءً على الاتجاه مع إضافة تظليل محسن
        int base_color = ray->was_hit_vertical ? 0xFF6B8B : 0x4ECDC4;
        
        // تحسين التظليل
        double max_distance = TILE_SIZE * 10;
        double shade = 1.0 - fmin(ray->distance / max_distance, 0.7);
        shade = fmax(shade, 0.2); // التأكد من وجود حد أدنى للإضاءة
        
        int color = ((int)((base_color >> 16 & 0xFF) * shade) << 16) |
                    ((int)((base_color >> 8 & 0xFF) * shade) << 8) |
                    (int)((base_color & 0xFF) * shade);
        
        // رسم شريحة الجدار
        int strip_width = (WIN_WIDTH / NUM_RAYS) > 0 ? (WIN_WIDTH / NUM_RAYS) : 1;
        draw_rectangle(&game->img, i * strip_width, wall_top, 
                      strip_width, wall_bottom - wall_top, color);
    }
}

void render_minimap(t_game *game) {
    if (!game->show_minimap) return;
    
    int minimap_size = 200;
    int minimap_x = WIN_WIDTH - minimap_size - 20;
    int minimap_y = 20;
    float scale = (float)minimap_size / WINDOW_WIDTH;
    
    // رسم خلفية الخريطة المصغرة بشفافية محسنة
    draw_rectangle(&game->img, minimap_x - 2, minimap_y - 2, 
                   minimap_size + 4, minimap_size + 4, 0x0000000);
    draw_rectangle(&game->img, minimap_x, minimap_y, 
                   minimap_size, minimap_size, 0x1a1a1a);
    
    // رسم الخريطة
    for (int y = 0; y < MAP_NUM_ROWS; y++) {
        for (int x = 0; x < MAP_NUM_COLS; x++) {
            int tile_x = minimap_x + x * TILE_SIZE * scale;
            int tile_y = minimap_y + y * TILE_SIZE * scale;
            int tile_size = TILE_SIZE * scale;
            
            if (game->map[y][x] == '1') {
                draw_rectangle(&game->img, tile_x, tile_y, tile_size, tile_size, 0xFFFFFF);
            } else {
                draw_rectangle(&game->img, tile_x, tile_y, tile_size, tile_size, 0x333333);
            }
        }
    }
    
    // رسم اللاعب
    int player_x = minimap_x + game->player.x * scale;
    int player_y = minimap_y + game->player.y * scale;
    draw_circle(&game->img, player_x, player_y, PLAYER_SIZE * scale, 0xFF0000);
    
    // رسم اتجاه اللاعب
    int line_end_x = player_x + cos(game->player.rotation_angle) * 30;
    int line_end_y = player_y + sin(game->player.rotation_angle) * 30;
    draw_line(&game->img, player_x, player_y, line_end_x, line_end_y, 0x0000FF);
    
    // رسم الأشعة (كل 10 أشعة لتحسين الأداء)
    for (int i = 0; i < NUM_RAYS; i += 10) {
        t_ray *ray = &game->rays[i];
        if (ray->distance != INFINITY) {
            int ray_end_x = minimap_x + (ray->wall_hit_x) * scale;
            int ray_end_y = minimap_y + (ray->wall_hit_y) * scale;
            
            // التأكد من أن النقاط داخل الخريطة المصغرة
            if (ray_end_x >= minimap_x && ray_end_x < minimap_x + minimap_size &&
                ray_end_y >= minimap_y && ray_end_y < minimap_y + minimap_size) {
                draw_line(&game->img, player_x, player_y, ray_end_x, ray_end_y, 0xFFFF00);
            }
        }
    }
}

void render(t_game *game) {
    // مسح الصورة
    memset(game->img.addr, 0, WIN_HEIGHT * game->img.line_length);
    
    // رسم المنظر ثلاثي الأبعاد
    render_3d_view(game);
    
    // رسم الخريطة المصغرة
    render_minimap(game);
    
    // عرض الصورة
    mlx_put_image_to_window(game->mlx, game->win, game->img.img_ptr, 0, 0);
}

void move_player(t_game *game) {
    double move_step = game->player.walk_direction * game->player.move_speed;
    double side_step = game->player.side_direction * game->player.move_speed;
    
    double new_player_x = game->player.x;
    double new_player_y = game->player.y;
    
    // حساب الموقع الجديد للحركة للأمام/الخلف
    if (move_step != 0) {
        new_player_x += cos(game->player.rotation_angle) * move_step;
        new_player_y += sin(game->player.rotation_angle) * move_step;
    }
    
    // حساب الموقع الجديد للحركة الجانبية
    if (side_step != 0) {
        new_player_x += cos(game->player.rotation_angle + M_PI_2) * side_step;
        new_player_y += sin(game->player.rotation_angle + M_PI_2) * side_step;
    }
    
    // فحص التصادمات بشكل منفصل للمحور x و y مع هامش أمان
    double collision_margin = PLAYER_SIZE / 2;
    if (!has_wall_at(game, new_player_x + collision_margin, game->player.y) && 
        !has_wall_at(game, new_player_x - collision_margin, game->player.y)) {
        game->player.x = new_player_x;
    }
    if (!has_wall_at(game, game->player.x, new_player_y + collision_margin) &&
        !has_wall_at(game, game->player.x, new_player_y - collision_margin)) {
        game->player.y = new_player_y;
    }
    
    // تحديث الدوران
    if (game->player.turn_direction != 0) {
        game->player.rotation_angle += game->player.turn_direction * game->player.rotation_speed;
        game->player.rotation_angle = normalize_angle(game->player.rotation_angle);
    }
}

int key_press(int keycode, t_game *game) {
    if (keycode == 53) { // ESC
        mlx_destroy_image(game->mlx, game->img.img_ptr);
        mlx_destroy_window(game->mlx, game->win);
        exit(0);
    }
    else if (keycode == 13 || keycode == 126) // W or UP
        game->player.walk_direction = 1;
    else if (keycode == 1 || keycode == 125) // S or DOWN
        game->player.walk_direction = -1;
    else if (keycode == 0) // A
        game->player.side_direction = -1;
    else if (keycode == 2) // D
        game->player.side_direction = 1;
    else if (keycode == 123) // LEFT
        game->player.turn_direction = -1;
    else if (keycode == 124) // RIGHT
        game->player.turn_direction = 1;
    else if (keycode == 46) // M
        game->show_minimap = !game->show_minimap;
    return 0;
}

int key_release(int keycode, t_game *game) {
    if (keycode == 13 || keycode == 126) // W or UP
        game->player.walk_direction = 0;
    else if (keycode == 1 || keycode == 125) // S or DOWN
        game->player.walk_direction = 0;
    else if (keycode == 0) // A
        game->player.side_direction = 0;
    else if (keycode == 2) // D
        game->player.side_direction = 0;
    else if (keycode == 123) // LEFT
        game->player.turn_direction = 0;
    else if (keycode == 124) // RIGHT
        game->player.turn_direction = 0;
    return 0;
}

int game_loop(t_game *game) {
    move_player(game);
    cast_all_rays(game);
    render(game);
    return 0;
}

void init_game(t_game *game) {
    game->mlx = mlx_init();
    if (!game->mlx) {
        printf("خطأ في تهيئة MLX\n");
        exit(1);
    }
    
    game->win = mlx_new_window(game->mlx, WIN_WIDTH, WIN_HEIGHT, "3D Raycaster - محسن");
    if (!game->win) {
        printf("خطأ في إنشاء النافذة\n");
        exit(1);
    }
    
    game->img.img_ptr = mlx_new_image(game->mlx, WIN_WIDTH, WIN_HEIGHT);
    if (!game->img.img_ptr) {
        printf("خطأ في إنشاء الصورة\n");
        exit(1);
    }
    
    game->img.addr = mlx_get_data_addr(game->img.img_ptr, &game->img.bits_per_pixel, 
                                      &game->img.line_length, &game->img.endian);
    
    // تهيئة اللاعب
    game->player.x = WINDOW_WIDTH / 2;
    game->player.y = WINDOW_HEIGHT / 2;
    game->player.rotation_angle = M_PI / 2;
    game->player.move_speed = 3.0;
    game->player.rotation_speed = 3 * (M_PI / 180); // سرعة دوران محسنة
    game->player.turn_direction = 0;
    game->player.walk_direction = 0;
    game->player.side_direction = 0;
    
    // تهيئة الخريطة
    char *map[MAP_NUM_ROWS] = {
        "11111111111111111111",
        "10000000010000000001",
        "10000000010000000001",
        "10000011110000000001",
        "10000000010000011111",
        "10000000011110011001",
        "10000000000000001001",
        "10000000000000001001",
        "11111111100000001001",
        "10000000000000101001",
        "10000000000000100001",
        "11111111111111111111"
    };
    
    for (int y = 0; y < MAP_NUM_ROWS; y++) {
        for (int x = 0; x < MAP_NUM_COLS; x++) {
            game->map[y][x] = map[y][x];
        }
    }
    
    game->show_minimap = 1;
}

int close_window(t_game *game) {
    mlx_destroy_image(game->mlx, game->img.img_ptr);
    mlx_destroy_window(game->mlx, game->win);
    exit(0);
    return 0;
}

int main() {
    t_game game;
    
    init_game(&game);
    
    // إعداد معالجات الأحداث
    mlx_hook(game.win, 2, 1L << 0, key_press, &game);
    mlx_hook(game.win, 3, 1L << 1, key_release, &game);
    mlx_hook(game.win, 17, 0, close_window, &game); // معالج إغلاق النافذة محسن
    
    // بدء حلقة اللعبة
    mlx_loop_hook(game.mlx, game_loop, &game);
    mlx_loop(game.mlx);
    
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

// #define MAP_NUM_ROWS    12
// #define MAP_NUM_COLS    20
// #define TILE_SIZE       64
// #define WINDOW_WIDTH    (MAP_NUM_COLS * TILE_SIZE)
// #define WINDOW_HEIGHT   (MAP_NUM_ROWS * TILE_SIZE)

// #define PLAYER_SIZE     8
// #define FOV_ANGLE       (60 * (M_PI / 180))
// #define NUM_RAYS        120
// #define WALL_STRIP_WIDTH (WIN_WIDTH / NUM_RAYS)

// typedef struct s_img {
//     void *img_ptr;
//     char *addr;
//     int bits_per_pixel;
//     int line_length;
//     int endian;
// } t_img;

// typedef struct s_player {
//     double x;
//     double y;
//     double rotation_angle;
//     double move_speed;
//     double rotation_speed;
//     int turn_direction;
//     int walk_direction;
//     int side_direction;
// } t_player;

// typedef struct s_ray {
//     double ray_angle;
//     double wall_hit_x;
//     double wall_hit_y;
//     double distance;
//     int was_hit_vertical;
//     int wall_hit_content;
// } t_ray;

// typedef struct s_game {
//     void *mlx;
//     void *win;
//     t_img img;
//     t_player player;
//     t_ray rays[NUM_RAYS];
//     char map[MAP_NUM_ROWS][MAP_NUM_COLS];
//     int show_minimap;
// } t_game;

// /* Utility Functions */
// double normalize_angle(double angle) {
//     angle = fmod(angle, 2 * M_PI);
//     if (angle < 0) angle += (2 * M_PI);
//     return angle;
// }

// double distance_between_points(double x1, double y1, double x2, double y2) {
//     return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
// }

// /* Drawing Functions */
// void my_mlx_pixel_put(t_img *img, int x, int y, int color) {
//     if (x >= 0 && x < WIN_WIDTH && y >= 0 && y < WIN_HEIGHT) {
//         char *dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
//         *(unsigned int*)dst = color;
//     }
// }

// void draw_rectangle(t_img *img, int x, int y, int width, int height, int color) {
//     for (int i = 0; i < width; i++) {
//         for (int j = 0; j < height; j++) {
//             if (x + i >= 0 && x + i < WIN_WIDTH && y + j >= 0 && y + j < WIN_HEIGHT) {
//                 my_mlx_pixel_put(img, x + i, y + j, color);
//             }
//         }
//     }
// }

// void draw_line(t_img *img, int x0, int y0, int x1, int y1, int color) {
//     int dx = abs(x1 - x0);
//     int dy = -abs(y1 - y0);
//     int sx = (x0 < x1) ? 1 : -1;
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

// void draw_circle(t_img *img, int center_x, int center_y, int radius, int color) {
//     for (int y = -radius; y <= radius; y++) {
//         for (int x = -radius; x <= radius; x++) {
//             if (x * x + y * y <= radius * radius) {
//                 int px = center_x + x;
//                 int py = center_y + y;
//                 if (px >= 0 && px < WIN_WIDTH && py >= 0 && py < WIN_HEIGHT) {
//                     my_mlx_pixel_put(img, px, py, color);
//                 }
//             }
//         }
//     }
// }

// /* Game Functions */
// int has_wall_at(t_game *game, double x, double y) {
//     if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
//         return 1;

//     int map_x = (int)(x / TILE_SIZE);
//     int map_y = (int)(y / TILE_SIZE);

//     if (map_y < 0 || map_y >= MAP_NUM_ROWS || map_x < 0 || map_x >= MAP_NUM_COLS)
//         return 1;

//     return (game->map[map_y][map_x] == '1');
// }

// void cast_ray(t_game *game, double ray_angle, int ray_id) {
//     ray_angle = normalize_angle(ray_angle);
    
//     t_ray *ray = &game->rays[ray_id];
//     ray->ray_angle = ray_angle;
//     ray->distance = INFINITY;
//     ray->was_hit_vertical = 0;
    
//     // Horizontal intersections
//     double yintercept = floor(game->player.y / TILE_SIZE) * TILE_SIZE;
//     yintercept += (sin(ray_angle) > 0 ? TILE_SIZE : 0);
    
//     double xintercept = game->player.x + (yintercept - game->player.y) / tan(ray_angle);
    
//     double ystep = TILE_SIZE;
//     ystep *= (sin(ray_angle) > 0 ? 1 : -1);
    
//     double xstep = TILE_SIZE / tan(ray_angle);
//     xstep *= (cos(ray_angle) < 0 ? -1 : 1);
    
//     double next_horz_x = xintercept;
//     double next_horz_y = yintercept;
    
//     while (next_horz_x >= 0 && next_horz_x < WINDOW_WIDTH && 
//            next_horz_y >= 0 && next_horz_y < WINDOW_HEIGHT) {
//         double x_to_check = next_horz_x;
//         double y_to_check = next_horz_y + ((sin(ray_angle) > 0 ? 0 : -1));
        
//         if (has_wall_at(game, x_to_check, y_to_check)) {
//             ray->wall_hit_x = next_horz_x;
//             ray->wall_hit_y = next_horz_y;
//             ray->distance = distance_between_points(game->player.x, game->player.y, 
//                                                    ray->wall_hit_x, ray->wall_hit_y);
//             ray->was_hit_vertical = 0;
//             ray->wall_hit_content = game->map[(int)y_to_check/TILE_SIZE][(int)x_to_check/TILE_SIZE];
//             break;
//         }
        
//         next_horz_x += xstep;
//         next_horz_y += ystep;
//     }

//     // Vertical intersections
//     xintercept = floor(game->player.x / TILE_SIZE) * TILE_SIZE;
//     xintercept += (cos(ray_angle) > 0 ? TILE_SIZE : 0);
    
//     yintercept = game->player.y + (xintercept - game->player.x) * tan(ray_angle);
    
//     xstep = TILE_SIZE;
//     xstep *= (cos(ray_angle) > 0 ? 1 : -1);
    
//     ystep = TILE_SIZE * tan(ray_angle);
//     ystep *= (sin(ray_angle) > 0 ? 1 : -1);
    
//     double next_vert_x = xintercept;
//     double next_vert_y = yintercept;
    
//     while (next_vert_x >= 0 && next_vert_x < WINDOW_WIDTH && 
//            next_vert_y >= 0 && next_vert_y < WINDOW_HEIGHT) {
//         double x_to_check = next_vert_x + ((cos(ray_angle) < 0 ? -1 : 0));
//         double y_to_check = next_vert_y;
        
//         if (has_wall_at(game, x_to_check, y_to_check)) {
//             double distance = distance_between_points(game->player.x, game->player.y, 
//                                                     next_vert_x, next_vert_y);
//             if (distance < ray->distance) {
//                 ray->wall_hit_x = next_vert_x;
//                 ray->wall_hit_y = next_vert_y;
//                 ray->distance = distance;
//                 ray->was_hit_vertical = 1;
//                 ray->wall_hit_content = game->map[(int)y_to_check/TILE_SIZE][(int)x_to_check/TILE_SIZE];
//             }
//             break;
//         }
        
//         next_vert_x += xstep;
//         next_vert_y += ystep;
//     }
// }

// void cast_all_rays(t_game *game) {
//     double ray_angle = game->player.rotation_angle - (FOV_ANGLE / 2);
    
//     for (int i = 0; i < NUM_RAYS; i++) {
//         cast_ray(game, normalize_angle(ray_angle), i);
//         ray_angle += FOV_ANGLE / NUM_RAYS;
//     }
// }

// void render_3d_view(t_game *game) {
//     // Clear screen
//     draw_rectangle(&game->img, 0, 0, WIN_WIDTH, WIN_HEIGHT / 2, 0x333333); // Ceiling
//     draw_rectangle(&game->img, 0, WIN_HEIGHT / 2, WIN_WIDTH, WIN_HEIGHT / 2, 0x555555); // Floor
    
//     // Render walls
//     for (int i = 0; i < NUM_RAYS; i++) {
//         t_ray *ray = &game->rays[i];
        
//         // Correct fish-eye effect
//         double corrected_distance = ray->distance * cos(ray->ray_angle - game->player.rotation_angle);
        
//         // Calculate wall height
//         double wall_height = (TILE_SIZE / corrected_distance) * ((WIN_WIDTH / 2) / tan(FOV_ANGLE / 2));
        
//         // Calculate top and bottom pixels
//         int wall_top = (WIN_HEIGHT / 2) - (wall_height / 2);
//         wall_top = wall_top < 0 ? 0 : wall_top;
        
//         int wall_bottom = (WIN_HEIGHT / 2) + (wall_height / 2);
//         wall_bottom = wall_bottom > WIN_HEIGHT ? WIN_HEIGHT : wall_bottom;
        
//         // Choose wall color based on orientation and add shading
//         int base_color = ray->was_hit_vertical ? 0xCC3333 : 0x33CC33;
//         double shade = 1.0 - fmin(ray->distance / (TILE_SIZE * 8), 0.8);
//         int color = ((int)((base_color >> 16 & 0xFF) * shade) << 16) |
//                     ((int)((base_color >> 8 & 0xFF) * shade) << 8) |
//                     (int)((base_color & 0xFF) * shade);
        
//         // Draw wall slice
//         draw_rectangle(&game->img, i * WALL_STRIP_WIDTH, wall_top, 
//                       WALL_STRIP_WIDTH, wall_bottom - wall_top, color);
//     }
// }

// void render_minimap(t_game *game) {
//     if (!game->show_minimap) return;
    
//     int minimap_size = 200;
//     int minimap_x = WIN_WIDTH - minimap_size - 20;
//     int minimap_y = 20;
//     float scale = (float)minimap_size / WINDOW_WIDTH;
    
//     // Draw minimap background with transparency
//     for (int y = 0; y < minimap_size; y++) {
//         for (int x = 0; x < minimap_size; x++) {
//             int px = minimap_x + x;
//             int py = minimap_y + y;
//             if (px >= 0 && px < WIN_WIDTH && py >= 0 && py < WIN_HEIGHT) {
//                 unsigned int *dst = (unsigned int *)(game->img.addr + 
//                     (py * game->img.line_length + px * (game->img.bits_per_pixel / 8)));
//                 *dst = (*dst & 0xFEFEFE) >> 1; // Darken existing pixels
//             }
//         }
//     }
    
//     // Draw map
//     for (int y = 0; y < MAP_NUM_ROWS; y++) {
//         for (int x = 0; x < MAP_NUM_COLS; x++) {
//             if (game->map[y][x] == '1') {
//                 int tile_x = minimap_x + x * TILE_SIZE * scale;
//                 int tile_y = minimap_y + y * TILE_SIZE * scale;
//                 int tile_size = TILE_SIZE * scale;
//                 draw_rectangle(&game->img, tile_x, tile_y, tile_size, tile_size, 0x22DD22);
//             }
//         }
//     }
    
//     // Draw player
//     int player_x = minimap_x + game->player.x * scale;
//     int player_y = minimap_y + game->player.y * scale;
//     draw_circle(&game->img, player_x, player_y, PLAYER_SIZE * scale, 0xFF0000);
    
//     // Draw player direction
//     int line_end_x = player_x + cos(game->player.rotation_angle) * 20;
//     int line_end_y = player_y + sin(game->player.rotation_angle) * 20;
//     draw_line(&game->img, player_x, player_y, line_end_x, line_end_y, 0x0000FF);
    
//     // Draw rays
//     for (int i = 0; i < NUM_RAYS; i += 4) {
//         t_ray *ray = &game->rays[i];
//         int ray_end_x = player_x + (ray->wall_hit_x - game->player.x) * scale;
//         int ray_end_y = player_y + (ray->wall_hit_y - game->player.y) * scale;
//         draw_line(&game->img, player_x, player_y, ray_end_x, ray_end_y, 0xFFFF00);
//     }
// }

// void render(t_game *game) {
//     // Clear image
//     memset(game->img.addr, 0, WIN_HEIGHT * game->img.line_length);
    
//     // Render 3D view
//     render_3d_view(game);
    
//     // Render minimap
//     render_minimap(game);
    
//     // Display image
//     mlx_put_image_to_window(game->mlx, game->win, game->img.img_ptr, 0, 0);
// }

// void move_player(t_game *game) {
//     double move_step = game->player.walk_direction * game->player.move_speed;
//     double side_step = game->player.side_direction * game->player.move_speed;
    
//     double new_player_x = game->player.x;
//     double new_player_y = game->player.y;
    
//     // Calculate new position for forward/backward movement
//     if (move_step != 0) {
//         new_player_x += cos(game->player.rotation_angle) * move_step;
//         new_player_y += sin(game->player.rotation_angle) * move_step;
//     }
    
//     // Calculate new position for strafing
//     if (side_step != 0) {
//         new_player_x += cos(game->player.rotation_angle + M_PI_2) * side_step;
//         new_player_y += sin(game->player.rotation_angle + M_PI_2) * side_step;
//     }
    
//     // Check collisions separately for x and y
//     if (!has_wall_at(game, new_player_x, game->player.y)) {
//         game->player.x = new_player_x;
//     }
//     if (!has_wall_at(game, game->player.x, new_player_y)) {
//         game->player.y = new_player_y;
//     }
    
//     // Update rotation
//     if (game->player.turn_direction != 0) {
//         game->player.rotation_angle += game->player.turn_direction * game->player.rotation_speed;
//         game->player.rotation_angle = normalize_angle(game->player.rotation_angle);
//     }
// }

// int key_press(int keycode, t_game *game) {
//     if (keycode == 53) { // ESC
//         mlx_destroy_image(game->mlx, game->img.img_ptr);
//         mlx_destroy_window(game->mlx, game->win);
//         exit(0);
//     }
//     else if (keycode == 13 || keycode == 126) // W or UP
//         game->player.walk_direction = 1;
//     else if (keycode == 1 || keycode == 125) // S or DOWN
//         game->player.walk_direction = -1;
//     else if (keycode == 0) // A
//         game->player.side_direction = -1;
//     else if (keycode == 2) // D
//         game->player.side_direction = 1;
//     else if (keycode == 123) // LEFT
//         game->player.turn_direction = -1;
//     else if (keycode == 124) // RIGHT
//         game->player.turn_direction = 1;
//     else if (keycode == 46) // M
//         game->show_minimap = !game->show_minimap;
//     return 0;
// }

// int key_release(int keycode, t_game *game) {
//     if (keycode == 13 || keycode == 126) // W or UP
//         game->player.walk_direction = 0;
//     else if (keycode == 1 || keycode == 125) // S or DOWN
//         game->player.walk_direction = 0;
//     else if (keycode == 0) // A
//         game->player.side_direction = 0;
//     else if (keycode == 2) // D
//         game->player.side_direction = 0;
//     else if (keycode == 123) // LEFT
//         game->player.turn_direction = 0;
//     else if (keycode == 124) // RIGHT
//         game->player.turn_direction = 0;
//     return 0;
// }

// int game_loop(t_game *game) {
//     move_player(game);
//     cast_all_rays(game);
//     render(game);
//     return 0;
// }

// void init_game(t_game *game) {
//     game->mlx = mlx_init();
//     game->win = mlx_new_window(game->mlx, WIN_WIDTH, WIN_HEIGHT, "2D Raycaster");
//     game->img.img_ptr = mlx_new_image(game->mlx, WIN_WIDTH, WIN_HEIGHT);
//     game->img.addr = mlx_get_data_addr(game->img.img_ptr, &game->img.bits_per_pixel, 
//                                       &game->img.line_length, &game->img.endian);
    
//     // Initialize player
//     game->player.x = WINDOW_WIDTH / 2;
//     game->player.y = WINDOW_HEIGHT / 2;
//     game->player.rotation_angle = M_PI / 2;
//     game->player.move_speed = 3.0;
//     game->player.rotation_speed = 2 * (M_PI / 180);
//     game->player.turn_direction = 0;
//     game->player.walk_direction = 0;
//     game->player.side_direction = 0;
    
//     // Initialize map
//     char *map[MAP_NUM_ROWS] = {
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
//         "11111111111111111111"
//     };
    
//     for (int y = 0; y < MAP_NUM_ROWS; y++) {
//         for (int x = 0; x < MAP_NUM_COLS; x++) {
//             game->map[y][x] = map[y][x];
//         }
//     }
    
//     game->show_minimap = 1;
// }

// int main() {
//     t_game game;
    
//     init_game(&game);
    
//     // Set up event handlers
//     mlx_hook(game.win, 2, 1L << 0, key_press, &game);
//     mlx_hook(game.win, 3, 1L << 1, key_release, &game);
//     mlx_hook(game.win, 17, 0, (int (*)())exit, 0); // Handle window close
    
//     // Start game loop
//     mlx_loop_hook(game.mlx, game_loop, &game);
//     mlx_loop(game.mlx);
    
//     return 0;
// }
// #include <mlx.h>
// #include <math.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <string.h>

// #define WIN_WIDTH 1200 
// #define WIN_HEIGHT 800 

// #define MAP_NUM_ROWS    12
// #define MAP_NUM_COLS    20
// #define TILE_SIZE       64
// #define WINDOW_WIDTH    (MAP_NUM_COLS * TILE_SIZE)
// #define WINDOW_HEIGHT   (MAP_NUM_ROWS * TILE_SIZE)

// #define PLAYER_SIZE     8
// #define FOV_ANGLE       (60 * (M_PI / 180))
// #define NUM_RAYS        120
// #define WALL_STRIP_WIDTH (WIN_WIDTH / NUM_RAYS)

// typedef struct s_img {
//     void *img_ptr;
//     char *addr;
//     int bits_per_pixel;
//     int line_length;
//     int endian;
// } t_img;

// typedef struct s_player {
//     double x;
//     double y;
//     double rotation_angle;
//     double move_speed;
//     double rotation_speed;
//     int turn_direction;
//     int walk_direction;
//     int side_direction;
// } t_player;

// typedef struct s_ray {
//     double ray_angle;
//     double wall_hit_x;
//     double wall_hit_y;
//     double distance;
//     int was_hit_vertical;
//     int wall_hit_content;
// } t_ray;

// typedef struct s_game {
//     void *mlx;
//     void *win;
//     t_img img;
//     t_player player;
//     t_ray rays[NUM_RAYS];
//     char map[MAP_NUM_ROWS][MAP_NUM_COLS];
//     int show_minimap;
// } t_game;

// /* Utility Functions */
// double normalize_angle(double angle) {
//     angle = fmod(angle, 2 * M_PI);
//     if (angle < 0) angle += (2 * M_PI);
//     return angle;
// }

// double distance_between_points(double x1, double y1, double x2, double y2) {
//     return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
// }

// /* Drawing Functions */
// void my_mlx_pixel_put(t_img *img, int x, int y, int color) {
//     if (x >= 0 && x < WIN_WIDTH && y >= 0 && y < WIN_HEIGHT) {
//         char *dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
//         *(unsigned int*)dst = color;
//     }
// }

// void draw_rectangle(t_img *img, int x, int y, int width, int height, int color) {
//     for (int i = 0; i < width; i++) {
//         for (int j = 0; j < height; j++) {
//             my_mlx_pixel_put(img, x + i, y + j, color);
//         }
//     }
// }

// void draw_line(t_img *img, int x0, int y0, int x1, int y1, int color)
// {
//     int dx = abs(x1 - x0);
//     int dy = -abs(y1 - y0);
//     int sx = (x0 < x1) ? 1 : -1;
//     int sy = (y0 < y1) ? 1 : -1;
//     int err = dx + dy; // الخطأ الابتدائي
//     int e2;

//     while (1)
//     {
//         my_mlx_pixel_put(img, x0, y0, color);
//         if (x0 == x1 && y0 == y1)
//             break;
//         e2 = 2 * err;
//         if (e2 >= dy) // المحور X
//         {
//             err += dy;
//             x0 += sx;
//         }
//         if (e2 <= dx) // المحور Y
//         {
//             err += dx;
//             y0 += sy;
//         }
//     }
// }


// void draw_circle(t_img *img, int center_x, int center_y, int radius, int color) {
//     for (int y = -radius; y <= radius; y++) {
//         for (int x = -radius; x <= radius; x++) {
//             if (x * x + y * y <= radius * radius) {
//                 my_mlx_pixel_put(img, center_x + x, center_y + y, color);
//             }
//         }
//     }
// }

// /* Game Functions */
// int has_wall_at(t_game *game, double x, double y) {
//     if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
//         return 1;

//     int map_x = (int)(x / TILE_SIZE);
//     int map_y = (int)(y / TILE_SIZE);

//     if (map_y < 0 || map_y >= MAP_NUM_ROWS || map_x < 0 || map_x >= MAP_NUM_COLS)
//         return 1;

//     return (game->map[map_y][map_x] == '1');
// }

// void cast_ray(t_game *game, double ray_angle, int ray_id) {
//     ray_angle = normalize_angle(ray_angle);
    
//     t_ray *ray = &game->rays[ray_id];
//     ray->ray_angle = ray_angle;
//     ray->distance = INFINITY;
    
//     // Horizontal intersections
//     double yintercept = floor(game->player.y / TILE_SIZE) * TILE_SIZE;
//     yintercept += (ray_angle > 0 && ray_angle < M_PI) ? TILE_SIZE : 0;
    
//     double xintercept = game->player.x + (yintercept - game->player.y) / tan(ray_angle);
    
//     double ystep = TILE_SIZE;
//     ystep *= (ray_angle > 0 && ray_angle < M_PI) ? 1 : -1;
    
//     double xstep = TILE_SIZE / tan(ray_angle);
//     xstep *= (ray_angle > M_PI_2 && ray_angle < 3 * M_PI_2) ? -1 : 1;
    
//     double next_horz_x = xintercept;
//     double next_horz_y = yintercept;
    
//     while (next_horz_x >= 0 && next_horz_x < WINDOW_WIDTH && 
//            next_horz_y >= 0 && next_horz_y < WINDOW_HEIGHT) {
//         double x_to_check = next_horz_x;
//         double y_to_check = next_horz_y + ((ray_angle > 0 && ray_angle < M_PI) ? 0 : -1);
        
//         if (has_wall_at(game, x_to_check, y_to_check)) {
//             ray->wall_hit_x = next_horz_x;
//             ray->wall_hit_y = next_horz_y;
//             ray->distance = distance_between_points(game->player.x, game->player.y, 
//                                                    ray->wall_hit_x, ray->wall_hit_y);
//             ray->was_hit_vertical = 0;
//             ray->wall_hit_content = game->map[(int)y_to_check/TILE_SIZE][(int)x_to_check/TILE_SIZE];
//             break;
//         }
        
//         next_horz_x += xstep;
//         next_horz_y += ystep;
//     }

//     // Vertical intersections
//     xintercept = floor(game->player.x / TILE_SIZE) * TILE_SIZE;
//     xintercept += (ray_angle < M_PI_2 || ray_angle > 3 * M_PI_2) ? TILE_SIZE : 0;
    
//     yintercept = game->player.y + (xintercept - game->player.x) * tan(ray_angle);
    
//     xstep = TILE_SIZE;
//     xstep *= (ray_angle > M_PI_2 && ray_angle < 3 * M_PI_2) ? -1 : 1;
    
//     ystep = TILE_SIZE * tan(ray_angle);
//     ystep *= (ray_angle > 0 && ray_angle < M_PI) ? 1 : -1;
    
//     double next_vert_x = xintercept;
//     double next_vert_y = yintercept;
    
//     while (next_vert_x >= 0 && next_vert_x < WINDOW_WIDTH && 
//            next_vert_y >= 0 && next_vert_y < WINDOW_HEIGHT) {
//         double x_to_check = next_vert_x + ((ray_angle > M_PI_2 && ray_angle < 3 * M_PI_2) ? -1 : 0);
//         double y_to_check = next_vert_y;
        
//         if (has_wall_at(game, x_to_check, y_to_check)) {
//             double distance = distance_between_points(game->player.x, game->player.y, 
//                                                     next_vert_x, next_vert_y);
//             if (distance < ray->distance) {
//                 ray->wall_hit_x = next_vert_x;
//                 ray->wall_hit_y = next_vert_y;
//                 ray->distance = distance;
//                 ray->was_hit_vertical = 1;
//                 ray->wall_hit_content = game->map[(int)y_to_check/TILE_SIZE][(int)x_to_check/TILE_SIZE];
//             }
//             break;
//         }
        
//         next_vert_x += xstep;
//         next_vert_y += ystep;
//     }
// }

// void cast_all_rays(t_game *game) {
//     double ray_angle = game->player.rotation_angle - (FOV_ANGLE / 2);
    
//     for (int i = 0; i < NUM_RAYS; i++) {
//         cast_ray(game, ray_angle, i);
//         ray_angle += FOV_ANGLE / NUM_RAYS;
//     }
// }

// void render_3d_view(t_game *game) {
//     // Clear screen
//     draw_rectangle(&game->img, 0, 0, WIN_WIDTH, WIN_HEIGHT / 2, 0x333333); // Ceiling
//     draw_rectangle(&game->img, 0, WIN_HEIGHT / 2, WIN_WIDTH, WIN_HEIGHT / 2, 0x555555); // Floor
    
//     // Render walls
//     for (int i = 0; i < NUM_RAYS; i++) {
//         t_ray *ray = &game->rays[i];
        
//         // Correct fish-eye effect
//         double corrected_distance = ray->distance * cos(ray->ray_angle - game->player.rotation_angle);
        
//         // Calculate wall height
//         double wall_height = (TILE_SIZE / corrected_distance) * ((WIN_WIDTH / 2) / tan(FOV_ANGLE / 2));
        
//         // Calculate top and bottom pixels
//         int wall_top = (WIN_HEIGHT / 2) - (wall_height / 2);
//         wall_top = wall_top < 0 ? 0 : wall_top;
        
//         int wall_bottom = (WIN_HEIGHT / 2) + (wall_height / 2);
//         wall_bottom = wall_bottom > WIN_HEIGHT ? WIN_HEIGHT : wall_bottom;
        
//         // Choose wall color
//         int color = ray->was_hit_vertical ? 0xCC3333 : 0x33CC33;
        
//         // Draw wall slice
//         draw_rectangle(&game->img, i * WALL_STRIP_WIDTH, wall_top, 
//                       WALL_STRIP_WIDTH, wall_bottom - wall_top, color);
//     }
// }

// void render_minimap(t_game *game) {
//     if (!game->show_minimap) return;
    
//     int minimap_size = 200;
//     int minimap_x = WIN_WIDTH - minimap_size - 20;
//     int minimap_y = 20;
//     float scale = (float)minimap_size / WINDOW_WIDTH;
    
//     // Draw minimap background
//     draw_rectangle(&game->img, minimap_x, minimap_y, minimap_size, minimap_size, 0x000000);
    
//     // Draw map
//     for (int y = 0; y < MAP_NUM_ROWS; y++) {
//         for (int x = 0; x < MAP_NUM_COLS; x++) {
//             if (game->map[y][x] == '1') {
//                 int tile_x = minimap_x + x * TILE_SIZE * scale;
//                 int tile_y = minimap_y + y * TILE_SIZE * scale;
//                 int tile_size = TILE_SIZE * scale;
//                 draw_rectangle(&game->img, tile_x, tile_y, tile_size, tile_size, 0x22DD22);
//             }
//         }
//     }
    
//     // Draw player
//     int player_x = minimap_x + game->player.x * scale;
//     int player_y = minimap_y + game->player.y * scale;
//     draw_circle(&game->img, player_x, player_y, PLAYER_SIZE * scale, 0xFF0000);
    
//     // Draw player direction
//     int line_end_x = player_x + cos(game->player.rotation_angle) * 20;
//     int line_end_y = player_y + sin(game->player.rotation_angle) * 20;
//     draw_line(&game->img, player_x, player_y, line_end_x, line_end_y, 0x0000FF);
    
//     // Draw rays
//     for (int i = 0; i < NUM_RAYS; i += 4) {
//         t_ray *ray = &game->rays[i];
//         int ray_end_x = player_x + (ray->wall_hit_x - game->player.x) * scale;
//         int ray_end_y = player_y + (ray->wall_hit_y - game->player.y) * scale;
//         draw_line(&game->img, player_x, player_y, ray_end_x, ray_end_y, 0xFFFF00);
//     }
// }

// void render(t_game *game) {
//     // Clear image
//     draw_rectangle(&game->img, 0, 0, WIN_WIDTH, WIN_HEIGHT, 0x000000);
    
//     // Render 3D view
//     render_3d_view(game);
    
//     // Render minimap
//     render_minimap(game);
    
//     // Display image
//     mlx_put_image_to_window(game->mlx, game->win, game->img.img_ptr, 0, 0);
// }

// void move_player(t_game *game) {
//     double move_step = game->player.walk_direction * game->player.move_speed;
//     double side_step = game->player.side_direction * game->player.move_speed;
    
//     double new_x = game->player.x + cos(game->player.rotation_angle) * move_step;
//     new_x += cos(game->player.rotation_angle + M_PI_2) * side_step;
    
//     double new_y = game->player.y + sin(game->player.rotation_angle) * move_step;
//     new_y += sin(game->player.rotation_angle + M_PI_2) * side_step;
    
//     if (!has_wall_at(game, new_x, game->player.y))
//         game->player.x = new_x;
//     if (!has_wall_at(game, game->player.x, new_y))
//         game->player.y = new_y;
    
//     game->player.rotation_angle += game->player.turn_direction * game->player.rotation_speed;
//     game->player.rotation_angle = normalize_angle(game->player.rotation_angle);
// }

// int key_press(int keycode, t_game *game) {
//     if (keycode == 53) { // ESC
//         mlx_destroy_window(game->mlx, game->win);
//         exit(0);
//     }
//     else if (keycode == 13 || keycode == 126) // W or UP
//         game->player.walk_direction = 1;
//     else if (keycode == 1 || keycode == 125) // S or DOWN
//         game->player.walk_direction = -1;
//     else if (keycode == 0) // A
//         game->player.side_direction = -1;
//     else if (keycode == 2) // D
//         game->player.side_direction = 1;
//     else if (keycode == 123) // LEFT
//         game->player.turn_direction = -1;
//     else if (keycode == 124) // RIGHT
//         game->player.turn_direction = 1;
//     else if (keycode == 46) // M
//         game->show_minimap = !game->show_minimap;
//     return 0;
// }

// int key_release(int keycode, t_game *game) {
//     if ((keycode == 13 || keycode == 126) && game->player.walk_direction == 1)
//         game->player.walk_direction = 0;
//     else if ((keycode == 1 || keycode == 125) && game->player.walk_direction == -1)
//         game->player.walk_direction = 0;
//     else if (keycode == 0 && game->player.side_direction == -1)
//         game->player.side_direction = 0;
//     else if (keycode == 2 && game->player.side_direction == 1)
//         game->player.side_direction = 0;
//     else if (keycode == 123 && game->player.turn_direction == -1)
//         game->player.turn_direction = 0;
//     else if (keycode == 124 && game->player.turn_direction == 1)
//         game->player.turn_direction = 0;
//     return 0;
// }

// int game_loop(t_game *game) {
//     move_player(game);
//     cast_all_rays(game);
//     render(game);
//     return 0;
// }

// void init_game(t_game *game) {
//     game->mlx = mlx_init();
//     game->win = mlx_new_window(game->mlx, WIN_WIDTH, WIN_HEIGHT, "2D Raycaster");
//     game->img.img_ptr = mlx_new_image(game->mlx, WIN_WIDTH, WIN_HEIGHT);
//     game->img.addr = mlx_get_data_addr(game->img.img_ptr, &game->img.bits_per_pixel, 
//                                       &game->img.line_length, &game->img.endian);
    
//     // Initialize player
//     game->player.x = WINDOW_WIDTH / 2;
//     game->player.y = WINDOW_HEIGHT / 2;
//     game->player.rotation_angle = M_PI / 2;
//     game->player.move_speed = 5.0;
//     game->player.rotation_speed = 3 * (M_PI / 180);
//     game->player.turn_direction = 0;
//     game->player.walk_direction = 0;
//     game->player.side_direction = 0;
    
//     // Initialize map
//     char *map[MAP_NUM_ROWS] = {
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
//         "11111111111111111111"
//     };
    
//     for (int y = 0; y < MAP_NUM_ROWS; y++) {
//         for (int x = 0; x < MAP_NUM_COLS; x++) {
//             game->map[y][x] = map[y][x];
//         }
//     }
    
//     game->show_minimap = 1;
// }

// int main() {
//     t_game game;
    
//     init_game(&game);
    
//     // Set up event handlers
//     mlx_hook(game.win, 2, 1L << 0, key_press, &game);
//     mlx_hook(game.win, 3, 1L << 1, key_release, &game);
    
//     // Start game loop
//     mlx_loop_hook(game.mlx, game_loop, &game);
//     mlx_loop(game.mlx);
    
//     return 0;
// }
// #include <mlx.h>
// #include <math.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <string.h>

// #define WIN_WIDTH 1200 
// #define WIN_HEIGHT 800 

// #define MAP_NUM_ROWS    12
// #define MAP_NUM_COLS    20
// #define TILE_SIZE       22
// #define WINDOW_WIDTH    (MAP_NUM_COLS * TILE_SIZE)
// #define WINDOW_HEIGHT   (MAP_NUM_ROWS * TILE_SIZE)
// // #define WIN_WIDTH       WINDOW_WIDTH
// // #define WIN_HEIGHT      WINDOW_HEIGHT

// #define SCALE           30
// #define ZOOM_Z          10
// #define OFFSET_X        400
// #define OFFSET_Y        200
// #define BLOCK_SIZE      8

// #define WALL_STRIP_WIDTH 4
// #define NUM_RAYS        (WINDOW_WIDTH / WALL_STRIP_WIDTH)
// #define FOV_ANGLE       (60 * (M_PI / 180.0))

// typedef struct s_vars {
//     void *mlx;
//     void *win;
// } t_vars;

// typedef struct s_dataplyer {
//    double x;  
//    double y;
//    int radius;
//    int turnDirection;
//    int walkDirection;
//    int sideDirection;
//    double rotationtangle;
//    double movespeed;
//    double rotationspeed;
//    void *mlx_ptr;
//    void *win_ptr;
//    char **map;
//    void *img;
//    char *addr;
//    int bits_per_pixel;
//    int line_length;
//    int endian;
// } t_dataplyer;


// // double normalize_angle(double angle)
// // {
// //     double two_pi = 2.0 * M_PI;
// //     while (angle < 0) angle += two_pi;
// //     while (angle >= two_pi) angle -= two_pi;
// //     return angle;
// // }

// // double distance_between_points(double x1, double y1, double x2, double y2)
// // {
// //     double dx = x2 - x1;
// //     double dy = y2 - y1;
// //     return sqrt(dx * dx + dy * dy);
// // // }
// int has_wall_at(char **map, double x, double y)
// {
//     int map_x = (int)(x / TILE_SIZE);
//     int map_y = (int)(y / TILE_SIZE);

//     if (map_y < 0 || map_y >= MAP_NUM_ROWS || map_x < 0 || map_x >= MAP_NUM_COLS)
//         return 1;

//     return (map[map_y][map_x] == '1');
// }

// void my_mlx_pixel_put(t_dataplyer *data, int x, int y, int color)
// {
//     char *dst;

//     if (x >= 0 && x < WIN_WIDTH && y >= 0 && y < WIN_HEIGHT)
//     {
//         dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
//         *(unsigned int*)dst = color;
//     }
// }

// void draw_line(t_dataplyer *data, int x0, int y0, int x1, int y1, int color)
// {
//     int dx = abs(x1 - x0);
//     int dy = abs(y1 - y0);
//     int sx = x0 < x1 ? 1 : -1;
//     int sy = y0 < y1 ? 1 : -1;
//     int err = (dx > dy ? dx : -dy) / 2;
//     int e2;

//     while (1)
//     {
//         my_mlx_pixel_put(data, x0, y0, color);
//         if (x0 == x1 && y0 == y1) break;
//         e2 = err;
//         if (e2 > -dx) { err -= dy; x0 += sx; }
//         if (e2 < dy) { err += dx; y0 += sy; }
//     }
// }

// void draw_circle(t_dataplyer *data, int center_x, int center_y, int radius, int color)
// {
//     for (int y = -radius; y <= radius; y++)
//     {
//         for (int x = -radius; x <= radius; x++)
//         {
//             if (x * x + y * y <= radius * radius)
//             {
//                 int px = center_x + x;
//                 int py = center_y + y;
//                 my_mlx_pixel_put(data, px, py, color);
//             }
//         }
//     }
// }



// void draw_square(t_dataplyer *data, int x, int y, int size, int color)
// {
//     for (int i = 0; i < size; i++)
//     {
//         for (int j = 0; j < size; j++)
//         {
//             int px = x + i;
//             int py = y + j;
//             my_mlx_pixel_put(data, px, py, color);
//         }
//     }
// }
// // void cast_and_draw_ray(t_dataplyer *data, double ray_angle, int color)
// // {
// //     ray_angle = normalize_angle(ray_angle);

// //     double ray_x = data->x;
// //     double ray_y = data->y;
// //     double step_size = 1.0; 
// //     double dx = cos(ray_angle) * step_size;
// //     double dy = sin(ray_angle) * step_size;

// //     int max_depth = WINDOW_WIDTH;
// //     for (int i = 0; i < max_depth; i++)
// //     {
// //         ray_x += dx;
// //         ray_y += dy;

   
// //         if (has_wall_at(data->map, ray_x, ray_y))
// //         {
         
// //             draw_line(data, (int)data->x, (int)data->y, (int)ray_x, (int)ray_y, color);
// //             break;
// //         }
      
// //     }
// // }
// //  void calleaye_wall(t_dataplyer *data)
// //  {
// //     double start_angle = data->rotationtangle - (FOV_ANGLE / 2.0);
// //     double ray_angle;
// //     double angle_step = FOV_ANGLE / (double)NUM_RAYS;

// //     for (int i = 0; i < NUM_RAYS; i++)
// //     {
// //         ray_angle = start_angle + i * angle_step;
// //         cast_and_draw_ray(data, ray_angle, 0xFFFF00);
// //     }

// //  }
// void render(t_dataplyer *data, char **map)
// {

//     for (int y = 0; y < WIN_HEIGHT; y++)
//     {
//         for (int x = 0; x < WIN_WIDTH; x++)
//         {
//             my_mlx_pixel_put(data, x, y, 0x000000); 
//     }


//     for (int i = 0; i < MAP_NUM_ROWS; i++)
//     {
//         for (int j = 0; j < MAP_NUM_COLS; j++)
//         {
//             int tilex = j * TILE_SIZE;
//             int tiley = i * TILE_SIZE;
//             int color = (map[i][j] == '1') ? 0x22dd22 : 0x444444;
//             draw_square(data, tilex, tiley, TILE_SIZE - 2, color);
//         }
//     }
// 	}
// // calleaye_wall(data);
//     // رسم اللاعب
//     draw_circle(data, (int)data->x, (int)data->y, data->radius, 0xFF0000);
    
//     // رسم اتجاه اللاعب
//     int endX = (int)(data->x + cos(data->rotationtangle) * 30);
//     int endY = (int)(data->y + sin(data->rotationtangle) * 30);
//     draw_line(data, (int)data->x, (int)data->y, endX, endY, 0x0000FF);

//     // عرض الصورة
//     mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->img, 0, 0);
// }

// void play_player(t_dataplyer *data, t_vars *vars)
// {
//     data->x = WINDOW_WIDTH / 2.0;
//     data->y = WINDOW_HEIGHT / 2.0;
//     data->radius = 8;
//     data->turnDirection = 0;
//     data->walkDirection = 0;
//     data->sideDirection = 0;
//     data->rotationtangle = M_PI / 2;
//     data->movespeed = 2.5;
//     data->rotationspeed = 3 * (M_PI / 180);
//     data->mlx_ptr = vars->mlx;
//     data->win_ptr = vars->win;

//     data->img = mlx_new_image(data->mlx_ptr, WIN_WIDTH, WIN_HEIGHT);
//     data->addr = mlx_get_data_addr(data->img, &data->bits_per_pixel, 
//                                    &data->line_length, &data->endian);
// }

// int key_press(int keycode, t_dataplyer *data)
// {
//     if (keycode == 13 || keycode == 126) 
//         data->walkDirection = 1;
//     else if (keycode == 1 || keycode == 125) 
//         data->walkDirection = -1;
//     else if (keycode == 0) 
//         data->sideDirection = -1;
//     else if (keycode == 2)
//         data->sideDirection = 1;
//     else if (keycode == 123) 
//         data->turnDirection = -1;
//     else if (keycode == 124) 
//         data->turnDirection = 1;
//     else if (keycode == 53) 
//     {
//         mlx_destroy_window(data->mlx_ptr, data->win_ptr);
//         exit(0);
//     }
//     return (0);
// }


// int key_release(int keycode, t_dataplyer *data)
// {
//     if ((keycode == 13 || keycode == 126) && data->walkDirection == 1)
//         data->walkDirection = 0;
//     else if ((keycode == 1 || keycode == 125) && data->walkDirection == -1)
//         data->walkDirection = 0;
//     else if (keycode == 0 && data->sideDirection == -1)
//         data->sideDirection = 0;
//     else if (keycode == 2 && data->sideDirection == 1)
//         data->sideDirection = 0;
//     else if (keycode == 123 && data->turnDirection == -1)
//         data->turnDirection = 0;
//     else if (keycode == 124 && data->turnDirection == 1)
//         data->turnDirection = 0;
//     return (0);
// }

// void update_player(t_dataplyer *data)
// {

//     data->rotationtangle += data->turnDirection * data->rotationspeed;


//     double moveStep = data->walkDirection * data->movespeed;
//     double sideStep = data->sideDirection * data->movespeed;


//     double newX = data->x + cos(data->rotationtangle) * moveStep;
//     double newY = data->y + sin(data->rotationtangle) * moveStep;


//     newX += cos(data->rotationtangle + M_PI_2) * sideStep;
//     newY += sin(data->rotationtangle + M_PI_2) * sideStep;


//     if (!has_wall_at(data->map, newX, data->y))
//         data->x = newX;
//     if (!has_wall_at(data->map, data->x, newY))
//         data->y = newY;


//     if (data->x < data->radius)
//         data->x = data->radius;
//     if (data->x > WINDOW_WIDTH - data->radius)
//         data->x = WINDOW_WIDTH - data->radius;
//     if (data->y < data->radius)
//         data->y = data->radius;
//     if (data->y > WINDOW_HEIGHT - data->radius)
//         data->y = WINDOW_HEIGHT - data->radius;
// }

// int loop_hook(void *param)
// {
//     t_dataplyer *data = (t_dataplyer *)param;
//     update_player(data);
//     render(data, data->map);
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
//     vars.win = mlx_new_window(vars.mlx, WIN_WIDTH, WIN_HEIGHT, "2D Map - Fixed Movement");

//     play_player(&data, &vars);
//     render(&data, data.map);
//     handle_event(&data);
//     mlx_loop_hook(vars.mlx, loop_hook, &data);
//     mlx_loop(vars.mlx);
    
//     return 0;
// }
// #include <mlx.h>
// #include <math.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <string.h>


// // #define M_PI 3.14159265358979323846
// #define WIN_WIDTH 1200
// #define WIN_HEIGHT 800

// #define SCALE 30
// #define ZOOM_Z 10
// #define OFFSET_X 400
// #define TILE_SIZE 32
// #define MAP_NUM_ROWS 12
// #define MAP_NUM_COLS 20
// #define WINDOW_WIDTH  (MAP_NUM_COLS * TILE_SIZE)
// #define WINDOW_HEIGHT (MAP_NUM_ROWS * TILE_SIZE)

// #define OFFSET_Y 200
// #define BLOCK_SIZE 8 // حجم كل مربع

// typedef struct s_vars {
//     void *mlx;
//     void *win;

// } t_vars;

// typedef struct s_dataplyer {
//    int x;
//    int y;
//    int radius;
//    int turnDirection;
//    int walkDirection;
//    int sideDirection;
//    double rotationtangle;
//    double movespeed;
//    double rotationspeed;
//     void *mlx_ptr;
//     void *win_ptr;
// 	char **map;
// } t_dataplyer;

// void draw_line(t_dataplyer *data, int x0, int y0, float angle, int length)
// {
// 	int i;
// 	int x;
// 	int y;

// 	i = 0;
// 	while (i < length)
// 	{
// 		// Calculate the next point on the line using trigonometry
// 		x = x0 + cos(angle) * i;
// 		y = y0 + sin(angle) * i;
// 		// Draw the pixel if it's within bounds
// 		if (x >= 0 && x < WINDOW_WIDTH  && y >= 0 && y < WINDOW_HEIGHT)
// 			my_mlx_pixel_put(data, x, y,0xFF0000 ); // Player direction line color
// 		i++;
// 	}
// }

// // void draw_line(void *mlx, void *win, int x1, int y1, int x2, int y2, int color)
// // {
// //     int dx = abs(x2 - x1);
// //     int dy = abs(y2 - y1);
// //     int sx = x1 < x2 ? 1 : -1;
// //     int sy = y1 < y2 ? 1 : -1;
// //     int err = (dx > dy ? dx : -dy) / 2;
// //     int e2;

// //     while (1)
// //     {
// //         mlx_pixel_put(mlx, win, x1, y1, color);
// //         if (x1 == x2 && y1 == y2) break;
// //         e2 = err;
// //         if (e2 > -dx) { err -= dy; x1 += sx; }
// //         if (e2 < dy) { err += dx; y1 += sy; }
// //     }
// // }




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
// int has_wall_at(char **map, double x, double y)
// {
//     int map_x = (int)(x / TILE_SIZE);
//     int map_y = (int)(y / TILE_SIZE);

//     if (map_y < 0 || map_y >= MAP_NUM_ROWS || map_x < 0 || map_x >= MAP_NUM_COLS)
//         return 1; // اعتبره جداراً خارج الخريطة

//     return (map[map_y][map_x] == '1');
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
// void render(t_dataplyer *data, char **map)
// {
//      mlx_clear_window(data->mlx_ptr, data->win_ptr);
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

  
//     draw_circle(data->mlx_ptr, data->win_ptr, data->x, data->y, data->radius, 0xFF0000);
// 	    // int lineLength = 20;
// 		//  
//     int endX = data->x + cos(data->rotationtangle) * 30;
//     int endY = data->y + sin(data->rotationtangle) * 30;
//     draw_line(data->mlx_ptr, data->win_ptr, data->x, data->y, endX, endY, 0x0000FF);
// }




// // رسم مربع بلون معين


// void 	play_palyer( t_dataplyer *data, t_vars *vars)
// {
// 	data->x = WINDOW_WIDTH / 2;
// 	data->y = WINDOW_HEIGHT / 2;
// 	data->radius = 8;
// 	data->turnDirection = 0;
// 	data->walkDirection = 0;
// 	data->sideDirection = 0;
// 	data->rotationspeed = 0;
// 	data->rotationtangle = M_PI /2;
// 	data->movespeed =3.0;
// 	data->rotationspeed = 4 * (M_PI /180);
// 	data->mlx_ptr = vars->mlx;
//     data->win_ptr = vars->win;

// }

// int key_press(int keycode, t_dataplyer *data)
// {
//     if ( keycode == 126) // W أو سهم ↑
//         data->walkDirection = +1;
//     else if ( keycode == 125) // S أو سهم ↓
//         data->walkDirection = -1;
//     else if (keycode == 123) // A أو سهم ←
//         data->turnDirection = +1;
//     else if ( keycode == 124) // D أو سهم →
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
//     if ( keycode == 126 && data->walkDirection == 1)
//         data->walkDirection = 0;
//     else if ( keycode == 125 && data->walkDirection == -1)
//         data->walkDirection = 0;
//     else if ( keycode == 123 && data->turnDirection == 1)
//         data->sideDirection = 0;
//     else if ( keycode == 124 && data->turnDirection == -1)
//         data->sideDirection = 0;
//     return (0);
// }

// void update_player(t_dataplyer *data)
// {
// 	data->rotationtangle += data->turnDirection * data->rotationspeed; 
//     double moveStep = data->walkDirection * data->movespeed * 2.0;
//     double sideStep = data->sideDirection * data->movespeed ;

//     double newX = data->x + cos(data->rotationtangle) * moveStep;
//     double newY = data->y + sin(data->rotationtangle) * moveStep;

//     // حركة جانبية (Strafing)
//     newX += cos(data->rotationtangle + M_PI_2) * sideStep;
//     newY += sin(data->rotationtangle + M_PI_2) * sideStep;

// 	  if (!has_wall_at(data->map, newX, data->y))
//     	data->x = (int)newX;
// 	if (!has_wall_at(data->map, data->x, newY))
//     	data->y = (int)newY;
// }
// int loop_hook(void *param)
// {
//     t_dataplyer *data = (t_dataplyer *)param;
//     update_player(data);             // تحديث موقع اللاعب
//     render(data,data->map);              // إعادة الرسم (سنرسل الخريطة قريبًا)
//     return (0);
// }

// void handle_event( t_dataplyer *data)
// {
// 	mlx_hook(data->win_ptr, 2, 1L << 0, key_press, data);
// 	mlx_hook(data->win_ptr, 3, 1L << 1, key_release, data);

// }
// int main()
// {
//     t_vars vars;
//     t_dataplyer data;
//     // الخريطة كسلسلة من النصوص
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
// 		data.map = map;
//     vars.mlx = mlx_init();
//     vars.win = mlx_new_window(vars.mlx, WIN_WIDTH, WIN_HEIGHT, "2D Map");

//     // رسم الخريطة
//     // for (int i = 0; i < MAP_NUM_ROWS; i++)
//     // {
//     //     for (int j = 0; j < MAP_NUM_COLS; j++)
//     //     {
//     //         int tilex = j * TILE_SIZE;
//     //         int tiley = i * TILE_SIZE;
//     //         int color = (map[i][j] == '1') ? 0x22dd22 : 0xFFFFFF; // لون كقيمة int
//     //         draw_square(vars.mlx, vars.win, tilex, tiley, TILE_SIZE -  2, color);
//     //     }
//     // }
// 	// draw_circle(vars.mlx, vars.win, data.x,data.y, data.radius ,  0xFF0000);
// 	play_palyer(&data,&vars);
// 	render(&data,data.map);
// 	handle_event( &data );
// 	mlx_loop_hook(vars.mlx, loop_hook, &data);
//     mlx_loop(vars.mlx);
// 	printf("%f",M_PI);
//     return 0;
// }
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