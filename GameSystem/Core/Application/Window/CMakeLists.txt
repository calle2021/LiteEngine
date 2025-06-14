set(SOURCES Window.cpp Window.h)

if(WIN32)
    list(APPEND SOURCES WindowsWindow.cpp WindowsWindow.h)
elseif(UNIX)
    list(APPEND SOURCES LinuxWindow.cpp LinuxWindow.h)
endif()

target_sources(GameSystem PRIVATE ${SOURCES})