#include <SDL2/SDL.h>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreSceneManager.h>
#include <OgreEntity.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <SDL2/SDL_syswm.h>

int main(int argc, char* argv[]) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

  // Create SDL window
  SDL_Window* window = SDL_CreateWindow("SDL2 & Ogre3D Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

  // Initialize Ogre
  Ogre::Root* root = new Ogre::Root();
  root->loadPlugin("RenderSystem_GL"); // Load OpenGL RenderSystem
  Ogre::RenderSystem* renderSystem = root->getRenderSystemByName("OpenGL Rendering Subsystem");
  root->setRenderSystem(renderSystem);
  root->initialise(false);

  SDL_SysWMinfo sdlInfo;
  SDL_VERSION(&sdlInfo.version);
  SDL_GetWindowWMInfo(window, &sdlInfo);

  // Create render window with SDL window handle
  Ogre::NameValuePairList params;
  //params["externalWindowHandle"] = Ogre::StringConverter::toString(reinterpret_cast<size_t>(sdlInfo.info.x11.window));

  // Create Ogre window
  //Ogre::NameValuePairList params;
  params["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)sdlInfo.info.x11.window);
 // params["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)window);
  Ogre::RenderWindow* ogreWindow = root->createRenderWindow("OgreWindow", 800, 600, false, &params);

  // Create scene
  Ogre::SceneManager* sceneManager = root->createSceneManager();
  //Ogre::Entity* cubeEntity = sceneManager->createEntity("Cube", "./test.3ds");
 // Ogre::SceneNode* cubeNode = sceneManager->getRootSceneNode()->createChildSceneNode();
 // cubeNode->attachObject(cubeEntity);

  // Create camera and viewport
  Ogre::Camera* camera = sceneManager->createCamera("Camera");
//  camera->setPosition(Ogre::Vector3(0, 0, 50));
 // camera->lookAt(Ogre::Vector3(0, 0, 0));
  Ogre::Viewport* viewport = ogreWindow->addViewport(camera);
  viewport->setBackgroundColour(Ogre::ColourValue(0.5, 0.5, 0.5));

  // Main loop
  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }
//see
    // Update scene
  //  cubeNode->yaw(Ogre::Degree(0.1f));

    // Render frame
    root->renderOneFrame();
  }

  // Cleanup
  delete root;
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
