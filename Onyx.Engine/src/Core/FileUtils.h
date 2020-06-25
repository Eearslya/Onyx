#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <fstream>
#include <vector>

namespace Onyx {
//! Helper class for dealing with local filesystems.
class FileUtils final {
 public:
  //! Read a binary file into an array.
  /*!
    \param filename The path to the file.
    \return The contents of the file, if found.
  */
  static std::vector<char> ReadBinaryFile(const std::string& filename);
};
}  // namespace Onyx