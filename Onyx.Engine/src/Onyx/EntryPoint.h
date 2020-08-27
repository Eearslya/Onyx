#pragma once

#include "Onyx/Core.h"

extern Onyx::Application* Onyx::CreateApplication();

#ifdef ONYX_PLATFORM_WINDOWS
int main(int argc, char** argv) {
  Onyx::Log::Init();
  OnyxInfo("Initializing application...");

  Onyx::Application* app = Onyx::CreateApplication();
  app->Run();
  delete app;
}
#endif