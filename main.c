#include <SDL2/SDL.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#define WIDTH 800
#define HEIGHT 600

struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    uint32_t pixels[WIDTH * HEIGHT];
    bool quit;
} context;

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
        SDL_RenderClear(context.renderer);
        SDL_RenderCopy(context.renderer, context.texture, NULL, NULL);
        SDL_RenderPresent(context.renderer);
    }

    SDL_DestroyWindow(context.window);
    SDL_Quit();

    return 0;
}
