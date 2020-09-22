#include <iostream>
#include <filesystem>

int main(int, char**) {
  namespace fs = std::filesystem;

  for (const auto& file : fs::directory_iterator("./")) {
    std::cout << file.path() << ": " << file.file_size() << '\n';
  }

  return 0;
}
