#include <irrlicht/irrlicht.h>
#include <iostream>
#include <vector>
#include <ctime>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Bug dimensions
const int BUG_WIDTH = 50;
const int BUG_HEIGHT = 50;

// Shot mark structure
struct ShotMark {
    int x;
    int y;
    int lifespan; // Lifespan in milliseconds
    u32 spawnTime; // Time when the shot mark was spawned
};

// Function to check collision between two rectangles
bool checkCollision(const core::rect<s32>& a, const core::rect<s32>& b) {
    return !(a.LowerRightCorner.X <= b.UpperLeftCorner.X || a.UpperLeftCorner.X >= b.LowerRightCorner.X || a.LowerRightCorner.Y <= b.UpperLeftCorner.Y || a.UpperLeftCorner.Y >= b.LowerRightCorner.Y);
}
// Custom event receiver for handling mouse clicks
class MyEventReceiver : public IEventReceiver {
public:
    MyEventReceiver() {}

    // This method is called by the engine when an event happens
    virtual bool OnEvent(const SEvent& event) {
        // If the event is a mouse click
        if (event.EventType == EET_MOUSE_INPUT_EVENT) {
            // If the left mouse button is pressed
            if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN) {
                // Store the position of the mouse click
                mouseClickPosition = vector2d<s32>(event.MouseInput.X, event.MouseInput.Y);
                // Set the flag to indicate that a mouse click has occurred
                isMouseClicked = true;
            }
        }
        return false;
    }

    // Function to check if a mouse click occurred
    bool IsMouseClicked() const {
        return isMouseClicked;
    }

    // Function to get the position of the mouse click
    vector2d<s32> GetMouseClickPosition() const {
        return mouseClickPosition;
    }

    // Function to reset the mouse click flag after processing
    void ResetMouseClicked() {
        isMouseClicked = false;
    }

private:
    bool isMouseClicked = false; // Flag to indicate if a mouse click occurred
    vector2d<s32> mouseClickPosition; // Position of the mouse click
};
int main() {

    // Create device
    IrrlichtDevice *device = createDevice(EDT_OPENGL, dimension2d<u32>(SCREEN_WIDTH, SCREEN_HEIGHT), 16, false, false, false, 0);

    if (!device) {
        std::cerr << "Failed to create Irrlicht device!" << std::endl;
        return 1;
    }

    // Get video driver
    video::IVideoDriver* driver = device->getVideoDriver();
    // Get scene manager
    scene::ISceneManager* smgr = device->getSceneManager();
    // Get GUI environment
    gui::IGUIEnvironment* guienv = device->getGUIEnvironment();
    // Create event receiver
     MyEventReceiver receiver;
  device->setEventReceiver(&receiver);
    // Load background texture
    ITexture* backgroundTexture = driver->getTexture("SindenBorderWhiteLarge_Wide.png");
    if (!backgroundTexture) {
        std::cerr << "Failed to load background texture!" << std::endl;
        device->drop();
        return 1;
    }

    // Load bug texture
    ITexture* bugTexture = driver->getTexture("bug.png");
    if (!bugTexture) {
        std::cerr << "Failed to load bug texture!" << std::endl;
        device->drop();
        return 1;
    }

    // Load shot mark texture
    ITexture* shotMarkTexture = driver->getTexture("bullet-hole.png");
    if (!shotMarkTexture) {
        std::cerr << "Failed to load shot mark texture!" << std::endl;
        device->drop();
        return 1;
    }

    // Load background music
    //ISoundEngine* soundEngine = createIrrKlangDevice();
    //if (!soundEngine) {
   //     std::cerr << "Failed to create sound engine!" << std::endl;
   //     device->drop();
   //     return 1;
   // }
    //soundEngine->play2D("background_music.mp3", true); // Looping background music

    // Seed random number generator
    std::srand(std::time(nullptr));

    // Shot marks vector
    std::vector<ShotMark> shotMarks;

    // Main loop flag
    bool quit = false;
    // Render bug
    int bugX = rand() % (SCREEN_WIDTH - BUG_WIDTH);
    int bugY = rand() % (SCREEN_HEIGHT - BUG_HEIGHT);
    // Main loop
    while (!quit && device->run()) {
        if (device->isWindowActive()) {
            // Clear the screen
            driver->beginScene(true, true, SColor(255, 255, 255, 255));

            // Render background
            driver->draw2DImage(backgroundTexture, core::position2d<s32>(0, 0),
                core::rect<s32>(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), 0, SColor(255, 255, 255, 255), true);


            driver->draw2DImage(bugTexture, core::position2d<s32>(bugX, bugY),
                core::rect<s32>(0, 0, BUG_WIDTH, BUG_HEIGHT), 0, SColor(255, 255, 255, 255), true);
            // If a mouse click occurred
             if (receiver.IsMouseClicked()) {
                 // Get the position of the mouse click
                 vector2d<s32> mousePos = receiver.GetMouseClickPosition();
                 // Add shot mark with a lifespan of 10 seconds
                 ShotMark newShotMark = { mousePos.X - 5, mousePos.Y - 5, 10000, device->getTimer()->getTime() };
                 shotMarks.push_back(newShotMark);
                 // Reset the mouse click flag after processing
                 receiver.ResetMouseClicked();
             }

             // Render shot marks
             u32 currentTime = device->getTimer()->getTime();
             for (auto it = shotMarks.begin(); it != shotMarks.end();) {
                 if (currentTime - it->spawnTime > it->lifespan) {
                     it = shotMarks.erase(it); // Remove shot mark if its lifespan has elapsed
                 } else {
                     driver->draw2DImage(shotMarkTexture, core::position2d<s32>(it->x, it->y),
                         core::rect<s32>(0, 0, 10, 10), 0, SColor(255, 255, 255, 255), true);
                     ++it;
                 }
             }

            // End scene rendering
            driver->endScene();
        } else {
            device->yield();
        }
    }

    // Drop the sound engine
   // soundEngine->drop();

    // Drop the device
    device->drop();

    return 0;
}

