#pragma once

#include "Onyx/Events/Event.h"

namespace Onyx {
class ONYX_API MousePressedEvent : public Event {
 public:
  MousePressedEvent(unsigned int button) : Button(button) {}

  unsigned int Button;

  EVENT_CLASS_TYPE(MousePressed);
};

class ONYX_API MouseReleasedEvent : public Event {
 public:
  MouseReleasedEvent(unsigned int button) : Button(button) {}

  unsigned int Button;

  EVENT_CLASS_TYPE(MouseReleased);
};

class ONYX_API MouseMovedEvent : public Event {
 public:
  MouseMovedEvent(double x, double y) : X(x), Y(y) {}

  double X;
  double Y;

  EVENT_CLASS_TYPE(MouseMoved);
};

class ONYX_API MouseScrolledEvent : public Event {
 public:
  MouseScrolledEvent(double offset) : Offset(offset) {}

  double Offset;

  EVENT_CLASS_TYPE(MouseScrolled);
};
}  // namespace Onyx