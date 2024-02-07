#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define ASPECT_RATIO 0.66

#define MAP_WIDTH 8
#define MAP_HEIGHT 8

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

    struct vector2f_t pos, dir;
} context;

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

void render()
{
    for (uint32_t x = 0; x < SCREEN_WIDTH; ++x) {
        // ray position
        const struct vector2f_t ray_pos = {
            context.pos.x, context.pos.y
        };

        // ray direction
        float correction = ASPECT_RATIO * (-1 + 2 * x / (float)SCREEN_WIDTH);
        const struct vector2f_t ray_dir = {
            context.dir.x,
            context.dir.y + correction
        };

        // integer position in map
        const struct vector2i_t ray_posi = { (int32_t) ray_pos.x, (int32_t) ray_pos.y };

        // length of ray cast
        const struct vector2f_t delta_dist = {
            fabsf(1.0f / ray_dir.x),
            fabsf(1.0f / ray_dir.y)
        };

        // length of ray from current pos to first x/y side
        struct vector2f_t side_dist = {
            delta_dist.x * (ray_dir.x < 0 ? (ray_pos.x - ray_posi.x) : (ray_posi.x + 1.0f - ray_pos.x)),
            delta_dist.y * (ray_dir.y < 0 ? (ray_pos.y - ray_posi.y) : (ray_posi.y + 1.0f - ray_pos.y))
        };

        // TODO: step direction, DDA, etc.
    }
}

int main()
{
    assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);

    context.window = SDL_CreateWindow("BSP Demo",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN);

    assert(context.window);

    uint32_t render_flags = SDL_RENDERER_ACCELERATED;
    context.renderer = SDL_CreateRenderer(context.window, -1, render_flags);
    SDL_SetRenderDrawBlendMode(context.renderer, SDL_BLENDMODE_BLEND);

    context.texture = SDL_CreateTexture(context.renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            SCREEN_WIDTH, SCREEN_HEIGHT);
    
    context.pos = (struct vector2f_t) { 4.0f, 4.0f };
    context.dir = norm((struct vector2f_t) { 1.0f, 1.0f });

    while (!context.quit) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    context.quit = true;
                    break;
            }
        }

        render();

        SDL_UpdateTexture(context.texture, NULL, context.pixels, SCREEN_WIDTH * 4); // 4 == sizeof(uint32_t)
        SDL_RenderCopy(context.renderer, context.texture, NULL, NULL);
        SDL_RenderPresent(context.renderer);
    }

    SDL_DestroyWindow(context.window);
    SDL_Quit();

    return 0;
}
