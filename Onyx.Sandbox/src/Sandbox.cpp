#include <Onyx.h>

int main(int argc, char** argv) {
  Onyx::IApplication* application = Onyx::Application::CreateApplication(L"Onyx");
  application->Run();
  Onyx::Application::DestroyApplication(application);

  return 0;
}