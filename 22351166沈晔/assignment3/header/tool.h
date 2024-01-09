#pragma once
#include <fstream>
#include <iostream>

namespace app {
auto readSpvFile(const std::string &filename)
    -> std::string;
}
