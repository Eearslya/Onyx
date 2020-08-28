#pragma once

#include "Onyx/Events/Event.h"

namespace Onyx {
class ONYX_API KeyPressedEvent : public Event {
 public:
  KeyPressedEvent(unsigned int keyCode) : KeyCode(keyCode) {}

  unsigned int KeyCode;

  EVENT_CLASS_TYPE(KeyPressed);
};

class ONYX_API KeyReleasedEvent : public Event {
 public:
  KeyReleasedEvent(unsigned int keyCode) : KeyCode(keyCode) {}

  unsigned int KeyCode;

  EVENT_CLASS_TYPE(KeyReleased);
};
}  // namespace Onyx