#include "gamecontext.hpp"

GameContext::GameContext(int argc, char* argv[])
{
    mSDL.reset(new SDL2plus::LibSDL(SDL_INIT_VIDEO));
    mSDL->SetGLAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    mSDL->SetGLAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    mSDL->SetGLAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    mWindow.reset(new SDL2plus::WindowGL(
                      "Beneath the Surface",
                      SDL_WINDOWPOS_UNDEFINED,
                      SDL_WINDOWPOS_UNDEFINED,
                      640, 480, SDL_WINDOW_OPENGL));

    mMillisecondsPerUpdate = 1000/60;
}

void GameContext::MainLoop()
{
    Uint32 lastTime = SDL_GetTicks();
    Uint32 timeLag = 0;

    while (true)
    {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        timeLag += deltaTime;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                goto MainLoopEnd;
            }
        }

        while (timeLag >= mMillisecondsPerUpdate)
        {
            Update();
            timeLag -= mMillisecondsPerUpdate;
        }

        float partialUpdatePercentage = (float) timeLag / mMillisecondsPerUpdate;
        Render(partialUpdatePercentage);
    }

    MainLoopEnd:;
}

void GameContext::Update()
{

}

void GameContext::Render(float partialUpdatePercentage)
{
    mWindow->SwapBuffers();
}
