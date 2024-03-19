#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Bug dimensions
const int BUG_WIDTH = 50;
const int BUG_HEIGHT = 50;

// Function to generate a random number within a range
int random(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Function to load a texture from file using SDL_image
GLuint loadTexture(const char* filename) {
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        std::cerr << "Failed to load texture! SDL_Error: " << IMG_GetError() << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLenum textureFormat = surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, surface->w, surface->h, 0, textureFormat, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_FreeSurface(surface);

    return textureID;
}

// Function to draw a textured rectangle
void drawTexturedRect(float x, float y, float width, float height, GLuint textureID) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width, y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width, y + height);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + height);
    glEnd();
}

int main(int argc, char* args[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }

    // Load background music
    Mix_Music* backgroundMusic = Mix_LoadMUS("music.mp3");
    if (!backgroundMusic) {
        std::cerr << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }
  //  Mix_PlayMusic(backgroundMusic, -1);

    // Load squish sound
    Mix_Chunk* squishSound = Mix_LoadWAV("squish.wav");
    if (!squishSound) {
        std::cerr << "Failed to load squish sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("Bug Squash", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize OpenGL
    glClearColor(1.0f, 0.0f, 0.50f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Load bug texture
    GLuint bugTexture = loadTexture("bug.png");
    if (!bugTexture) {
        return 1;
    }

    // Seed random number generator
    std::srand(std::time(nullptr));

    // Main loop flag
    bool quit = false;

    // Game variables
    int bugX = random(0, SCREEN_WIDTH - BUG_WIDTH);
    int bugY = random(0, SCREEN_HEIGHT - BUG_HEIGHT);
    GLuint backgroundTexture = loadTexture("SindenBorderWhiteLarge_Wide.png");

    // Main loop
    while (!quit) {
        // Handle events on queue
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            // Handle mouse click events
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                // Check if the mouse click is within the bug's area
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                if (mouseX >= bugX && mouseX <= bugX + BUG_WIDTH && mouseY >= bugY && mouseY <= bugY + BUG_HEIGHT) {
                    // Play squish sound
                    Mix_PlayChannel(-1, squishSound, 0);
                    // Randomly respawn bug
                    // Randomly respawn bug
                    bugX = random(0, SCREEN_WIDTH - BUG_WIDTH);
                    bugY = random(0, SCREEN_HEIGHT - BUG_HEIGHT);
                }
            }
        }

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);


        // Draw background image
        if (backgroundTexture) {
            drawTexturedRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, backgroundTexture);
            glDeleteTextures(1, &backgroundTexture);
        }

        // Draw bug
        drawTexturedRect(bugX, bugY, BUG_WIDTH, BUG_HEIGHT, bugTexture);

        // Update screen
        SDL_GL_SwapWindow(window);

        // Delay to control frame rate
        SDL_Delay(10);
    }

    // Free resources
    Mix_FreeMusic(backgroundMusic);
    Mix_FreeChunk(squishSound);

    // Destroy window
    SDL_DestroyWindow(window);

    // Quit SDL subsystems
    Mix_Quit();
    SDL_Quit();

    return 0;
}
