#include <iostream>
#include <memory>
#include <stdexcept>

#include "game-system.h"
#include "game.h"

int main()
{
    try
    {
        GameSystem::Application().Launch(std::make_unique<Game>());
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Caught a runtime_error: " << e.what() << std::endl;
    }
}