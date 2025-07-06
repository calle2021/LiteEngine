#include <iostream>
#include <memory>
#include <stdexcept>

#include "game-system.h"
#include "game.h"

int main()
{
    return GameSystem::Application().Launch(std::make_unique<Game>());
}