#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
SDL_Rect videoRect;

bool initializeSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Video Player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    return true;
}

bool loadVideo(const char* filename)
{
    SDL_Surface* videoSurface = SDL_LoadBMP(filename);
    if (videoSurface == NULL)
    {
        printf("SDL_LoadBMP Error: %s\n", SDL_GetError());
        return false;
    }

    texture = SDL_CreateTextureFromSurface(renderer, videoSurface);
    if (texture == NULL)
    {
        SDL_FreeSurface(videoSurface);
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
        return false;
    }

    SDL_FreeSurface(videoSurface);

    return true;
}

void handleEvents(bool* quit, bool* pause, float* speed)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                *quit = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_SPACE)
                    *pause = !(*pause);
                else if (event.key.keysym.sym == SDLK_UP)
                    *speed += 0.1f;
                else if (event.key.keysym.sym == SDLK_DOWN)
                    *speed -= 0.1f;
                break;
        }
    }
}

void renderFrame()
{
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, &videoRect);
    SDL_RenderPresent(renderer);
}

void cleanup()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main()
{
    const char* videoFile = NULL;
    printf("Enter the video file path: ");
    scanf("%ms", &videoFile);

    if (!initializeSDL())
    {
        free((void*)videoFile);
        return EXIT_FAILURE;
    }

    if (!loadVideo(videoFile))
    {
        free((void*)videoFile);
        cleanup();
        return EXIT_FAILURE;
    }

    free((void*)videoFile);

    videoRect.x = 0;
    videoRect.y = 0;
    videoRect.w = WINDOW_WIDTH;
    videoRect.h = WINDOW_HEIGHT;

    bool quit = false;
    bool pause = false;
    float speed = 1.0f;

    while (!quit)
    {
        handleEvents(&quit, &pause, &speed);

        if (!pause)
        {
            renderFrame();
            SDL_Delay((Uint32)(1000 / (30 * speed)));  // Assuming 30 FPS as an example
        }
    }

    cleanup();
    return EXIT_SUCCESS;
}
