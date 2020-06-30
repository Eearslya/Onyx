#pragma once

#include "Defines.h"
#include "Types.h"

#include <string>
#include <vector>

namespace Onyx {
class Utilities final {
 public:
  static std::vector<char> ReadBinaryFile(const std::string& filename);
};
}  // namespace Onyx