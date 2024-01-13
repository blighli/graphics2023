#include "../header/tool.h"
#include <fstream>
#include <iostream>

namespace app {

auto readSpvFile(const std::string &filename)
    -> std::string {
  // 需要二进制读取
  std::ifstream file(
      filename, std::ios::binary | std::ios::ate);

  if (!file.is_open()) {
    std::cerr << "read " << filename << " failed!!!"
              << '\n';
    return std::string{};
  }

  auto size = file.tellg();
  std::string content;
  content.resize(size);
  file.seekg(0);

  file.read(content.data(), content.size());

  return content;
}
} // namespace app
