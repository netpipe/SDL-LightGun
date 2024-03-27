#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

//based on https://stackoverflow.com/questions/22886500/how-to-render-text-in-sdl2
#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT (WINDOW_WIDTH)

int main(int argc, char **argv) {
    SDL_Event event;
    SDL_Rect rect1, rect2;
    SDL_Renderer *renderer;
    SDL_Texture *texture1, *texture2;
    SDL_Window *window;
    TTF_Font *font;
    const char *font_path = argc > 1 ? argv[1] : "arial.ttf";
    int quit = 0;

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    /* Create window and renderer */
    if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) < 0) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    /* Initialize SDL_ttf */
    if (TTF_Init() < 0) {
        fprintf(stderr, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    /* Open the font */
    font = TTF_OpenFont(font_path, 24);
    if (!font) {
        fprintf(stderr, "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    /* Render text onto textures */
    SDL_Color textColor = {255, 255, 255, 0};
    SDL_Surface *surface1 = TTF_RenderText_Solid(font, "hello", textColor);
    SDL_Surface *surface2 = TTF_RenderText_Solid(font, "world", textColor);
    texture1 = SDL_CreateTextureFromSurface(renderer, surface1);
    texture2 = SDL_CreateTextureFromSurface(renderer, surface2);
    SDL_FreeSurface(surface1);
    SDL_FreeSurface(surface2);
    SDL_QueryTexture(texture1, NULL, NULL, &rect1.w, &rect1.h);
    SDL_QueryTexture(texture2, NULL, NULL, &rect2.w, &rect2.h);

    /* Main loop */
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }

        /* Clear the screen */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        /* Render textures */
        //SDL_QueryTexture(texture1, NULL, NULL, &rect1.w, &rect1.h);
        //SDL_QueryTexture(texture2, NULL, NULL, &rect2.w, &rect2.h);
        rect1.x = 0;
        rect1.y = 0;
        rect2.x = 0;
        rect2.y = rect1.y + rect1.h;
        SDL_RenderCopy(renderer, texture1, NULL, &rect1);
        SDL_RenderCopy(renderer, texture2, NULL, &rect2);

        /* Present the renderer */
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    /* Cleanup */
    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
