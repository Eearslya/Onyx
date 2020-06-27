#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Onyx {
//! Generic RendererBackend interface, to be implemented by different graphics APIs.
class IRendererBackend {
 public:
  //! Construct our renderer backend, optionally with validation.
  /*!
    \param application The application this renderer should belong to.
    \param enableValidation Indicates whether to add additional debug/validation calls
                            to our renderer.
  */
  IRendererBackend(const bool enableValidation) {}
  virtual ~IRendererBackend() = default;

  //! Prepare to draw a frame.
  virtual const bool PrepareFrame() = 0;

  //! Draw a frame.
  virtual const bool Frame() = 0;

  //! Returns whether validation is enabled for this backend.
  /*!
    \return true if validation is enabled, false otherwise.
  */
  virtual const bool ValidationEnabled() const = 0;
};
}  // namespace Onyx