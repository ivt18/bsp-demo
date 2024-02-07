#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 216
#define ASPECT_RATIO 0.66

#define MAP_WIDTH 8
#define MAP_HEIGHT 8

#define COLOR_FLOOR 0x202020FF
#define COLOR_CEILING 0x505050FF
#define COLOR_DARKER_FACTOR 0xA0

#define min(a, b) ({ __typeof__(a) _a = (a), _b = (b); _a < _b ? _a : _b; })
#define max(a, b) ({ __typeof__(a) _a = (a), _b = (b); _a > _b ? _a : _b; })
#define sign(a) ({                                       \
        __typeof__(a) _a = (a);                          \
        (__typeof__(a))(_a < 0 ? -1 : (_a > 0 ? 1 : 0)); \
    })


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
} context;

struct {
    int32_t val, side;
    struct vector2f_t pos;
} hit;

const static uint8_t MAP[MAP_WIDTH * MAP_HEIGHT] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
};

static void v_line(int32_t x, int32_t y0, int32_t y1, uint32_t color)
{
    for (int y = y0; y < y1; ++y) {
        context.pixels[y * SCREEN_WIDTH + x] = color;
    }
}

void render()
{
    for (uint32_t x = 0; x < SCREEN_WIDTH; ++x) {
        // ray position
        const struct vector2f_t ray_pos = {
            context.pos.x, context.pos.y
        };

        // ray direction
        float correction = -1 + 2 * (x / (float)SCREEN_WIDTH);
        const struct vector2f_t ray_dir = {
            context.dir.x + context.plane.x * correction,
            context.dir.y + context.plane.y * correction
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
            case 1: color = 0xFF0000FF; break;
        }

        if (hit.side == 1) {
            const uint32_t
                r = ((color & 0xFF000000) * COLOR_DARKER_FACTOR),
                g = ((color & 0x00FF0000) * COLOR_DARKER_FACTOR),
                b = ((color & 0x0000FF00) * COLOR_DARKER_FACTOR);

            color = 0x000000FF | (r & 0xFF000000) | (g & 0x00FF0000) | (b & 0x0000FF00);
        }

        hit.pos = (struct vector2f_t) { ray_pos.x + side_dist.x, ray_pos.y + side_dist.y };

        // distance to the wall we hit
        const float dist_perp = 
            hit.side == 0 ?
            (side_dist.x - delta_dist.x) :
            (side_dist.y - delta_dist.y);

        const int32_t
            h = (int32_t) (SCREEN_HEIGHT / dist_perp),
            y0 = max((SCREEN_HEIGHT / 2) - (h / 2), 0), // upper line end
            y1 = min((SCREEN_HEIGHT / 2) + (h / 2), SCREEN_HEIGHT - 1); // lower line end

        v_line(x, 0, y0, COLOR_CEILING);
        v_line(x, y0, y1, color);
        v_line(x, y1, SCREEN_HEIGHT - 1, COLOR_FLOOR);
    }
}

static void rotate(const float deg)
{
    const struct vector2f_t p = context.plane, d = context.dir;
    context.dir.x = d.x * cos(deg) - d.y * sin(deg);
    context.dir.y = d.x * sin(deg) - d.y * cos(deg);
    context.plane.x = p.x * cos(deg) - p.y * sin(deg);
    context.plane.y = p.x * sin(deg) + p.y * cos(deg);
}

int main()
{
    assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);

    context.window = SDL_CreateWindow("BSP Demo",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH, WINDOW_HEIGHT,
            SDL_WINDOW_SHOWN);

    assert(context.window);

    uint32_t render_flags = SDL_RENDERER_PRESENTVSYNC;
    context.renderer = SDL_CreateRenderer(context.window, -1, render_flags);
    SDL_SetRenderDrawBlendMode(context.renderer, SDL_BLENDMODE_BLEND);

    context.texture = SDL_CreateTexture(context.renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            SCREEN_WIDTH, SCREEN_HEIGHT);
    
    context.pos = (struct vector2f_t) { 4.0f, 4.0f };
    context.dir = norm((struct vector2f_t) { 1.0f, 1.0f });
    context.plane = (struct vector2f_t) { 0.0f, ASPECT_RATIO };

    const float move_speed = 3.0f * 0.016f;
    const float rot_speed = 3.0f * 0.016f;

    while (!context.quit) {
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
    }

    SDL_DestroyTexture(context.texture);
    SDL_DestroyRenderer(context.renderer);
    SDL_DestroyWindow(context.window);
    SDL_Quit();

    return 0;
}
