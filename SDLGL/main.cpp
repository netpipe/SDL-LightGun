#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <GL/glut.h>
//g++ main.cpp -lGL -lglut -lSDL2_image -lSDL2_mixer -lSDL2

#define STB_IMAGE_IMPLEMENTATION    
#include "stb_image.h"

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Bug dimensions
const int BUG_WIDTH = 50;
const int BUG_HEIGHT = 50;

bool isFullScreen = false; // false=fullscreen

bool autoload = false;
int clipsize = 5;
bool reload = false;

#define SINDEN

// Shot mark structure
struct ShotMark 
{
    int x;
    int y;
    int lifespan; // Lifespan in milliseconds
    Uint32 spawnTime; // Time when the shot mark was spawned
};

// Function to check collision between two rectangles
bool checkCollision(SDL_Rect a, SDL_Rect b) 
{
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

void renderTexture(int id, float x0, float y0, float x1, float y1)
{
    glBindTexture(GL_TEXTURE_2D, id);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glColor4f(1, 1, 1, 1);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex2f(x0, y0);
        glTexCoord2f(1, 0);
        glVertex2f(x1, y0);
        glTexCoord2f(1, 1);
        glVertex2f(x1, y1);
        glTexCoord2f(0, 1);
        glVertex2f(x0, y1);
    glEnd();
}

int loadImage(const char* filename)
{
    int w;
    int h;
    int comp;
    unsigned char* image = stbi_load(filename, &w, &h, &comp, STBI_rgb_alpha);

    if (image == nullptr)
    {
        std::cout << "cannot load " << filename << std:: endl;
        return 0;
    }
    unsigned int texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    if (comp == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    else if (comp == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(image);
    return texID;
}

int main(int argc, char **argv)
{ 

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) 
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) 
    {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) 
    {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }

    // Load background music
    Mix_Music* backgroundMusic = Mix_LoadMUS("music.mp3");
    if (!backgroundMusic) 
    {
        std::cerr << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }

    // Play background music
    Mix_PlayMusic(backgroundMusic, -1); // -1 for looping

    // Create window
    SDL_Window* window = SDL_CreateWindow("Bug Squash", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    
    if (window == nullptr) 
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_GLContext Context = SDL_GL_CreateContext(window);

    int backGround = loadImage("SindenBorderWhiteLarge_Wide.png");
    int bug = loadImage("bug.png");
    // Load shot mark image
    int shotMark = loadImage("bullet-hole.png");

    // Load sound effects
    Mix_Chunk* squishSound = Mix_LoadWAV("squish.wav");
    if (!squishSound) 
    {
        std::cerr << "Failed to load squish sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }
    // Load sound effects
    Mix_Chunk* reloadSound = Mix_LoadWAV("reload.wav");
    if (!reloadSound) 
    {
        std::cerr << "Failed to load reloadSound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }

    bool cursortoggle = true;
    toggleFullScreen(window,isFullScreen);
    SDL_ShowCursor(SDL_ENABLE);

    // Bug position
    int bugX = SCREEN_WIDTH / 2 - BUG_WIDTH / 2;
    int bugY = SCREEN_HEIGHT / 2 - BUG_HEIGHT / 2;

    // Seed random number generator
    std::srand(std::time(nullptr));

    int shotsFired = 0;

    // Main loop flag
    bool quit = false;

    // Shot marks vector
    std::vector<ShotMark> shotMarks;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Main loop
    while (!quit) 
    {
        // Event handler
        SDL_Event e;
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) 
        {
            // User requests quit
            if (e.type == SDL_QUIT) 
            {
                quit = true;
            }
            switch( e.type )
            {
                case SDL_KEYDOWN:


                switch( e.key.keysym.sym )
                {
                    case SDLK_ESCAPE: 
                        quit = true; 
                        break;
                    case SDLK_1: 
                        SDL_ShowCursor(!cursortoggle);
                        cursortoggle=!cursortoggle;
                        SDL_Delay(10); 
                        break;
                    case SDLK_5: 
                        reload=true;
                        autoload=!autoload; 
                        std::cerr << autoload << "test" << std::endl;
                        if (autoload)
                        {
                            std::cerr << "autoloadon" << std::endl;
                        }; 
                        SDL_Delay(100);
                        break;
                }
            }
            // Handle joystick axis motion
            if (e.type == SDL_JOYAXISMOTION) 
            {
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
            if (e.type == SDL_MOUSEBUTTONDOWN || reload) 
            {
                if (e.button.button == SDL_BUTTON_LEFT) 
                { 
                    // Left mouse button
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    mouseY = SCREEN_HEIGHT - mouseY;
                    SDL_Rect mouseRect = { mouseX, mouseY, 1, 1 };
                    SDL_Rect bugRect = { bugX, bugY, BUG_WIDTH, BUG_HEIGHT };
                    if (shotsFired <= clipsize) 
                    {
                        if (checkCollision(bugRect, mouseRect)) 
                        {
                            // Bug squashed!
                            std::cout << "Bug squashed!" << std::endl;
                            Mix_PlayChannel(-1, squishSound, 0); // Play squish sound
                            bugX = std::rand() % (SCREEN_WIDTH - BUG_WIDTH); // Generate new bug position
                            bugY = std::rand() % (SCREEN_HEIGHT - BUG_HEIGHT);
                        } 
                        else 
                        {
                            // Shot missed
                            std::cout << "Missed shot!" << std::endl;
                            // Draw shot mark
                            ShotMark newShotMark = { mouseX, mouseY, 1000, SDL_GetTicks() }; // 10 seconds lifespan
                            shotMarks.push_back(newShotMark);
                        }
                        // Increment shots fired
                        shotsFired++;
                    } 
                    else 
                    {
                        // Check if reload is needed
                        if (shotsFired >= clipsize && autoload) 
                        {
                            std::cout << "Reloaded!" << std::endl;
                            shotsFired = 0; // Reset shots fired.
                        };
                        std::cout << "reload!" << std::endl;
                        Mix_PlayChannel(-1, reloadSound, 0); // Play squish sound
                    }
                } 
                else if (e.button.button == SDL_BUTTON_RIGHT || reload) 
                { 
                    // Right mouse button for reload
                    std::cout << "Manual reload!" << std::endl;
                    shotsFired = 0; // Reset shots fired
                    Mix_PlayChannel(-1, reloadSound, 0); // Play squish sound
                }
                reload=false;
            }
        }



        // Clear screen
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClearColor(25.0f/255.0f, 25 / 255.0f, 80 / 255.0f, 255.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        #ifdef SINDEN
            renderTexture(backGround, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        #endif
        // Render shot marks
        for (auto it = shotMarks.begin(); it != shotMarks.end();) 
        {
            if (SDL_GetTicks() - it->spawnTime > it->lifespan) 
            {
                it = shotMarks.erase(it); // Remove shot mark if its lifespan has elapsed
            } 
            else 
            {
                renderTexture(shotMark, it->x, it->y, it->x+10, it->y+10);
                ++it;
            }
        }
        // Render bug
        renderTexture(bug, bugX, bugY, bugX+ BUG_WIDTH, bugY+ BUG_HEIGHT);

        // swap the window
        SDL_GL_SwapWindow(window);

        // Delay to control frame rate
        SDL_Delay(10);
    }

    // Free resources
    Mix_FreeChunk(squishSound);
    SDL_DestroyWindow(window);

    // Quit SDL subsystems
    IMG_Quit();
    Mix_Quit();
    SDL_Quit();

    return 0;
}
