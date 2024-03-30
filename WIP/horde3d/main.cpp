#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <Horde3D.h>
#include "Horde3DUtils.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <GL/gl.h>
// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Bug dimensions
const float BUG_WIDTH = 50.0f;
const float BUG_HEIGHT = 50.0f;
H3DNode      cam;

// Function to generate a random number within a range
int random(int min, int max) {
    return min + rand() % (max - min + 1);
}

int main(int argc, char* args[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }

        int _renderInterface = H3DRenderDevice::OpenGL2;
    // Initialize Horde3D
    if (!h3dInit( ( H3DRenderDevice::List ) _renderInterface ) ) {
        std::cerr << "Horde3D could not initialize!" << std::endl;
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("Bug Squish", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

    // Load background music
    Mix_Music* backgroundMusic = Mix_LoadMUS("background_music.mp3");
    if (!backgroundMusic) {
        std::cerr << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }
    Mix_PlayMusic(backgroundMusic, -1);

    // Seed random number generator
    std::srand(std::time(nullptr));

    // Create bug node
    H3DNode bugNode = h3dAddModelNode(H3DRootNode, "test.3ds",H3DResTypes::SceneGraph);

    // Main loop flag
    bool quit = false;

    // Game variables
    float bugX = random(0, SCREEN_WIDTH - BUG_WIDTH);
    float bugY = random(0, SCREEN_HEIGHT - BUG_HEIGHT);
	cam = h3dAddCameraNode( H3DRootNode, "Camera", 400 );

    // Main loop
    while (!quit) {
        // Handle events on queue
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up view and projection matrices
      //  h3dutPerspectiveMatrix(gProjMat, 45.0f, SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);
      //  h3dSetMatrix(H3DRenderContext::CamProjMat, gProjMat);

        // Update bug position
        h3dSetNodeTransform(bugNode, bugX, 0, bugY, 0, 0, 0, 1, 1, 1);

        // Render scene
        h3dRender(cam);

        // Update screen
        SDL_GL_SwapWindow(window);

        // Delay to control frame rate
        SDL_Delay(10);
    }

    // Free resources
    h3dRelease();

    // Destroy window
    SDL_DestroyWindow(window);

    // Quit SDL subsystems
    Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}

