#include "gamecontext.hpp"

int main(int argc, char* argv[])
{
    try
    {
        GameContext game(argc, argv);
        game.MainLoop();
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
