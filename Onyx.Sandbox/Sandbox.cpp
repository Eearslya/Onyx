#include <Onyx.h>

int main() {
  Onyx::IApplication* application =
      Onyx::Application::CreateApplication(L"Onyx");
  application->Run();
  return 0;
}