#include "gamecontext.hpp"

#include "worldscene.hpp"

GameContext::GameContext(int argc, char* argv[])
{
    mpSDL.reset(new SDL2plus::LibSDL(SDL_INIT_VIDEO));
    mpSDL->SetGLAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    mpSDL->SetGLAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    mpSDL->SetGLAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    mpWindow.reset(new SDL2plus::WindowGL(
                      "Beneath the Surface",
                      SDL_WINDOWPOS_UNDEFINED,
                      SDL_WINDOWPOS_UNDEFINED,
                      640, 480, SDL_WINDOW_OPENGL));

    mpWindowFrameBuffer.reset(new GLplus::FrameBuffer(GLplus::DefaultFrameBuffer()));

    Viewport viewport(glm::ivec2(0), glm::ivec2(mpWindow->GetWidth(), mpWindow->GetHeight()));
    mpRenderContext.reset(new RenderContext());
    mpRenderContext->CurrentFrameBuffer = mpWindowFrameBuffer;
    mpRenderContext->CurrentViewport = viewport;

    mpCurrentScene.reset(new WorldScene());

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
            Update(mMillisecondsPerUpdate);
            timeLag -= mMillisecondsPerUpdate;
        }

        float partialUpdatePercentage = (float) timeLag / mMillisecondsPerUpdate;
        Render(*mpRenderContext, partialUpdatePercentage);
    }

    MainLoopEnd:;
}

void GameContext::Update(unsigned int deltaTimeMS)
{
    mpCurrentScene->Update(deltaTimeMS);
}

void GameContext::Render(RenderContext& renderContext, float partialUpdatePercentage)
{
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLplus::CheckGLErrors();

    mpCurrentScene->Render(renderContext, partialUpdatePercentage);
    mpWindow->SwapBuffers();
}
