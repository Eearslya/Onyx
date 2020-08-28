#include "pch.h"

#include "Event.h"

namespace Onyx {
std::unordered_map<EventType, std::list<EventBus::Callback>> EventBus::s_Listeners;
}