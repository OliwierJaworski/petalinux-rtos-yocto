#include "stdio.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

#define WINDOW_W 320
#define WINDOW_H 240
#define SQUARE_SIZE 20
#define SPEED 3

int main(int argc, char* argv[]) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }

    if (SDL_CreateWindowAndRenderer(WINDOW_W, WINDOW_H, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        return 3;
    }

    SDL_Rect square = { WINDOW_W / 2, WINDOW_H / 2, SQUARE_SIZE, SQUARE_SIZE };
    int dx = SPEED;
    int dy = SPEED;

    while (1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                goto quit;
            }
        }

        // Move
        square.x += dx;
        square.y += dy;

        // Bounce off walls
        if (square.x <= 0 || square.x + SQUARE_SIZE >= WINDOW_W) dx = -dx;
        if (square.y <= 0 || square.y + SQUARE_SIZE >= WINDOW_H) dy = -dy;

        // Draw
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // black bg
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF); // red square
        SDL_RenderFillRect(renderer, &square);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60fps
    }

quit:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
