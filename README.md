# C-VideoPlayer
C语言视频播放器
1. 引入所需的头文件：
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

2. 定义窗口的宽度和高度：

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480


3. 声明全局变量以存储窗口、渲染器和纹理对象：

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
SDL_Rect videoRect;

4. 定义一个函数 `initializeSDL()` 用于初始化SDL库：

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

这个函数首先调用 `SDL_Init()` 初始化SDL库，并检查是否成功初始化。然后，创建一个窗口对象和一个渲染器对象。窗口的大小由 `WINDOW_WIDTH` 和 `WINDOW_HEIGHT` 宏定义决定。渲染器使用硬件加速和垂直同步。

5. 定义一个函数 `loadVideo()` 用于加载视频文件
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

这个函数通过调用 `SDL_LoadBMP()` 加载视频文件，并将其转换为纹理对象 `texture`。如果加载或转换过程中发生错误，会打印相应的错误消息。

6. 定义一个函数 `handleEvents()` 用于处理SDL事件：

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

这个函数通过调用 `SDL_PollEvent()` 处理所有等待的事件，包括键盘事件和退出事件。根据按下的键，更新相应的变量：`quit`（退出应用程序）、`pause`（暂停/播放）和 `speed`（速度控制）。

7. 定义一个函数 `renderFrame()` 用于渲染视频帧：

void renderFrame()
{
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, &videoRect);
    SDL_RenderPresent(renderer);
}


这个函数清除渲染器的内容，将视频纹理复制到渲染器的目标矩形中，然后更新渲染器的显示。

8. 定义一个函数 `cleanup()` 用于清理资源：


void cleanup()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


这个函数释放纹理、渲染器和窗口对象，并调用 `SDL_Quit()` 关闭SDL库。

9. 主函数 `main()` 实现主要逻辑：

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

主函数首先获取用户输入的视频文件路径，并调用 `initializeSDL()` 进行SDL库的初始化。然后，调用 `loadVideo()` 加载视频文件。如果任何一步失败，将释放资源并返回退出代码。接下来，初始化视频矩形的位置和大小，并定义一些控制变量。

然后，进入主循环，不断处理事件并渲染视频帧。如果暂停标志未设置，调用 `renderFrame()` 渲染帧，并使用 `SDL_Delay()` 控制帧速率。循环继续，直到退出标志设置为 true。

最后，在退出之前调用 `cleanup()` 函数清理所有

的资源，并返回成功退出代码。
