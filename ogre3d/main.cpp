#include <SDL2/SDL.h>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreSceneManager.h>
#include <OgreEntity.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <SDL2/SDL_syswm.h>
#include <OgreMeshManager.h>
#include <OgreTextureManager.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <Ogre.h>
#include <OgreOverlay.h>
#include <OgreOverlaySystem.h>

using namespace Ogre;

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

      // without light we would just get a black screen
    Ogre::Light* light = sceneManager->createLight("MainLight");
    Ogre::SceneNode* lightNode = sceneManager->getRootSceneNode()->createChildSceneNode();
    lightNode->setPosition(0, 10, 15);
    lightNode->attachObject(light);
  // Create camera and viewport
  Ogre::Camera* camera = sceneManager->createCamera("Camera");
//  camera->setPosition(Ogre::Vector3(0, 0, 50));
 // camera->lookAt(Ogre::Vector3(0, 0, 0));
  Ogre::Viewport* viewport = ogreWindow->addViewport(camera);
  viewport->setBackgroundColour(Ogre::ColourValue(0.5, 0.5, 0.5));

  //https://forums.ogre3d.org/viewtopic.php?t=64514
  Ogre::String lNameOfResourceGroup = "Mission 1 : Deliver Tom";
  		Ogre::ResourceGroupManager& lRgMgr = Ogre::ResourceGroupManager::getSingleton();
		lRgMgr.createResourceGroup(lNameOfResourceGroup);
		Ogre::String lDirectoryToLoad = "./";
		bool lIsRecursive = false;
		lRgMgr.addResourceLocation(lDirectoryToLoad, "FileSystem", lNameOfResourceGroup, lIsRecursive);

		// The function 'initialiseResourceGroup' parses scripts if any in the locations.
		lRgMgr.initialiseResourceGroup(lNameOfResourceGroup);

		// Files that can be loaded are loaded.
		lRgMgr.loadResourceGroup(lNameOfResourceGroup);




    Ogre::Entity* cubeEntity = sceneManager->createEntity("test.mesh");
  Ogre::SceneNode* cubeNode = sceneManager->getRootSceneNode()->createChildSceneNode();
  cubeNode->attachObject(cubeEntity);

Ogre::Plane plane(Vector3::UNIT_Y, -10);
    Ogre::MeshManager::getSingleton().createPlane(
            "ground", Ogre::RGN_DEFAULT,
            plane,
            1500, 1500, 20, 20,
            true,
            1, 5, 5,
            Vector3::UNIT_Z);

                Entity* groundEntity = sceneManager->createEntity("ground");
    sceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);


// Load the HUD texture
Ogre::TextureManager& textureManager = Ogre::TextureManager::getSingleton();
Ogre::TexturePtr hudTexture = textureManager.load("SindenBorderWhiteLarge.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
//
//// Create a material for the HUD
Ogre::MaterialPtr hudMaterial = Ogre::MaterialManager::getSingleton().create("HUDMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
hudMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("SindenBorderWhiteLarge.png");
//
Ogre::OverlaySystem* pOverlaySystem = OGRE_NEW Ogre::OverlaySystem();
sceneManager->addRenderQueueListener(pOverlaySystem);
//// Create an overlay for the HUD
Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
Ogre::Overlay* overlay = overlayManager.create("HUDOverlay");
//Ogre::Overlay* overlay = Ogre::OverlayManager::getSingleton().getByName("myMaterial");

//// Create a panel to display the HUD texture
Ogre::OverlayContainer* panel = static_cast<Ogre::OverlayContainer*>(overlayManager.createOverlayElement("Panel", "HUDPanel"));
panel->setMaterialName("HUDMaterial");
panel->setMetricsMode(Ogre::GMM_PIXELS);
panel->setPosition(0, 0);
panel->setDimensions(300, 400); // Set dimensions according to your HUD texture size

// Add the panel to the overlay
overlay->add2D(panel);

// Show the overlay
overlay->show();






//// Create a fullscreen quad
//Ogre::ManualObject* fullscreenQuad = sceneManager->createManualObject("FullscreenQuad");
//fullscreenQuad->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
//
//// Define vertices for a fullscreen quad (spanning the entire screen)
//fullscreenQuad->position(-1, 1, 0); // Top left
//fullscreenQuad->textureCoord(0, 0); // Texture coordinate (top left)
//
//fullscreenQuad->position(1, 1, 0); // Top right
//fullscreenQuad->textureCoord(1, 0); // Texture coordinate (top right)
//
//fullscreenQuad->position(-1, -1, 0); // Bottom left
//fullscreenQuad->textureCoord(0, 1); // Texture coordinate (bottom left)
//
//fullscreenQuad->position(1, -1, 0); // Bottom right
//fullscreenQuad->textureCoord(1, 1); // Texture coordinate (bottom right)
//
//// Define the faces of the quad
//fullscreenQuad->quad(0, 1, 3, 2);
//
//// End definition
//fullscreenQuad->end();
//
//// Create a material for the fullscreen quad
//Ogre::MaterialPtr fullscreenMaterial = Ogre::MaterialManager::getSingleton().create("FullscreenMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
//fullscreenMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("SindenBorderWhiteLarge.png.png");
//
//// Attach the material to the fullscreen quad
//fullscreenQuad->setMaterialName(0,"FullscreenMaterial");
//fullscreenQuad->setMaterial(0,fullscreenMaterial);
//
//// Attach the quad to the root scene node
//Ogre::SceneNode* rootNode = sceneManager->getRootSceneNode()->createChildSceneNode();
//rootNode->attachObject(fullscreenQuad);

//rootNode->setVisible(true);


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
