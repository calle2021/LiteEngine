#include "GameSystem.h"
#include "Game.h"
#include <memory>

int main()
{
    return GameSystem::Application::Launch(std::make_unique<Game>());
}