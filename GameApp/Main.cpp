#include "GameSystem.h"
#include "Game.h"

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int )
{
    Game game;
    GameSystem::CoreApplication core_app(&game);
    return core_app.Run();
}
