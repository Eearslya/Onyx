#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Onyx {
//! Logging methods.
class ONYX_API Logger final {
 public:
  //! Log levels. Lower values are more "important".
  enum class Level { FATAL = 0, ERROR, WARN, INFO, DEBUG, TRACE };

  //! Set our logging level.
  /*!
    \param newLevel The new level to set to.
  */
  static void SetLevel(Level newLevel) { _logLevel = newLevel; }

  //! Log with a TRACE severity.
  /*!
    \param msg The message, formatted similarly to printf.
    \param ... The parameters to be inserted by printf.
  */
  static void Trace(const char* msg, ...);

  //! Log with a DEBUG severity.
  /*!
    \param msg The message, formatted similarly to printf.
    \param ... The parameters to be inserted by printf.
  */
  static void Debug(const char* msg, ...);

  //! Log with an INFO severity.
  /*!
    \param msg The message, formatted similarly to printf.
    \param ... The parameters to be inserted by printf.
  */
  static void Info(const char* msg, ...);

  //! Log with a WARN severity.
  /*!
    \param msg The message, formatted similarly to printf.
    \param ... The parameters to be inserted by printf.
  */
  static void Warn(const char* msg, ...);

  //! Log with an ERROR severity.
  /*!
    \param msg The message, formatted similarly to printf.
    \param ... The parameters to be inserted by printf.
  */
  static void Error(const char* msg, ...);

  //! Log with a FATAL severity.
  /*!
    \param msg The message, formatted similarly to printf.
    \param ... The parameters to be inserted by printf.
  */
  static void Fatal(const char* msg, ...);

 private:
  //! Internal method to write log messages to stdout.
  /*!
    \param level The level of the log mesage to be printed.
    \param msg The message format.
    \param args The extra arguments for formatting.
  */
  static void WriteLog(Level level, const char* msg, va_list args);

  //! Our current log level.
  static Level _logLevel;
};
}  // namespace Onyx