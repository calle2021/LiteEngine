#pragma once
#include "game-app.h"
#include <memory>

namespace GameSystem
{
class GameSystem
{
  public:
    static int launch(std::unique_ptr<GameApp> game);
};
} // namespace GameSystem