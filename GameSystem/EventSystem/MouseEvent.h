#pragma once
#include "Event.h"

namespace GameSystem {
    class MouseEvent : public Event {
    public:
        MouseEvent(int x, int y) : x(x), y(y) {}
        int GetType() const override { return type; };

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseEvent: " << x << ", " << y;
            return ss.str();
        }
    protected:
        int type = 1;
        int x, y;
    };
}