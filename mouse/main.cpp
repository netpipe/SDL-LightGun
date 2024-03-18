#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Bug dimensions
const int BUG_WIDTH = 50;
const int BUG_HEIGHT = 50;

bool isFullScreen = false; // false=fullscreen

#define SINDEN

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



void toggleFullScreen(SDL_Window* window, bool currentState)
{
    isFullScreen = !currentState;

    SDL_SetWindowFullscreen(window, !currentState);
    SDL_ShowCursor(currentState);
}

int main(int argc, char* args[]) {

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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



    // Create window
    SDL_Window* window = SDL_CreateWindow("Bug Squash", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    #ifdef SINDEN
        SDL_Surface *background = IMG_Load("SindenBorderWhiteLarge_Wide.png");
        if(background == NULL)
        {
            SDL_ShowSimpleMessageBox(0, "Background init error",         SDL_GetError(), window);
        }
        SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, background);
        SDL_FreeSurface(background);
    #endif


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

toggleFullScreen(window,isFullScreen);

    // Bug position
    int bugX = SCREEN_WIDTH / 2 - BUG_WIDTH / 2;
    int bugY = SCREEN_HEIGHT / 2 - BUG_HEIGHT / 2;

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
        SDL_SetRenderDrawColor(renderer, 25, 25, 80, 25);
        SDL_RenderClear(renderer);
        #ifdef SINDEN
            SDL_RenderCopy(renderer, texture, NULL, NULL);
        #endif
        // Render bug
        SDL_Rect bugRect = { bugX, bugY, BUG_WIDTH, BUG_HEIGHT };
        SDL_RenderCopy(renderer, bugTexture, NULL, &bugRect);

        // Update screen
        SDL_RenderPresent(renderer);

        // Delay to control frame rate
        SDL_Delay(10);
    }

    // Free resources
    #ifdef SINDEN
    SDL_DestroyTexture(texture);
    #endif
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
