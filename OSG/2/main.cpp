#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgGA/FirstPersonManipulator>


#include <osgDB/ReadFile>

#include <SDL2/SDL.h>

#include <iostream>

bool convertEvent(SDL_Event& event, osgGA::EventQueue& eventQueue)
{
    switch (event.type) {

        case SDL_MOUSEMOTION:
            eventQueue.mouseMotion(event.motion.x, event.motion.y);
            return true;

        case SDL_MOUSEBUTTONDOWN:
            eventQueue.mouseButtonPress(event.button.x, event.button.y, event.button.button);
            return true;

        case SDL_MOUSEBUTTONUP:
            eventQueue.mouseButtonRelease(event.button.x, event.button.y, event.button.button);
            return true;

        case SDL_KEYUP:
            eventQueue.keyRelease( (osgGA::GUIEventAdapter::KeySymbol) event.key.keysym.sym);
            return true;

        case SDL_KEYDOWN:
            eventQueue.keyPress( (osgGA::GUIEventAdapter::KeySymbol) event.key.keysym.sym);
            return true;

        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                eventQueue.windowResize(0, 0, event.window.data1, event.window.data2);
                return true;
            }
            break;

        default:
            break;
    }
    return false;
}

int main( int argc, char **argv )
{
    if (argc < 2)
    {
        std::cout << argv[0] << ": requires filename argument." << std::endl;
        return 1;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    // Load the scene
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readRefNodeFile(argv[1]);
    if (!loadedModel)
    {
        std::cout << argv[0] << ": No data loaded." << std::endl;
        return 1;
    }

    // Set up the window size
    unsigned int windowWidth = 800;
    unsigned int windowHeight = 600;

    // Set up SDL OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Create SDL window
    SDL_Window* window = SDL_CreateWindow("OpenSceneGraph SDL2 Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Create SDL OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        std::cerr << "Failed to create SDL OpenGL context: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Create an OSG viewer
    osgViewer::Viewer viewer;
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> gw = viewer.setUpViewerAsEmbeddedInWindow(0, 0, windowWidth, windowHeight);

    // Set up camera
    viewer.getCamera()->setDrawBuffer(GL_BACK);
    viewer.getCamera()->setReadBuffer(GL_BACK);

    // Set scene data
    viewer.setSceneData(loadedModel.get());
 //   viewer.setCameraManipulator(new osgGA::TrackballManipulator);
    viewer.setCameraManipulator(new osgGA::FirstPersonManipulator);

    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.realize();

    bool done = false;
    while (!done)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            // Pass the SDL event into the viewer's event queue
            convertEvent(event, *(gw->getEventQueue()));

            switch (event.type)
            {
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        done = true;
                    break;

                case SDL_QUIT:
                    done = true;
                    break;
            }
        }

        if (done)
            continue;

        // Draw the new frame
        viewer.frame();

        // Swap buffers
        SDL_GL_SwapWindow(window);
    }

    // Clean up
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);

    return 0;
}
