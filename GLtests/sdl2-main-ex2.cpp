#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
//g++ main.cpp -lSDL2 -lGLU -lGL

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;


// Function to initialize SDL2 and create a window
bool initSDL(SDL_Window*& window, SDL_GLContext& context) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create window
    window = SDL_CreateWindow("SDL2 OpenGL Demo",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create OpenGL context
    context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        printf("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Set OpenGL attributes
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Set up perspective projection with glu
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    return true;
}

// Function to render OpenGL content
void render(SDL_Window* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Render a triangle strip
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1.0f, 0.0f, 0.0f);  // Red
    glVertex3f(-0.5f, 0.5f, -1.0f);
    glColor3f(0.0f, 1.0f, 0.0f);  // Green
    glVertex3f(-0.5f, -0.5f, -1.0f);
    glColor3f(0.0f, 0.0f, 1.0f);  // Blue
    glVertex3f(0.5f, 0.5f, -1.0f);
    glColor3f(1.0f, 1.0f, 0.0f);  // Yellow
    glVertex3f(0.5f, -0.5f, -1.0f);
    glEnd();

    SDL_GL_SwapWindow(window);
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_GLContext context = NULL;

    if (!initSDL(window, context)) {
        printf("Failed to initialize!\n");
        return -1;
    }

    // Main loop flag
    bool quit = false;
    SDL_Event e;

    // Main loop
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Render scene
        render(window);
    }

    // Destroy window and context
    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(context);

    // Quit SDL subsystems
    SDL_Quit();

    return 0;
}
