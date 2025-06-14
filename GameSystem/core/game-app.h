#pragma once

class GameApp
{
  public:
    virtual ~GameApp() = default;
    virtual void update() = 0;
    virtual void init() = 0;
};