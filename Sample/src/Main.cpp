#include "PIXL.h"
#include "Sample.h"

#ifdef _WIN32
int WINAPI wWinMain(
    _In_ HINSTANCE Hinstance,
    _In_opt_ HINSTANCE PrevInstance,
    _In_ LPWSTR CmdLine,
    _In_ int ShowCmd
)
{
#ifdef _DEBUG
    PIXL::CONSOLE();
    PIXL::Logger::Init();
    PIXL_APP_INFO("Hello from App!");
#endif
    auto pixl_sample = new Sample();
    int res = PIXL::WinApplication::Run(pixl_sample, { Hinstance, PrevInstance, CmdLine, ShowCmd });
    delete pixl_sample;
    return res;
}
#else
#error "Only support Windows."
#endif
