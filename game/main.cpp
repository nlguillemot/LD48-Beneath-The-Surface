#include <SDL2plus.hpp>

void run()
{

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
