#pragma once

#include <functional>
#include <list>
#include <unordered_map>

#include "Onyx/Core.h"

namespace Onyx {
enum class EventType {
  None = 0,
  WindowClosed,
  WindowResized,
  KeyPressed,
  KeyReleased,
  MousePressed,
  MouseReleased,
  MouseMoved,
  MouseScrolled
};

#define EVENT_CLASS_TYPE(type)                                        \
  static EventType GetStaticType() { return EventType::##type; }      \
  EventType GetEventType() const override { return GetStaticType(); } \
  const char* GetName() const override { return #type; }

class ONYX_API Event {
 public:
  virtual EventType GetEventType() const = 0;
  virtual const char* GetName() const = 0;
};

class EventBus {
  using Callback = std::function<bool(const Event&)>;

 public:
  template <typename T>
  static void Listen(Callback callback) {
    EventType type = T::GetStaticType();
    s_Listeners[type].push_back(callback);
  }

  template <typename T>
  static void Dispatch(const T& e) {
    EventType type = T::GetStaticType();
    for (auto& func : s_Listeners[type]) {
      if (func(e)) {
        break;
      }
    }
  }

 private:
  static std::unordered_map<EventType, std::list<Callback>> s_Listeners;
};
}  // namespace Onyx