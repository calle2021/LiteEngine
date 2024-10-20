#pragma once

#define CREATE_APPLICATION( app ) \
        int WINAPI wWinMain(_In_ HINSTANCE hins, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int showcmd) \
        { \
            auto pixl_sample = new app(); \
            int res = PIXL::Application::Run( app(), {hins, showcmd}); \
            delete pixl_sample; \
            return res; \
        }