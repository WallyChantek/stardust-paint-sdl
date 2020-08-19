#include <SDL.h>
#include <stdio.h>
#include <cstring>
#include <cmath>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init();
bool loadMedia();
void close();

SDL_Window * window = NULL;
SDL_Renderer * renderer = NULL;
SDL_Texture * displayTexture = NULL;
SDL_Surface * canvas = NULL;
SDL_Surface * floatingCanvas = NULL;
SDL_Rect floatingCanvasRect;
SDL_Surface * overlay = NULL;
SDL_Rect overlayRect;

int mouseX = 0;
int mouseY = 0;
int oldMouseX = 0;
int oldMouseY = 0;

bool init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }
    else
    {
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf( "Warning: Linear texture filtering not enabled!" );
        }
        window = SDL_CreateWindow("Stardust Paint (DEV)",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
            SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf( "Window could not be created! SDL_Error: %s\n",
                SDL_GetError() );
            return false;
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == NULL)
        {
            printf( "Renderer could not be created! SDL Error: %s\n",
                SDL_GetError() );
            return false;
        }
        // SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        
        canvas = SDL_CreateRGBSurfaceWithFormat(NULL,
                                                SCREEN_WIDTH, SCREEN_HEIGHT,
                                                8, SDL_PIXELFORMAT_BGRA32);
        SDL_FillRect(canvas, NULL, SDL_MapRGB(canvas->format, 200, 190, 230));
        
        floatingCanvas = SDL_CreateRGBSurfaceWithFormat(NULL,
                                                200, 200,
                                                8, SDL_PIXELFORMAT_BGRA32);
        SDL_FillRect(floatingCanvas, NULL, SDL_MapRGB(floatingCanvas->format, 0, 255, 0));
        floatingCanvasRect.x = 0;
        floatingCanvasRect.y = 0;
        floatingCanvasRect.w = 200;
        floatingCanvasRect.h = 200;
        
        overlay = SDL_CreateRGBSurfaceWithFormat(NULL,
                                                100, 100,
                                                8, SDL_PIXELFORMAT_BGRA32);
        SDL_FillRect(overlay, NULL, SDL_MapRGB(overlay->format, 255, 0, 0));
        overlayRect.x = 0;
        overlayRect.y = 0;
        overlayRect.w = 100;
        overlayRect.h = 100;
        
        displayTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    
    return true;
}

bool loadMedia()
{
    // Load media here
    return true;
}

void close()
{
    SDL_DestroyTexture(displayTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(canvas);
    SDL_FreeSurface(floatingCanvas);
    SDL_FreeSurface(overlay);
    displayTexture = NULL;
    renderer = NULL;
    window = NULL;
    canvas = NULL;
    floatingCanvas = NULL;
    overlay = NULL;
    
    SDL_Quit();
}

void putPixel(SDL_Surface * surface, int x, int y, int r, int g, int b)
{
    Uint32 * pixels = (Uint32 *)surface->pixels;
    pixels[y * SCREEN_WIDTH + x] = SDL_MapRGB(surface->format, r, g, b);
}

void drawLine(SDL_Surface * surface, int x0, int y0, int x1, int y1)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int steps = 0;
    if (abs(dx) > abs(dy))
        steps = abs(dx);
    else
        steps = abs(dy);
    
    float incrementX = dx / (float) steps;
    float incrementY = dy / (float) steps;
    
    // TODO: Check out this stuff here, verify against some other DDA line
    // algorithm implementations (see if there are C++ ones) to make sure we're
    // not skipping any pixels at the beginning or whatever
    float x = x0;
    float y = y0;
    
    for (int v = 0; v < steps; v++)
    {
        x = x + incrementX;
        y = y + incrementY;
        putPixel(surface, (int)round(x), (int)round(y), 255, 0, 0);
    }
}

int main(int argc, char * args[])
{
    
    bool leftMouseButtonDown = false;
    
    if (!init())
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        if (!loadMedia())
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            bool isRunning = true;
            SDL_Event event;
            while (isRunning)
            {
                SDL_UpdateTexture(displayTexture, NULL, canvas->pixels, canvas->pitch);
                SDL_UpdateTexture(displayTexture, &floatingCanvasRect, floatingCanvas->pixels, floatingCanvas->pitch);
                SDL_UpdateTexture(displayTexture, &overlayRect, overlay->pixels, overlay->pitch);
                
                if (SDL_WaitEvent(&event) != 0)
                {
                    switch(event.type)
                    {
                        case SDL_MOUSEBUTTONUP:
                            if (event.button.button == SDL_BUTTON_LEFT)
                                leftMouseButtonDown = false;
                            break;
                        case SDL_MOUSEBUTTONDOWN:
                            if (event.button.button == SDL_BUTTON_LEFT)
                                    leftMouseButtonDown = true;
                        case SDL_MOUSEMOTION:
                            mouseX = event.motion.x;
                            mouseY = event.motion.y;
                            if (leftMouseButtonDown)
                            {
                                if (SDL_MUSTLOCK(canvas))
                                    SDL_LockSurface(canvas);
                                
                                // TODO: Handle off-screen "drawing" so we don't
                                // have jumpy lines and stuff when moving back
                                // into the window
                                drawLine(canvas, oldMouseX, oldMouseY, mouseX, mouseY);
                                
                                if (SDL_MUSTLOCK(canvas))
                                    SDL_UnlockSurface(canvas);
                            }
                            break;
                        case SDL_QUIT:
                            isRunning = false;
                            break;
                    }
                }
                
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, displayTexture, NULL, NULL);
                SDL_RenderPresent(renderer);
                
                oldMouseX = mouseX;
                oldMouseY = mouseY;
            }
        }
    }
        
    close();
    
    return 0;
}