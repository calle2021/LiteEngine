#include "gamesystem.h"
#include "game.h"
#include <memory>

int main()
{
    return GameSystem::GameSystem::launch(std::make_unique<Game>());
}