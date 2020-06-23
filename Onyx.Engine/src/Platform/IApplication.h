#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Onyx {
class ONYX_API IApplication {
 public:
  virtual void Run() = 0;
};

class ONYX_API Application final {
 public:
  static IApplication* CreateApplication(const wchar_t* applicationName);

 private:
  Application() {}
  ~Application() {}
};
}  // namespace Onyx