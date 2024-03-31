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
#include <OgreCamera.h>
#include <OgreSceneNode.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
using namespace Ogre;

int windowWidth=1024;
int windowHeight=768;
//int windowWidth=1920;
//int windowHeight=1080;

// Bug dimensions
const int BUG_WIDTH = 50;
const int BUG_HEIGHT = 50;

bool isFullScreen = false; // false=fullscreen

bool autoload = false;
int clipsize = 5;
bool reload = false;
int range = 10 - -10 + 1;
//int num = rand() % range + min;

const float CAMERA_MOVE_SPEED = 10.0f;

SDL_DisplayMode dm;

#define SINDEN

// Shot mark structure
struct ShotMark
{
    int x;
    int y;
    int lifespan; // Lifespan in milliseconds
    Uint32 spawnTime; // Time when the shot mark was spawned
};

void toggleFullScreen(SDL_Window* window, bool currentState)
{
    isFullScreen = !currentState;

    SDL_SetWindowFullscreen(window, !currentState);
    SDL_ShowCursor(currentState);
}


int main(int argc, char* argv[]) {


  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }
    // Initialize SDL
   SDL_DisplayMode dm;
  if (SDL_GetCurrentDisplayMode(0, &dm) == 0) {
          std::cout << "Current resolution: " << dm.w << "x" << dm.h << std::endl;
        std::cout << "Refresh rate: " << dm.refresh_rate << " Hz" << std::endl;
    windowWidth = dm.w;
    windowHeight = dm.h;
} else {

         std::cerr << "Failed to get display mode: " << SDL_GetError() << std::endl;
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

        // Load sound effects
    Mix_Chunk* squishSound = Mix_LoadWAV("squish.wav");
    if (!squishSound)
    {
        std::cerr << "Failed to load squish sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }

  // Create SDL window
  SDL_Window* window = SDL_CreateWindow("SDL2 & Ogre3D Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

      bool cursortoggle = true;
    toggleFullScreen(window,isFullScreen);
    SDL_ShowCursor(SDL_ENABLE);

        // Seed random number generator
    std::srand(std::time(nullptr));

        int shotsFired = 0;

  // Initialize Ogre
  Ogre::Root* root = new Ogre::Root();
  root->loadPlugin("RenderSystem_GL"); // Load OpenGL RenderSystem
  Ogre::RenderSystem* renderSystem = root->getRenderSystemByName("OpenGL Rendering Subsystem");
  root->setRenderSystem(renderSystem);
  root->initialise(false);
//renderSystem->setCapability(Ogre::RSC_SHADOWS);

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
  Ogre::RenderWindow* ogreWindow = root->createRenderWindow("OgreWindow", windowWidth, windowHeight, false, &params);


  // Create scene
  Ogre::SceneManager* sceneManager = root->createSceneManager();
//sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
sceneManager->setShadowTextureSize(1024); // Set shadow map resolution
sceneManager->setShadowTextureCount(1);   // Set the number of shadow maps
sceneManager->setShadowCasterRenderBackFaces(false);
      // without light we would just get a black screen
    Ogre::Light* light = sceneManager->createLight("MainLight");
    Ogre::SceneNode* lightNode = sceneManager->getRootSceneNode()->createChildSceneNode();
    //lightNode->setPosition(0, 5, 11);
     lightNode->setPosition(20, 80, 50);

    lightNode->attachObject(light);
  // Create camera and viewport
      SceneNode* camNode = sceneManager->getRootSceneNode()->createChildSceneNode();
  Ogre::Camera* camera = sceneManager->createCamera("Camera");
  camNode->setPosition(Ogre::Vector3(0, 0, 50));
  camNode->lookAt(Ogre::Vector3(0, 0, 0), Node::TransformSpace::TS_WORLD);

      camera->setNearClipDistance(5);
    camNode->attachObject(camera);


  Ogre::Viewport* viewport = ogreWindow->addViewport(camera);
  viewport->setBackgroundColour(Ogre::ColourValue(0.5, 0.5, 0.5));
    sceneManager->setAmbientLight(ColourValue(0, 0, 0));
  //  sceneManager->setShadowTechnique(ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);

  //https://forums.ogre3d.org/viewtopic.php?t=64514
  Ogre::String lNameOfResourceGroup = "resources : maindirectory";
  		Ogre::ResourceGroupManager& lRgMgr = Ogre::ResourceGroupManager::getSingleton();
		lRgMgr.createResourceGroup(lNameOfResourceGroup);
		Ogre::String lDirectoryToLoad = "./";
		bool lIsRecursive = false;
		lRgMgr.addResourceLocation(lDirectoryToLoad, "FileSystem", lNameOfResourceGroup, lIsRecursive);

		// The function 'initialiseResourceGroup' parses scripts if any in the locations.
		lRgMgr.initialiseResourceGroup(lNameOfResourceGroup);

		// Files that can be loaded are loaded.
		lRgMgr.loadResourceGroup(lNameOfResourceGroup);



//Add Cube
 //Ogre::Entity* cubeEntity = sceneManager->createEntity("test.mesh");
   // Ogre::Entity* cubeEntity = sceneManager->createEntity("Cube.mesh");
        Ogre::Entity* cubeEntity = sceneManager->createEntity("ogrehead.mesh");
   //     Ogre::Entity* cubeEntity = sceneManager->createEntity("test.obj");
//
  Ogre::SceneNode* cubeNode = sceneManager->getRootSceneNode()->createChildSceneNode();
  cubeNode->attachObject(cubeEntity);
  cubeNode->scale(0.1,0.1,0.1);

// Load particle system from script file
Ogre::ParticleSystem* particleSystem = sceneManager->createParticleSystem("MyParticleSystem", "Examples/Smoke");

// Attach particle system to a scene node
Ogre::SceneNode* particleNode = sceneManager->getRootSceneNode()->createChildSceneNode();
particleNode->attachObject(particleSystem);
//particleNode->setPosition(10,10,0);
//particleNode->scale(0.1,0.1,0.1);
// // Create a material
//Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName("Material");

//// Assign the material to the entity's submeshes
//for (unsigned int i = 0; i < cubeEntity->getNumSubEntities(); ++i) {
//    cubeEntity->getSubEntity(i)->setMaterial(material);
//}


//Add Ground Plane
Ogre::Plane groundPlane(Ogre::Vector3::UNIT_Y, -10);
Ogre::MeshManager::getSingleton().createPlane(
    "GroundMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    groundPlane, 1000, 1000, 1, 1, true, 1, 10, 10, Ogre::Vector3::UNIT_Z);

          Ogre::Entity* groundEntity = sceneManager->createEntity("GroundEntity", "GroundMesh");
groundEntity->setMaterialName("GroundMaterial");
    sceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);



// Load the HUD texture
Ogre::TextureManager& textureManager = Ogre::TextureManager::getSingleton();
Ogre::TexturePtr hudTexture = textureManager.load("SindenBorderWhiteLarge.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

// Create a material for the HUD
//Ogre::MaterialPtr hudMaterial = Ogre::MaterialManager::getSingleton().create("HUDMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
Ogre::MaterialPtr hudMaterial = Ogre::MaterialManager::getSingleton().getByName("hud_material");
//hudMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("SindenBorderWhiteLarge.png");

//    Ogre::Pass *myMatPass = Ogre::MaterialManager::getSingletonPtr()->getByName("myMaterial")->getTechnique(0)->getPass(0);
//    myMatPass->getTextureUnitState(0)->setTexture( hudMaterial );


Ogre::OverlaySystem* pOverlaySystem = OGRE_NEW Ogre::OverlaySystem();
sceneManager->addRenderQueueListener(pOverlaySystem);
// Create an overlay for the HUD
Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
Ogre::Overlay* overlay = overlayManager.create("HUDOverlay");

//// Create a panel to display the HUD texture
Ogre::OverlayContainer* panel = static_cast<Ogre::OverlayContainer*>(overlayManager.createOverlayElement("Panel", "HUDPanel"));
panel->setMaterialName("HUDMaterial");
panel->setMetricsMode(Ogre::GMM_PIXELS);
panel->setPosition(0, 0);
panel->setDimensions(windowWidth, windowHeight); // Set dimensions according to your HUD texture size

// Add the panel to the overlay
overlay->add2D(panel);

// Show the overlay
overlay->show();




// Create a water surface mesh (e.g., a plane)
Ogre::Plane waterPlane(Ogre::Vector3::UNIT_Y, 0);
Ogre::MeshManager::getSingleton().createPlane(
    "WaterSurface", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    waterPlane, 1000, 1000, 1, 1, true, 1, 10, 10, Ogre::Vector3::UNIT_Z);

// Create a water surface entity
Ogre::Entity* waterEntity = sceneManager->createEntity("WaterSurface", "WaterSurface");

// Apply a water material to the water surface entity
waterEntity->setMaterialName("WaterMaterial");

// Attach the water surface entity to the scene
Ogre::SceneNode* cubeNode2 =sceneManager->getRootSceneNode()->createChildSceneNode();

cubeNode2->attachObject(waterEntity);
cubeNode2->setPosition(1,-1,1);
cubeNode2->setVisible(true);
//Ogre::SceneNode* testNode = sceneManager->getRootSceneNode()->get


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
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            // Convert mouse coordinates to world space
            Ogre::Ray mouseRay = camera->getCameraToViewportRay(mouseX / static_cast<float>(windowWidth), mouseY / static_cast<float>(windowHeight));

            // Perform ray-casting to check for intersection with the model
            Ogre::RaySceneQuery* rayQuery = sceneManager->createRayQuery(mouseRay);
            rayQuery->setSortByDistance(true);

            Ogre::RaySceneQueryResult& result = rayQuery->execute();
            for (auto& hit : result) {
                if (hit.movable && hit.movable->getParentSceneNode() == cubeNode) {
                    // If the model is clicked, play animation and hide the model
                   // cubeNode->setVisible(false);
                  //  cubeNode->flipVisibility(true);

  cubeNode->setPosition(rand() % range + -10,rand() % range + -10,0);

                    // Play animation here
                    break; // No need to check further
                }
            }
            sceneManager->destroyQuery(rayQuery);
        }
                   switch( event.type )
            {
                case SDL_KEYDOWN:


                switch( event.key.keysym.sym )
                {
                    case SDLK_ESCAPE:
                        running = false;
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
                                            case SDLK_w: // Move camera forward
                        camNode->translate(camNode->getOrientation() * Vector3::UNIT_Z * CAMERA_MOVE_SPEED);
                        break;
                    case SDLK_s: // Move camera backward
                        camNode->translate(camNode->getOrientation() * -Vector3::UNIT_Z * CAMERA_MOVE_SPEED);
                        break;
                    case SDLK_a: // Move camera left
                        camNode->translate(camNode->getOrientation() * -Vector3::UNIT_X * CAMERA_MOVE_SPEED);
                        break;
                    case SDLK_d: // Move camera right
                        camNode->translate(camNode->getOrientation() * Vector3::UNIT_X * CAMERA_MOVE_SPEED);
                        break;
                    case SDLK_q: // Move camera up
                        camNode->translate(Vector3::UNIT_Y * CAMERA_MOVE_SPEED);
                        break;
                    case SDLK_e: // Move camera down
                        camNode->translate(-Vector3::UNIT_Y * CAMERA_MOVE_SPEED);
                        break;
                    default:
                        break;
                }
            }
            // Handle joystick axis motion
            if (event.type == SDL_JOYAXISMOTION)
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
//            // Handle mouse click
//            if (e.type == SDL_MOUSEBUTTONDOWN || reload)
//            {
//                if (e.button.button == SDL_BUTTON_LEFT)
//                {
//                    // Left mouse button
//                    int mouseX, mouseY;
//                    SDL_GetMouseState(&mouseX, &mouseY);
//                    mouseY = SCREEN_HEIGHT - mouseY;
//                    SDL_Rect mouseRect = { mouseX, mouseY, 1, 1 };
//                    SDL_Rect bugRect = { bugX, bugY, BUG_WIDTH, BUG_HEIGHT };
//                    if (shotsFired <= clipsize)
//                    {
//                        if (checkCollision(bugRect, mouseRect))
//                        {
//                            // Bug squashed!
//                            std::cout << "Bug squashed!" << std::endl;
//                            Mix_PlayChannel(-1, squishSound, 0); // Play squish sound
//                            bugX = std::rand() % (SCREEN_WIDTH - BUG_WIDTH); // Generate new bug position
//                            bugY = std::rand() % (SCREEN_HEIGHT - BUG_HEIGHT);
//                        }
//                        else
//                        {
//                            // Shot missed
//                            std::cout << "Missed shot!" << std::endl;
//                            // Draw shot mark
//                            ShotMark newShotMark = { mouseX, mouseY, 1000, SDL_GetTicks() }; // 10 seconds lifespan
//                            shotMarks.push_back(newShotMark);
//                        }
//                        // Increment shots fired
//                        shotsFired++;
//                    }
//                    else
//                    {
//                        // Check if reload is needed
//                        if (shotsFired >= clipsize && autoload)
//                        {
//                            std::cout << "Reloaded!" << std::endl;
//                            shotsFired = 0; // Reset shots fired.
//                        };
//                        std::cout << "reload!" << std::endl;
//                        Mix_PlayChannel(-1, reloadSound, 0); // Play squish sound
//                    }
//                }
//                else if (e.button.button == SDL_BUTTON_RIGHT || reload)
//                {
//                    // Right mouse button for reload
//                    std::cout << "Manual reload!" << std::endl;
//                    shotsFired = 0; // Reset shots fired
//                    Mix_PlayChannel(-1, reloadSound, 0); // Play squish sound
//                }
//                reload=false;
//            }
      //  }
    }
//see
    // Update scene
    cubeNode->yaw(Ogre::Degree(0.1f));

    // Render frame
    root->renderOneFrame();

    SDL_Delay(1);
  }
     // Free resources
    Mix_FreeChunk(squishSound);
   // Mix_FreeChunk(backgroundMusic);

  // Cleanup
  delete root;
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
