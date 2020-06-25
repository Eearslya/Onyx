#include "pch.h"
#include "FileUtils.h"

namespace Onyx {
std::vector<char> FileUtils::ReadBinaryFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);
  ASSERT_MSG(file.is_open(), "Could not read from file!");
  // TODO: Error handling.
  size_t fileSize = file.tellg();
  std::vector<char> buffer(fileSize);
  file.seekg(0, std::ios::beg);
  file.read(buffer.data(), fileSize);
  file.close();
  return buffer;
}
}  // namespace Onyx