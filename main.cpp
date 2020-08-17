#include <SDL.h>
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init();
bool loadMedia();
void close();

SDL_Window * window = NULL;
SDL_Renderer * renderer = NULL;

bool init()
{
    bool success = true;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf( "Warning: Linear texture filtering not enabled!" );
        }
        window = SDL_CreateWindow("Stardust Paint (DEV)",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf( "Window could not be created! SDL_Error: %s\n",
                SDL_GetError() );
            success = false;
        }
        else
        {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if (renderer == NULL)
            {
                printf( "Renderer could not be created! SDL Error: %s\n",
                    SDL_GetError() );
				success = false;
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }
    
    return success;
}

bool loadMedia()
{
    // Load media here
    return true;
}

void close()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;
    
    SDL_Quit();
}

int main(int argc, char * args[])
{
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
            SDL_Event e;
            while (isRunning)
            {
                if (SDL_WaitEvent(&e) != 0)
                {
                    switch(e.type)
                    {
                        case SDL_QUIT:
                            isRunning = false;
                            break;
                    }
                }
                
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);
            }
        }
    }
        
    close();
    
    return 0;
}