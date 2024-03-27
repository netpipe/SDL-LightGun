#include <iostream>
#include <osgViewer/Viewer>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgUtil/LineSegmentIntersector>
#include <osgGA/TrackballManipulator> // Added header
#include <SDL2/SDL.h>

//g++ main.cpp -lSDL2 -lSDL2_image -lSDL2_ttf -losg -losgUtil -losgDB -losgViewer -losgGA

osg::Node* createModel(const std::string& filename) {
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(filename);
    if (!loadedModel) {
        std::cerr << "Failed to load model file: " << filename << std::endl;
        return nullptr;
    }
    return loadedModel.release();
}

int main(int argc, char** argv) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create an SDL window
    SDL_Window* window = SDL_CreateWindow("OpenSceneGraph with SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create an SDL OpenGL context
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Load COLLADA model
    osg::Node* modelNode = createModel("test.3ds");
    if (!modelNode) {
        std::cerr << "Failed to load model!" << std::endl;
        return 1;
    }

    // Initialize OpenSceneGraph viewer
    osgViewer::Viewer viewer;

    // Create a root node
    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->addChild(modelNode);

    // Set up the viewer
    viewer.setSceneData(root);
    viewer.setCameraManipulator(new osgGA::TrackballManipulator);

    // Main loop
    while (!viewer.done()) {
        // Render the scene
        viewer.frame();

        // Swap buffers
        SDL_GL_SwapWindow(window);

        // Handle SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    viewer.setDone(true);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int mouseX = event.button.x;
                        int mouseY = event.button.y;

                        // Perform collision detection with mouse
                        osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, mouseX, mouseY);
                        osgUtil::IntersectionVisitor iv(intersector);
                        viewer.getCamera()->accept(iv);

                        if (intersector->containsIntersections()) {
                            osgUtil::LineSegmentIntersector::Intersection intersection = *(intersector->getIntersections().begin());
                         //   std::cout << "Collision detected at point: " << intersection.getWorldIntersectPoint() << std::endl;
                        } else {
                            std::cout << "No collision detected." << std::endl;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    // Cleanup
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

