#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Bug dimensions
const int BUG_WIDTH = 50;
const int BUG_HEIGHT = 50;

// Function to check collision between two rectangles
bool checkCollision(SDL_Rect a, SDL_Rect b) {
    int leftA = a.x;
    int rightA = a.x + a.w;
    int topA = a.y;
    int bottomA = a.y + a.h;

    int leftB = b.x;
    int rightB = b.x + b.w;
    int topB = b.y;
    int bottomB = b.y + b.h;

    if (bottomA <= topB || topA >= bottomB || rightA <= leftB || leftA >= rightB)
        return false;
    return true;
}

int main(int argc, char* args[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }

    // Load images
    SDL_Surface* bugSurface = IMG_Load("bug.png");
    if (!bugSurface) {
        std::cerr << "Failed to load bug image! SDL_image Error: " << IMG_GetError() << std::endl;
        return 1;
    }
    SDL_Texture* bugTexture = SDL_CreateTextureFromSurface(renderer, bugSurface);
    SDL_FreeSurface(bugSurface);

    // Load sound effects
    Mix_Chunk* squishSound = Mix_LoadWAV("squish.wav");
    if (!squishSound) {
        std::cerr << "Failed to load squish sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }

    // Seed random number generator
    std::srand(std::time(nullptr));

    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;

    // Main loop
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            // Handle joystick axis motion
            if (e.type == SDL_JOYAXISMOTION) {
                // Here you can adjust bugX and bugY based on joystick input
                // Example:
                // if (e.jaxis.axis == 0) { // X-axis motion
                //     bugX += e.jaxis.value / 1000;
                // }
                // if (e.jaxis.axis == 1) { // Y-axis motion
                //     bugY += e.jaxis.value / 1000;
                // }
            }
            // Handle mouse click
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                SDL_Rect mouseRect = { mouseX, mouseY, 1, 1 };
                SDL_Rect bugRect = { bugX, bugY, BUG_WIDTH, BUG_HEIGHT };
                if (checkCollision(bugRect, mouseRect)) {
                    // Bug squashed!
                    std::cout << "Bug squashed!" << std::endl;
                    Mix_PlayChannel(-1, squishSound, 0); // Play squish sound
                    bugX = std::rand() % (SCREEN_WIDTH - BUG_WIDTH); // Generate new bug position
                    bugY = std::rand() % (SCREEN_HEIGHT - BUG_HEIGHT);
                }
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);

        // Render bug
        SDL_Rect bugRect = { bugX, bugY, BUG_WIDTH, BUG_HEIGHT };
        SDL_RenderCopy(renderer, bugTexture, NULL, &bugRect);

        // Update screen
        SDL_RenderPresent(renderer);

        // Delay to control frame rate
        SDL_Delay(10);
    }

    // Free resources
    SDL_DestroyTexture(bugTexture);
    Mix_FreeChunk(squishSound);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Quit SDL subsystems
    IMG_Quit();
    Mix_Quit();
    SDL_Quit();

    return 0;
}

