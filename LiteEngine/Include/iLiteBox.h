#pragma once

class iLiteBox
{
  public:
    virtual ~iLiteBox() = default;
    virtual void Update() = 0;
    virtual void Init() = 0;
    virtual void Destroy() = 0;
};