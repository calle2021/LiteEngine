#pragma once
#include <windows.h>
#include <iostream>

namespace GameSystem {

#ifdef _WIN32

    struct WinArgs
    {
        HINSTANCE hins;
        int showcmd;
    };

#ifdef _DEBUG
    inline void AllocateConsole()
    {
        AllocConsole();
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
    }
    #define ALLOCATE_CONSOLE() AllocateConsole()
#endif
#endif
}
