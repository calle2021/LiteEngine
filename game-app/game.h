#pragma once
#include "game-system.h"

class Game : public GameApp
{
  public:
    Game();
    ~Game();
    void Init() override;
    void Update() override;
    void Destroy() override;
};