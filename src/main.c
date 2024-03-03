#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "vector.h"
#include "wad_loader.h"
// #include "bsp-tree.h"

#define FPS_INTERVAL 1.0f // seconds

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 216

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_GREY  0xA0A0A0FF
#define COLOR_RED   0xFF0000FF
#define COLOR_GREEN 0x00FF00FF
#define COLOR_BLUE  0x0000FFFF
#define COLOR_AQUA  0x00FFFFFF

#define RADIUS_PlAYER 1.5f
#define COLOR_PLAYER COLOR_RED

#define COLOR_VERTEX COLOR_AQUA
#define COLOR_MAP_LINES COLOR_GREY

const float MOVE_SPEED = 5.0f * 0.016f;
const float ROT_SPEED = 3.0f * 0.0026f;

static inline float min(const float a, const float b)
{
    return (a < b) ? a : b;
}

static inline float max(const float a, const float b)
{
    return (a > b) ? a : b;
}

static inline int32_t sign(const float a)
{
    return a < 0 ? -1 : (a > 0 ? 1 : 0);
}

struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    bool quit;

    struct vector2f_t pos, dir;

    uint32_t frame_start, frame_end, frame_time, frame_count;
    uint64_t time_now, time_last;
    float delta_time, fps;
} context;

struct linedef {
    uint8_t start, end;     // start and end vertices
};

struct {
    uint8_t n_vertices, n_linedefs;
    struct vector2i_t vertices[255];
    struct linedef linedefs[255];
} map;

static void rotate(const float deg)
{
    const struct vector2f_t d = context.dir;
    context.dir.x = d.x * cos(deg * context.frame_time) - d.y * sin(deg * context.frame_time);
    context.dir.y = d.x * sin(deg * context.frame_time) + d.y * cos(deg * context.frame_time);
}

static inline void handle_movement(const uint8_t *keystate)
{
    if (keystate[SDL_SCANCODE_W]) {
        context.pos.x += context.dir.x * MOVE_SPEED * context.frame_time;
        context.pos.y += context.dir.y * MOVE_SPEED * context.frame_time;
    }

    if (keystate[SDL_SCANCODE_S]) {
        context.pos.x -= context.dir.x * MOVE_SPEED * context.frame_time;
        context.pos.y -= context.dir.y * MOVE_SPEED * context.frame_time;
    }

    if (keystate[SDL_SCANCODE_A]) {
        rotate(+ROT_SPEED);
    }
    
    if (keystate[SDL_SCANCODE_D]) {
        rotate(-ROT_SPEED);
    }
}

static void v_line(uint32_t x, uint32_t y0, uint32_t y1, uint32_t color)
{
    for (uint32_t y = y0; y < y1; ++y) {
        context.pixels[y * SCREEN_WIDTH + x] = color;
    }
}

static void draw_square_2d(const struct vector2f_t pos, const float radius, const uint32_t color)
{
    const struct vector2i_t blc = {
        (uint32_t) (pos.x - min(pos.x, radius)),
        (uint32_t) (pos.y - min(pos.y, radius))
    }; // bottom left corner
    const struct vector2f_t trc = {
        (uint32_t) (pos.x + min(SCREEN_WIDTH - pos.x, radius)),
        (uint32_t) (pos.y + min(SCREEN_HEIGHT - pos.y, radius))
    }; // top right corner

    uint32_t x, y;
    for (x = blc.x; x < trc.x; ++x) {
        for (y = blc.y; y < trc.y; ++y) {
            context.pixels[y * SCREEN_WIDTH + x] = color;
        }
    }
}

/**
 * Draw a 2D line segment from point u to point v.
 */
