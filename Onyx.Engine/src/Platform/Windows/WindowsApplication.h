#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <Windows.h>

#include "Platform/IApplication.h"

namespace Onyx {
class WindowsApplication final : public IApplication {
 public:
  WindowsApplication();
  ~WindowsApplication();

  void Run() override;
};
}  // namespace Onyx