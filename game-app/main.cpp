#include "game-system.h"
#include "game.h"
#include <memory>

int main()
{
    return GameSystem::Application::Launch(std::make_unique<Game>());
}