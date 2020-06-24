#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Onyx {
class ONYX_API IApplication {
 public:
  IApplication(const wchar_t* applicationName)
      : _applicationName(applicationName) {}

  virtual void Run() = 0;

  virtual void* GetHandle() = 0;

 protected:
  const wchar_t* _applicationName;
};

class ONYX_API Application final {
 public:
  static IApplication* CreateApplication(const wchar_t* applicationName);
  static void DestroyApplication(IApplication* app);
};
}  // namespace Onyx