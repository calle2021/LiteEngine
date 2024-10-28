#pragma once
#include "Event.h"

namespace GameSystem {
    class MouseEvent : public Event {
    public:
        MouseEvent(int x, int y) : x(x), y(y) {}
        int GetType() const override { return type; };
    protected:
        int type = 1;
        int x, y;
    };
}