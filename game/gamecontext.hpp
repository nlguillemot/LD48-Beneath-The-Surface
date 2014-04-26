#ifndef GAMECONTEXT_HPP
#define GAMECONTEXT_HPP

#include <SDL2plus.hpp>

class GameContext
{
    std::unique_ptr<SDL2plus::LibSDL> mSDL;
    std::unique_ptr<SDL2plus::WindowGL> mWindow;

    Uint32 mMillisecondsPerUpdate;

public:
    GameContext(int argc, char* argv[]);

    void MainLoop();

private:
    void Update();

    void Render(float partialUpdatePercentage);
};

#endif // GAMECONTEXT_HPP
