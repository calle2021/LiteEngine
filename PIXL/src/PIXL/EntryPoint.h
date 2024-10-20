#pragma once

#ifdef _WIN32
#define CREATE_APPLICATION( game ) \
    int WINAPI wWinMain(_In_ HINSTANCE hins, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int showcmd) \
    { \
        PIXL::Application::Init({hins, showcmd}); \
        return PIXL::Application::Run(game()); \
    }
#else
#define CREATE_APPLICATION( game ) \
    int main(int argc, char *argv[]) \
    { \
        PIXL::Application::Init();
        return PIXL::Application::Run(game()); \
    }
#endif