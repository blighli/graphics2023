#include "header/application.h"
#include <iostream>


void initRender(uint32_t width = 800, uint32_t height = 600) {
  app::Application::Init(width, height);
}

auto main() -> int {
  initRender();
  auto &app = app::Application::GetInstance();
  std::cout << "Prepare!"<< "\n";
  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
