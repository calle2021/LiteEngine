#include "PIXL.h"
#include "Sample.h"

#ifdef _WIN32
int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
)
{
#ifdef _DEBUG
    PIXL::CONSOLE();
    //PIXL::Logger::Init();
#endif
    auto sample = new Sample();
    int res = PIXL::WinApplication::Run(sample, {hInstance, hPrevInstance, lpCmdLine, nShowCmd});
    delete sample;
    return res;
}
#else
#error "Only support Windows."
#endif
