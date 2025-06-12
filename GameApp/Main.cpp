#include "GameSystem.h"
#include "Game.h"
#include <iostream>

//int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int )
int main()
{
    Game game;
    GameSystem::Application app(&game);
    return app.Run();
}