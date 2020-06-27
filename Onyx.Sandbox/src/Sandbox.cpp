#include <Onyx.h>

int main(int argc, char** argv) {
  Onyx::Application::Initialize(L"Onyx");
  Onyx::Application::Run();
  Onyx::Application::Shutdown();

  return 0;
}