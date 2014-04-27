#ifndef GAMECONTEXT_HPP
#define GAMECONTEXT_HPP

#include <SDL2plus.hpp>
#include <GLplus.hpp>

#include "rendercontext.hpp"
#include "scene.hpp"

class GameContext
{
    std::unique_ptr<SDL2plus::LibSDL> mpSDL;
    std::unique_ptr<SDL2plus::WindowGL> mpWindow;
    std::shared_ptr<GLplus::FrameBuffer> mpWindowFrameBuffer;
    std::unique_ptr<RenderContext> mpRenderContext;

    std::unique_ptr<Scene> mpCurrentScene;

    Uint32 mMillisecondsPerUpdate;

public:
    GameContext(int argc, char* argv[]);

    void MainLoop();

private:
    void Update(unsigned int deltaTimeMS);

    void Render(RenderContext& renderContext, float partialUpdatePercentage);
};

#endif // GAMECONTEXT_HPP
