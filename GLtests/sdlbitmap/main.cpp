#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
SDL_Texture* gFontTexture = nullptr;

bool init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create window
    gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create renderer
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}

bool loadFont(const std::string& path) {
    // Load font texture
    SDL_Surface* fontSurface = IMG_Load(path.c_str());
    if (fontSurface == nullptr) {
        std::cerr << "Unable to load font image! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create texture from surface pixels
    gFontTexture = SDL_CreateTextureFromSurface(gRenderer, fontSurface);
    if (gFontTexture == nullptr) {
        std::cerr << "Unable to create texture from font image! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Free loaded surface
    SDL_FreeSurface(fontSurface);

    return true;
}

void close() {
    // Free loaded images
    SDL_DestroyTexture(gFontTexture);
    gFontTexture = nullptr;

    // Destroy window and renderer
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    gRenderer = nullptr;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

void renderText(const std::string& text, int x, int y, int charWidth, int charHeight) {
    int charPerRow = SCREEN_WIDTH / charWidth;

    // Iterate through each character in the text
    for (size_t i = 0; i < text.length(); ++i) {
        char ch = text[i];
        int srcX = (ch % charPerRow) * charWidth;
        int srcY = (ch / charPerRow) * charHeight;

        SDL_Rect srcRect = { srcX, srcY, charWidth, charHeight };
        SDL_Rect destRect = { x + (i * charWidth), y, charWidth, charHeight };

        SDL_RenderCopy(gRenderer, gFontTexture, &srcRect, &destRect);
    }
}

int main(int argc, char* args[]) {
    if (!init()) {
        std::cerr << "Failed to initialize!" << std::endl;
        return -1;
    }

    if (!loadFont("font.png")) {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    SDL_Event e;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        // Render "Hello, World!" at position (100, 100) with character size 20x20
        renderText("Hello, World!", 100, 100, 20, 20);

        SDL_RenderPresent(gRenderer);
    }

    close();

    return 0;
}
