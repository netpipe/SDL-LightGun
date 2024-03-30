#include <Ogre.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <OgreConfigDialog.h> // Include OgreConfigDialog.h for ConfigDialog
#include <GL/gl.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Bug dimensions
const float BUG_WIDTH = 50.0f;
const float BUG_HEIGHT = 50.0f;

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

    // Initialize Ogre3D
    Ogre::Root root;
    Ogre::ConfigDialog *configDialog;
    if (!root.showConfigDialog(configDialog)) {
        return 1; // Dialog canceled
    }

    // Create render window
    Ogre::RenderWindow* window = root.initialise(true, "Bug Squish");

    // Create scene manager
    Ogre::SceneManager* sceneMgr = root.createSceneManager();

    // Create camera
    Ogre::Camera* camera = sceneMgr->createCamera("MainCamera");
   // camera->setPosition(Ogre::Vector3(0, 100, 500));
  //  camera->lookAt(Ogre::Vector3(0, 0, 0));
    camera->setNearClipDistance(5);

    // Create viewport
    Ogre::Viewport* viewport = window->addViewport(camera);
    viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));

    // Set aspect ratio
    camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

    // Load background music
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }
    Mix_Music* backgroundMusic = Mix_LoadMUS("music.mp3");
    if (!backgroundMusic) {
        std::cerr << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }
    Mix_PlayMusic(backgroundMusic, -1);

    // Seed random number generator
    std::srand(std::time(nullptr));

    // Create entity and node for the bug
    Ogre::Entity* bugEntity = sceneMgr->createEntity("Bug", "test.3ds");
    Ogre::SceneNode* bugNode = sceneMgr->getRootSceneNode()->createChildSceneNode("BugNode");
    bugNode->attachObject(bugEntity);

    // Set initial position of the bug
    bugNode->setPosition(random(-SCREEN_WIDTH / 2, SCREEN_WIDTH / 2), 0, random(-SCREEN_HEIGHT / 2, SCREEN_HEIGHT / 2));

    // Main loop flag
    bool quit = false;

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

        // Render Ogre3D scene
        root.renderOneFrame();

        // Update screen
       // SDL_GL_SwapWindow(window);
        window->swapBuffers();
        // Delay to control frame rate
        SDL_Delay(10);
    }

    // Free resources
    root.destroySceneManager(sceneMgr);

    // Quit SDL subsystems
    Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}
