#pragma once


#ifdef _WIN32
#define LAUNCH( game ) \
    int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int ) \
    { \
        return GameSystem::CoreApplication::Run(game()); \
    }
#else
#define LAUNCH( game ) \
    int main(int argc, char *argv[]) \
    { \
        PIXL::Application::Init();
        return PIXL::Application::Run(game()); \
    }
#endif
