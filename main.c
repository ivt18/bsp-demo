#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL.h>

#define FPS_INTERVAL 1.0f // seconds

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 216

#define MAP_WIDTH 8
#define MAP_HEIGHT 8

#define COLOR_FLOOR 0x202020FF
#define COLOR_CEILING 0x505050FF
#define COLOR_DARKER_FACTOR 0xA0

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

struct vector2f_t { float x, y; };
struct vector2i_t { int32_t x, y; };

static inline float dot(const struct vector2f_t v, const struct vector2f_t u)
{
    return (v.x * u.x) + (v.y * u.y);
}

static inline float len(const struct vector2f_t v)
{
    return sqrtf(dot(v, v));
}

static inline struct vector2f_t norm(const struct vector2f_t v)
{
    struct vector2f_t u;
    float l = len(v);
    u.x = v.x / l;
    u.y = v.y / l;
    return u;
}

struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    bool quit;

    struct vector2f_t pos, dir, plane;

    uint32_t frame_start, frame_end, frame_time, frame_count;
    float delta_time, fps;
} context;

struct {
    int32_t val, side;
    struct vector2f_t pos;
} hit;

static const uint8_t MAP[MAP_WIDTH * MAP_HEIGHT] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 2, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 3, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 4, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
};

static void v_line(int32_t x, int32_t y0, int32_t y1, uint32_t color)
{
    for (int y = y0; y < y1; ++y) {
        context.pixels[y * SCREEN_WIDTH + x] = color;
    }
}

static uint32_t rgba_shading(const uint32_t color, const uint8_t shading)
{
    uint8_t r = (color >> 24) & 0xFF;
    r -= min(shading, r);
    uint8_t g = (color >> 16) & 0xFF;
    g -= min(shading, g);
    uint8_t b = (color >> 8)  & 0xFF;
    b -= min(shading, b);

    uint32_t result = 0x000000FF | ((r << 24) & 0xFF000000) | ((g << 16) & 0x00FF0000) | ((b << 8) & 0x0000FF00);
    return result;
}

void render()
{
    for (uint32_t x = 0; x < SCREEN_WIDTH; ++x) {
        // ray position
        const struct vector2f_t ray_pos = context.pos;

        // ray direction
        float x_cam = (2 * (x / (float) (SCREEN_WIDTH))) - 1;
        const struct vector2f_t ray_dir = {
            context.dir.x + context.plane.x * x_cam,
            context.dir.y + context.plane.y * x_cam
        };

        // integer position in map
        struct vector2i_t ray_ipos = { (int32_t) ray_pos.x, (int32_t) ray_pos.y };

        // length of ray cast
        const struct vector2f_t delta_dist = {
            fabsf(1.0f / ray_dir.x),
            fabsf(1.0f / ray_dir.y)
        };

        // length of ray from current pos to first x/y side
        struct vector2f_t side_dist = {
            delta_dist.x * (ray_dir.x < 0 ? (ray_pos.x - ray_ipos.x) : (ray_ipos.x + 1.0f - ray_pos.x)),
            delta_dist.y * (ray_dir.y < 0 ? (ray_pos.y - ray_ipos.y) : (ray_ipos.y + 1.0f - ray_pos.y))
        };

        // direction to step in each axis
        struct vector2i_t step = {
            (int32_t) sign(ray_dir.x), (int32_t) sign(ray_dir.y)
        };

        hit.val = 0;
        while (!hit.val) {
            if (side_dist.x < side_dist.y) {
                side_dist.x += delta_dist.x;
                ray_ipos.x += step.x;
                hit.side = 0;
            } else {
                side_dist.y += delta_dist.y;
                ray_ipos.y += step.y;
                hit.side = 1;
            }

            assert(ray_ipos.x >= 0 &&
                    ray_ipos.x < MAP_WIDTH &&
                    ray_ipos.y >= 0 &&
                    ray_ipos.y < MAP_HEIGHT);

            hit.val = MAP[ray_ipos.y * MAP_WIDTH + ray_ipos.x];
        }

        uint32_t color;
        switch (hit.val) {
            case 1: color = 0xF0F0F0FF; break;
            case 2: color = 0xFF0000FF; break;
            case 3: color = 0x00FF00FF; break;
            case 4: color = 0x0000FFFF; break;
            default: color = 0; break;
        }

        // darken walls depending on perspective
        if (hit.side == 1) {
            color = rgba_shading(color, 40);
        }

        hit.pos = (struct vector2f_t) { ray_pos.x + side_dist.x, ray_pos.y + side_dist.y };

        // distance to the wall we hit
        const float dist_perp = 
            hit.side == 0 ?
            (side_dist.x - delta_dist.x) :
            (side_dist.y - delta_dist.y);

        const int32_t
            h = (int32_t) (SCREEN_HEIGHT / dist_perp),
            y0 = max((SCREEN_HEIGHT / 2.0f) - (h / 2.0f), 0.0f), // upper line end
            y1 = min((SCREEN_HEIGHT / 2.0f) + (h / 2.0f), SCREEN_HEIGHT - 1.0f); // lower line end

        v_line(x, 0.0f, y0, COLOR_CEILING);
        v_line(x, y0, y1, color);
        v_line(x, y1, SCREEN_HEIGHT - 1, COLOR_FLOOR);
    }
}

static void rotate(const float deg)
{
    const struct vector2f_t p = context.plane, d = context.dir;
    context.dir.x = d.x * cos(deg) - d.y * sin(deg);
    context.dir.y = d.x * sin(deg) + d.y * cos(deg);
    context.plane.x = p.x * cos(deg) - p.y * sin(deg);
    context.plane.y = p.x * sin(deg) + p.y * cos(deg);
}

int main()
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
    
    context.pos = (struct vector2f_t) { 2.0f, 2.0f };
    context.dir = norm((struct vector2f_t) { 1.0f, -0.1f });
    context.plane = (struct vector2f_t) { 0.0f, 0.66f };
    context.delta_time = 0.0f;

    const float move_speed = 3.0f * 0.016f;
    const float rot_speed = 3.0f * 0.016f;

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
        if (keystate[SDL_SCANCODE_W]) {
            context.pos.x += context.dir.x * move_speed;
            context.pos.y += context.dir.y * move_speed;
        }

        if (keystate[SDL_SCANCODE_S]) {
            context.pos.x -= context.dir.x * move_speed;
            context.pos.y -= context.dir.y * move_speed;
        }

        if (keystate[SDL_SCANCODE_A]) {
            rotate(-rot_speed);
        }
        
        if (keystate[SDL_SCANCODE_D]) {
            rotate(+rot_speed);
        }

        render();

        SDL_UpdateTexture(context.texture, NULL, context.pixels, SCREEN_WIDTH * 4); // 4 == sizeof(uint32_t)
        SDL_RenderCopy(context.renderer, context.texture, NULL, NULL);
        SDL_RenderPresent(context.renderer);

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
