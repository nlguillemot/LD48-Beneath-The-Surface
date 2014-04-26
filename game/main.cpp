#include <SDL2plus.hpp>

void run()
{
    SDL2plus::LibSDL sdl(SDL_INIT_VIDEO);
    sdl.SetGLAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    sdl.SetGLAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    sdl.SetGLAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL2plus::WindowGL window("Beneath the Surface",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              640, 480, SDL_WINDOW_OPENGL);

    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                goto MainLoopEnd;
            }
        }

        window.SwapBuffers();
    }

    MainLoopEnd:;
}

int main(int argc, char* argv[])
{
    try
    {
        run();
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "Fatal exception: %s\n", e.what());
        throw;
    }
    catch (...)
    {
        fprintf(stderr, "Unknown fatal exception\n");
        throw;
    }
}
