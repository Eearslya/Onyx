#include "Utilities.h"

#include <fstream>

namespace Onyx {
std::vector<char> Utilities::ReadBinaryFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);
  ASSERT_MSG(file.is_open(), "Failed to open file for reading!");

  size_t fileSize = file.tellg();
  std::vector<char> buffer(fileSize);
  file.seekg(0, std::ios::beg);
  file.read(buffer.data(), fileSize);

  file.close();
  return buffer;
}
}  // namespace Onyx
