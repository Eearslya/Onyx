#include <Onyx.h>

int main(int argc, char** argv) {
  if (Onyx::Application::Initialize(L"Onyx")) {
    Onyx::Application::Run();
    Onyx::Application::Shutdown();
  } else {
    Onyx::Logger::Fatal("Could not initialize application!");
    return 1;
  }

  return 0;
}