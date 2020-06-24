#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Onyx {
class Engine;
class IRendererBackend;

//! Main rendering API to be used by the engine and application.
class Renderer final {
 public:
  Renderer() = delete;
  ~Renderer() = delete;

  //! Initialize our rendering API, setting up the backend for drawing.
  /*!
    \param engine The engine this Renderer should associate with.
    \return true upon successful initialization, false otherwise.
  */
  static const bool Initialize(Engine* engine);

  //! Shutdown our rendering API, cleaning up all used data.
  static void Shutdown();

  //! Process a single frame and draw it to the screen.
  /*!
    \return true if frame was drawn successfully, false otherwise.
  */
  static const bool Frame();

 private:
  //! Our engine instance.
  static Engine* _engine;

  //! Our rendering API back-end.
  static IRendererBackend* _backend;
};
}  // namespace Onyx