#include <Ogre.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <OgreConfigDialog.h> // Include OgreConfigDialog.h for ConfigDialog
#include <SDL2/SDL_syswm.h>
int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create SDL window
    SDL_Window* sdlWindow = SDL_CreateWindow("Ogre3D Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!sdlWindow) {
        std::cerr << "SDL window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Initialize Ogre3D
    Ogre::Root root;
    Ogre::ConfigDialog *configDialog;
    if (!root.showConfigDialog(configDialog)) {
        return 1; // Dialog canceled
    }
    root.restoreConfig();

    // Get SDL window handle
    SDL_SysWMinfo sdlInfo;
    SDL_VERSION(&sdlInfo.version);
    SDL_GetWindowWMInfo(sdlWindow, &sdlInfo);

    // Create render window with SDL window handle
    Ogre::NameValuePairList params;
    params["externalWindowHandle"] = Ogre::StringConverter::toString(reinterpret_cast<size_t>(sdlInfo.info.x11.window));
    Ogre::RenderWindow* renderWindow = root.createRenderWindow("Ogre3D Demo", 800, 600, false, &params);

    // Create scene manager
    Ogre::SceneManager* sceneMgr = root.createSceneManager();

    // Create camera
    Ogre::Camera* camera = sceneMgr->createCamera("MainCamera");
   // camera->setPosition(0, 0, 500);
   // camera->lookAt(0, 0, 0);

    // Create viewport
    Ogre::Viewport* viewport = renderWindow->addViewport(camera);
    viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));

    // Create cube entity
    Ogre::Entity* cubeEntity = sceneMgr->createEntity("Cube", Ogre::SceneManager::PT_CUBE);
    Ogre::SceneNode* cubeNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
    cubeNode->attachObject(cubeEntity);

    // Main loop flag
    bool quit = false;

    // Main loop
    while (!quit) {
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Rotate cube
        cubeNode->yaw(Ogre::Degree(0.5f));

        // Clear viewport
    //    renderWindow->clear();

        // Render scene
        root.renderOneFrame();

        // Present the rendered image
        renderWindow->update();

        // Delay to control frame rate
        SDL_Delay(10);
    }

    // Destroy Ogre3D resources
    root.destroySceneManager(sceneMgr);

    // Destroy SDL window
    SDL_DestroyWindow(sdlWindow);

    // Quit SDL
    SDL_Quit();

    return 0;
}
