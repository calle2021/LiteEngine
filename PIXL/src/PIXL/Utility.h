#pragma once
#include <windows.h>
#include <iostream>

namespace PIXL {

#ifdef _WIN32

    struct WinArgs
    {
        HINSTANCE hins;
        HINSTANCE pins;
        LPWSTR cmd;
        int showcmd;
    };

#ifdef _DEBUG
    inline void AllocateConsole()
    {
        AllocConsole();
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
    }
    #define CONSOLE() AllocateConsole()
#endif
#endif
}
