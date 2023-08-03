#pragma once

#include "Event.h"

#include <sstream>

namespace oil{
    class OIL_API KeyEvent : public Event{
    public:
        inline int GetKeyCode() const { return m_KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput )

    protected:
        KeyEvent(int keycode)
            : m_KeyCode(keycode) {}

        int m_KeyCode;
    };

    class OIL_API KeyPressedEvent{
    public:
        KeyPressedEvent(int keycode, int repeatCount)
            : KeyEvent(keycode), m_RepeatCount(repeatCount) {}

        inline int GetRepeatCount const {return m_RepeatCount; }

        std::string ToString() const override{
            std::stringstream ss;
            ss << "KeyPressedEvent: " << m_Keycode << " (" << m_RepeatCount << " repeats)";
            return ss.str();

        }

        EVENT_CLASS_TYPE(KeyPressed)

    private:
        int m_RepeatCount;


    };

    class OIL_API KeyReleasedEvent : public KeyEvent{
    public:
        KeyReleasedEvent(int keycode)
            : KeyEvent(keycode) {}

        std::string ToString() const override{
            std::stringstream ss;
            ss << "KeyReleased event: " << m_KeyCode;
        }

        EVENT_CLASS_TYPE(KeyReleased)
    }
}