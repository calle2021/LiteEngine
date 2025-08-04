#include <iostream>
#include <memory>
#include <stdexcept>

#include "game-system.h"
#include "game.h"

int main()
{
    try {
        LiteEngine::Application().Launch(std::make_unique<Game>());
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}