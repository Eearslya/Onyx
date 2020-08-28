#pragma once

#include "Onyx/Events/Event.h"

namespace Onyx {
class ONYX_API WindowClosedEvent : public Event {
 public:
  EVENT_CLASS_TYPE(WindowClosed);
};

class ONYX_API WindowResizedEvent : public Event {
 public:
  WindowResizedEvent(unsigned int width, unsigned int height) : Width(width), Height(height) {}

  unsigned int Width;
  unsigned int Height;

  EVENT_CLASS_TYPE(WindowResized);
};
}  // namespace Onyx