#pragma once

namespace GameSystem {
	enum class EventType {
		MouseLeftClick,
		MouseRightClick
	};

	class Event {
	public:
		virtual ~Event() = default;
        bool handled = false;
        virtual int GetType() const = 0;
	};

    class Dispatcher
    {
    public:
        Dispatcher(Event& event) : m_event(event)
        {
        }
        template<typename T, typename F>
        bool Dispatch(const F& func)
        {
            return m_event |= func(static_cast<T&>(m_event));
        }
    private:
        Event& m_event;
    };

}