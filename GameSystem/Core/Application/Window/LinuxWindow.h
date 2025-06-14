#pragma once
#include "Window.h"

namespace GameSystem {
class LinuxWindow : public Window {
   public:
    LinuxWindow(WindowProps props);
    bool Update() override;
};
}  // namespace GameSystem