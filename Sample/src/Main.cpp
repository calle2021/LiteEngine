#include "PIXL.h"
#include "Sample.h"

#ifdef _WIN32
int WINAPI wWinMain(
    _In_ HINSTANCE hins,
    _In_opt_ HINSTANCE pins,
    _In_ LPWSTR cmdline,
    _In_ int showcmd
)
{
#ifdef _DEBUG
    PIXL::CONSOLE();
    PIXL::Logger::Init();
#endif
    auto pixl_sample = new Sample();
    int res = PIXL::Application::Run(pixl_sample, { hins, pins, cmdline, showcmd });
    delete pixl_sample;
    return res;
}
#else
#error "Only support Windows."
#endif