static void draw_line_2d(const struct vector2i_t u, const struct vector2i_t v, const uint32_t color)
{
    const int32_t dx = abs(v.x - u.x);
    const int32_t dy = abs(v.y - u.y);
    const int32_t sx = (u.x < v.x) ? 1 : -1;
    const int32_t sy = (u.y < v.y) ? 1 : -1;
    int32_t err = dx - dy;

    int32_t x = u.x, y = u.y;
    int32_t err2;

    while ((x != v.x || y != v.y) && x >= 0 && y >= 0 && x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
        context.pixels[SCREEN_WIDTH * y + x] = color;
        
        err2 = 2 * err;
        if (err2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (err2 < dx) {
            err += dx;
            y += sy;
        }
    }
    context.pixels[SCREEN_WIDTH * v.y + v.x] = color;
}

static void draw_player_2d()
{
    draw_square_2d(context.pos, RADIUS_PlAYER, COLOR_PLAYER);
    struct vector2i_t t = { (int32_t) (context.pos.x + 3 * context.dir.x), (int32_t) (context.pos.y + 3 * context.dir.y) };
    context.pixels[SCREEN_WIDTH * t.y + t.x] = COLOR_WHITE;
}

static void draw_map_lines_2d()
{
    for (uint8_t i = 0; i < map.n_linedefs; ++i) {
        draw_line_2d(map.vertices[map.linedefs[i].start], map.vertices[map.linedefs[i].end], COLOR_MAP_LINES);
    }
}

static void draw_map_vertices_2d()
{
    for (uint8_t i = 0; i < map.n_vertices; ++i) {
        context.pixels[SCREEN_WIDTH * map.vertices[i].y + map.vertices[i].x] = COLOR_VERTEX;
    }
}

static void render_2d()
{
    draw_map_lines_2d();
    draw_map_vertices_2d();
    draw_player_2d();
}

int main(int argc, char *argv[])
{
    assert(SDL_Init(SDL_INIT_VIDEO) == 0);

    context.window = SDL_CreateWindow("BSP Demo",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH, WINDOW_HEIGHT,
            SDL_WINDOW_ALLOW_HIGHDPI);

    assert(context.window);

    uint32_t render_flags = SDL_RENDERER_PRESENTVSYNC;
    context.renderer = SDL_CreateRenderer(context.window, -1, render_flags);

    context.texture = SDL_CreateTexture(context.renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            SCREEN_WIDTH, SCREEN_HEIGHT);

    context.pos = (struct vector2f_t) { 100.0f, 100.0f };
    context.dir = norm((struct vector2f_t) { 1.0f, -0.1f });
    context.delta_time = 0.0f;

    map.n_vertices = 4;
    map.n_linedefs = 4;
    map.vertices[0] = (struct vector2i_t) { 100, 100 };
    map.vertices[1] = (struct vector2i_t) { 100, 200 };
    map.vertices[2] = (struct vector2i_t) { 200, 200 };
    map.vertices[3] = (struct vector2i_t) { 200, 100 };
    map.linedefs[0] = (struct linedef) { 0, 1 };
    map.linedefs[1] = (struct linedef) { 1, 2 };
    map.linedefs[2] = (struct linedef) { 2, 3 };
    map.linedefs[3] = (struct linedef) { 3, 0 };

    struct wad_header header;
    if (!load_header("C:\\Users\\iliya\\Desktop\\bsp-demo\\e1m1.wad", &header)) {
        printf("%s\n%d\n%d\n",
            header.wad_type, header.num_directories, header.listing_offset);
    }

    while (!context.quit) {
        context.frame_start = SDL_GetTicks();

        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    context.quit = true;
                    break;
            }
        }

        const uint8_t *keystate = SDL_GetKeyboardState(NULL);
        handle_movement(keystate);

        memset(context.pixels, 0, sizeof(context.pixels)); // clear what was previously drawn
        render_2d();

        SDL_UpdateTexture(context.texture, NULL, context.pixels, SCREEN_WIDTH * 4); // 4 == sizeof(uint32_t)
        SDL_RenderCopyEx(context.renderer, context.texture, NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL); // flip vertically
        SDL_RenderPresent(context.renderer);

        context.time_last = context.time_now;
        context.time_now = SDL_GetPerformanceCounter();
        context.frame_end = SDL_GetTicks();
        context.frame_time = context.frame_end - context.frame_start;
        context.delta_time += context.frame_time / 1000.0f;
        context.frame_count++;

        if (context.delta_time > FPS_INTERVAL) {
            context.fps = context.frame_count / context.delta_time;
            printf("FPS: %0.2f\n", context.fps);
            context.frame_count = 0;
            context.delta_time = 0.0f;
        }
    }

    SDL_DestroyTexture(context.texture);
    SDL_DestroyRenderer(context.renderer);
    SDL_DestroyWindow(context.window);
    SDL_Quit();

    return 0;
}
