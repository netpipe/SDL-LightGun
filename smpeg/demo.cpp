#include <SDL.h>
#include <smpeg/smpeg.h>

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SMPEG* mpeg = NULL;



    // Load the video file
    mpeg = SMPEG_new(argv[1], NULL, 0);
    if (!mpeg) {
     //   SMPEG_error("Couldn't load MPEG video");
      //  SMPEG_Quit();
        SDL_Quit();
        return 1;
    }

    // Set up SDL video mode
    SDL_Surface* screen = SDL_SetVideoMode(640, 480, 0, SDL_HWSURFACE|SDL_DOUBLEBUF); // Set a default size for now
    if (!screen) {
       // SMPEG_error("Couldn't set SDL video mode");
        SMPEG_delete(mpeg);
    //    SMPEG_Quit();
        SDL_Quit();
        return 1;
    }

    // Set up the video surface
    SDL_Overlay* overlay = SDL_CreateYUVOverlay(640, 480, SDL_YV12_OVERLAY, screen); // Again, default size for now
    if (!overlay) {
     //   SMPEG_error("Couldn't create SDL overlay");
        SMPEG_delete(mpeg);
     //   SMPEG_Quit();
        SDL_Quit();
        return 1;
    }

    // Play the video
    SMPEG_setdisplay(mpeg, screen, NULL, NULL);
    SMPEG_setvolume(mpeg, 100);
    SMPEG_play(mpeg);

    // Main loop
    SDL_Event event;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        SDL_Delay(10);
    }

    // Clean up
    SMPEG_stop(mpeg);
    SMPEG_delete(mpeg);
 //   SMPEG_Quit();
    SDL_Quit();

    return 0;
}
