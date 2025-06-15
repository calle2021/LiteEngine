#pragma once
#include "game-app.h"
#include <memory>

namespace GameSystem
{
class Application
{
  public:
    static int Launch(std::unique_ptr<GameApp> game);
};
} // namespace GameSystem