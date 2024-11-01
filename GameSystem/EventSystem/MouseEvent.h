#pragma once
#include "Event.h"

namespace GameSystem {
    class MouseClickedEvent : public Event {
    public:
        MouseClickedEvent(int t, int x, int y) :type(t), x(x), y(y) {}
        int GetType() const override { return type; };

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseClickedEvent: " << type << ", " << x << ", " << y;
            return ss.str();
        }
    protected:
        int type = 1;
        int x, y;
    };
}