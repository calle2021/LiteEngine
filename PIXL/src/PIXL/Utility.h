#pragma once
#include <windows.h>
#include <iostream>

namespace PIXL {
#ifdef _DEBUG
    #ifdef _WIN32
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