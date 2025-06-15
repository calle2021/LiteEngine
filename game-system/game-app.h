#pragma once

class GameApp
{
  public:
    virtual ~GameApp() = default;
    virtual void Update() = 0;
    virtual void Init() = 0;
};