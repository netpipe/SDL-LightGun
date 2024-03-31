#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

// Screen dimensions
const int windowWidth = 800;
const int windowHeight = 600;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
// Bug dimensions
const int BUG_WIDTH = 50;
const int BUG_HEIGHT = 50;
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
   // Bug position
    int bugX = SCREEN_WIDTH / 2 - BUG_WIDTH / 2;
    int bugY = SCREEN_HEIGHT / 2 - BUG_HEIGHT / 2;
    // Seed random number generator
    std::srand(std::time(nullptr));

    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;
SDL_Joystick *joystick;
    const char *name = NULL;


printf("There are %d joysticks attached\n", SDL_NumJoysticks());
for (int i = 0; i < SDL_NumJoysticks(); ++i) {
    name = SDL_JoystickNameForIndex(i);
    printf("Joystick %d: %s\n", i, name ? name : "Unknown Joystick");
    joystick = SDL_JoystickOpen(i);
    if (joystick == NULL) {
        fprintf(stderr, "SDL_JoystickOpen(%d) failed: %s\n", i,
                SDL_GetError());
    }
}
name = SDL_JoystickName(joystick);
printf("Watching joystick %d: (%s)\n", SDL_JoystickInstanceID(joystick),
       name ? name : "Unknown Joystick");
printf("Joystick has %d axes, %d hats, %d balls, and %d buttons\n",
       SDL_JoystickNumAxes(joystick), SDL_JoystickNumHats(joystick),
       SDL_JoystickNumBalls(joystick), SDL_JoystickNumButtons(joystick));




    // Main loop
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            switch(e.type){
            case SDL_JOYAXISMOTION:
                if (e.jaxis.axis == 0) {
                          // X-axis motion
                          int xAxisValue = e.jaxis.value;
                          // Normalize the axis value to the range [-1, 1]
                          float normalizedX = static_cast<float>(xAxisValue) / 32767.0f;
                          // Process the normalized value, e.g., adjust aim horizontally
                          int lightgunX = static_cast<int>((normalizedX + 1.0f) * 0.5f * windowWidth);
printf("Joystick %i\n",lightgunX);
                }
                      else if (e.jaxis.axis == 1) {
                          // Y-axis motion
                          int yAxisValue = e.jaxis.value;
                          // Normalize the axis value to the range [-1, 1]
                          float normalizedY = static_cast<float>(yAxisValue) / 32767.0f;
                          // Process the normalized value, e.g., adjust aim vertically
                          // Invert the axis as described
                          float invertedY = -normalizedY;
                          int lightgunY = static_cast<int>((invertedY + 1.0f) * 0.5f * windowHeight);
                           printf("Joystick %i\n",lightgunY);
                      }
                printf("Joystick %d axis %d value: %d\n",
                       e.jaxis.which,
                       e.jaxis.axis, e.jaxis.value);
                break;
            case SDL_JOYHATMOTION:
                printf("Joystick %d hat %d value:",
                       e.jhat.which, e.jhat.hat);
                if (e.jhat.value == SDL_HAT_CENTERED)
                    printf(" centered");
                if (e.jhat.value & SDL_HAT_UP)
                    printf(" up");
                if (e.jhat.value & SDL_HAT_RIGHT)
                    printf(" right");
                if (e.jhat.value & SDL_HAT_DOWN)
                    printf(" down");
                if (e.jhat.value & SDL_HAT_LEFT)
                    printf(" left");
                printf("\n");
                break;
            case SDL_JOYBALLMOTION:
                printf("Joystick %d ball %d delta: (%d,%d)\n",
                       e.jball.which,
                       e.jball.ball, e.jball.xrel, e.jball.yrel);
                break;
            case SDL_JOYBUTTONDOWN:
                printf("Joystick %d button %d down\n",
                       e.jbutton.which, e.jbutton.button);
                break;
            case SDL_JOYBUTTONUP:
                printf("Joystick %d button %d up\n",
                       e.jbutton.which, e.jbutton.button);
                break;
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

