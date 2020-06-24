#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Onyx {
namespace Platform {
class IApplication;
}

//! The main class for the Onyx game engine.
class ONYX_API Engine final {
 public:
  //! Construct a new Engine.
  /*!
    \param application The application this Engine belongs to.
  */
  Engine(Platform::IApplication* application);
  ~Engine();

  //! Begin running our engine.
  void Run();

  //! Handle a single iteration of our engine loop.
  /*!
    \return true upon successful loop, false if an error occurred.
  */
  const bool OnLoop();

  //! Get our parent application.
  /*!
    \return The IApplication instance we were created with.
  */
  Platform::IApplication* GetApplication() { return _application; }

 private:
  Platform::IApplication* _application = nullptr;
};
}  // namespace Onyx