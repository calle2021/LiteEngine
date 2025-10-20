#pragma once
#include "LiteEngine.h"

class LiteBox : public iLiteBox
{
  public:
    void Init() override;
    void Update() override;
    void Destroy() override;
};
