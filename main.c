#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL.h>

#define WIDTH 800
#define HEIGHT 600

struct vector2_f { float x, y; };

struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    uint32_t pixels[WIDTH * HEIGHT];
    bool quit;

    struct vector2_f pos, dir;
} context;

static uint8_t map_data[8 * 8] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
};

static inline float dot(const struct vector2_f v, const struct vector2_f u)
{
    return (v.x * u.x) + (v.y * u.y);
}

static inline float len(const struct vector2_f v)
{
    return sqrtf(dot(v, v));
}

static inline void norm(struct vector2_f *v)
{
    float l = len(*v);
    v->x /= l;
    v->y /= l;
}

int main()
{
    assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);

    context.window = SDL_CreateWindow("BSP Demo",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WIDTH, HEIGHT,
            SDL_WINDOW_SHOWN);

    assert(context.window);

    uint32_t render_flags = SDL_RENDERER_ACCELERATED;
    context.renderer = SDL_CreateRenderer(context.window, -1, render_flags);
    SDL_SetRenderDrawBlendMode(context.renderer, SDL_BLENDMODE_BLEND);

    context.texture = SDL_CreateTexture(context.renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            WIDTH, HEIGHT);
    
    context.pos = (struct vector2_f) {4, 4};
    context.dir = (struct vector2_f) {0, 0};

    while (!context.quit) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    context.quit = true;
                    break;
            }
        }

        SDL_UpdateTexture(context.texture, NULL, context.pixels, WIDTH * 4); // 4 == sizeof(uint32_t)
        SDL_RenderCopy(context.renderer, context.texture, NULL, NULL);
        SDL_RenderPresent(context.renderer);
    }

    SDL_DestroyWindow(context.window);
    SDL_Quit();

    return 0;
}
