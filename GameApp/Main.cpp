#include "GameSystem.h"
#include "Game.h"
#include <iostream>

int main()
{
    Game game;
    GameSystem::Application app(&game);
    return app.Run();
}