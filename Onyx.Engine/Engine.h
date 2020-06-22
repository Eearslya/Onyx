#pragma once

#include "Defines.h"
#include "Types.h"

namespace Onyx {
class IApplication;

class ONYX_API Engine final {
 public:
  Engine(IApplication* application);
  ~Engine();

  void Run();
  const bool OnLoop(const F32 deltaTime);
  void Shutdown();

  IApplication* GetApplication() { return _application; }

 private:
  IApplication* _application;
};
}  // namespace Onyx