#include "Application.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Logger.h"
#include "Mesh.h"
#include "Renderer.h"

namespace Onyx {
HINSTANCE Application::s_Instance;
HWND Application::s_Window;
bool Application::s_CloseRequested = false;

static const U32 g_WindowWidth = 1280;
static const U32 g_WindowHeight = 720;
static const wchar_t* g_WindowClassName = L"OnyxWindow";

static Mesh squareMesh;
static Mesh objectMesh;

LRESULT CALLBACK ApplicationWindowProcedure(HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_ERASEBKGND:
      return 1;
    case WM_CLOSE:
      Application::RequestClose();
      return 0;
    case WM_DESTROY:
      Logger::Fatal("Application window destroyed. Shutting down...");
      PostQuitMessage(0);
      return 0;
  }

  return DefWindowProcW(hwnd, msg, wParam, lParam);
}

const bool Application::Initialize() {
  Logger::Info("Initializing Onyx...");
  WNDCLASSW wc{};
  wc.lpfnWndProc = ApplicationWindowProcedure;
  wc.hInstance = s_Instance;
  wc.lpszClassName = g_WindowClassName;
  RegisterClassW(&wc);

  U32 windowStyle =
      WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
  U32 windowExStyle = WS_EX_APPWINDOW;

  U32 windowW = g_WindowWidth;
  U32 windowH = g_WindowHeight;
  U32 windowX = 100;
  U32 windowY = 100;

  RECT borderRect = {0, 0, 0, 0};
  AdjustWindowRectEx(&borderRect, windowStyle, false, windowExStyle);
  windowX += borderRect.left;
  windowY += borderRect.top;
  windowW += borderRect.right - borderRect.left;
  windowH += borderRect.bottom - borderRect.top;

  Logger::Trace("Creating %dx%d application window. (Actual size: %dx%d)", g_WindowWidth,
                g_WindowHeight, windowW, windowH);

  s_Window = CreateWindowExW(windowExStyle, g_WindowClassName, L"Onyx", windowStyle, windowX,
                             windowY, windowW, windowH, nullptr, nullptr, s_Instance, nullptr);
  if (!s_Window) {
    Logger::Fatal("Failed to create application window!");
    ASSERT(false);
  }

  ShowWindow(s_Window, SW_SHOW);

  if (!Renderer::Initialize()) {
    Logger::Fatal("Failed to initialize application renderer!");
    ASSERT(false);
  }

  squareMesh.Create({{{-0.5f, -0.5f, 0.0f}, {}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                     {{0.5f, -0.5f, 0.0f}, {}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                     {{0.5f, 0.5f, 0.0f}, {}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
                     {{-0.5f, 0.5f, 0.0f}, {}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}},
                    {0, 1, 2, 2, 3, 0});
  Renderer::UploadMesh(squareMesh);

#if 0
  std::string objectFile = "assets/models/cube.obj";
  tinyobj::attrib_t objectAttribs;
  std::vector<tinyobj::shape_t> objectShapes;
  std::vector<tinyobj::material_t> objectMaterials;
  std::string objectWarning;
  std::string objectError;

  Logger::Debug("Loading object \"%s\"", objectFile.c_str());

  bool objectLoaded = tinyobj::LoadObj(&objectAttribs, &objectShapes, &objectMaterials,
                                       &objectWarning, &objectError, objectFile.c_str());
  if (!objectWarning.empty()) {
    Logger::Warn("TinyObjLoader: %s", objectWarning.c_str());
  }
  if (!objectError.empty()) {
    Logger::Error("TinyObjLoader: %s", objectError.c_str());
  }

  Logger::Debug("Loaded object \"%s\"", objectFile.c_str());
  Logger::Debug(" - Vertex Count: %d", objectAttribs.vertices.size() / 3);
  Logger::Debug(" - Normal Count: %d", objectAttribs.normals.size() / 3);
  Logger::Debug(" - UV Count:     %d", objectAttribs.texcoords.size() / 2);
  Logger::Debug(" - Shape Count: %d", objectShapes.size());
  Logger::Debug("   - Shape 0: Primitive Count: %d", objectShapes[0].mesh.num_face_vertices.size());
  Logger::Debug("   - Shape 0: Index Count: %d", objectShapes[0].mesh.indices.size());

  std::vector<Vertex> objectVertices(objectShapes[0].mesh.indices.size());
  std::vector<U16> objectIndices;
  for (U32 index = 0; index < objectShapes[0].mesh.indices.size(); index++) {
    tinyobj::index_t idx = objectShapes[0].mesh.indices[index];
    objectVertices[index].Position.x = objectAttribs.vertices[3 * idx.vertex_index + 0];
    objectVertices[index].Position.y = objectAttribs.vertices[3 * idx.vertex_index + 1];
    objectVertices[index].Position.z = objectAttribs.vertices[3 * idx.vertex_index + 2];
    if (idx.normal_index != -1) {
      objectVertices[index].Normal.x = objectAttribs.normals[3 * idx.normal_index + 0];
      objectVertices[index].Normal.y = objectAttribs.normals[3 * idx.normal_index + 1];
      objectVertices[index].Normal.z = objectAttribs.normals[3 * idx.normal_index + 2];
    }
    if (idx.texcoord_index != -1) {
      objectVertices[index].TexCoord.x = objectAttribs.texcoords[2 * idx.texcoord_index + 0];
      objectVertices[index].TexCoord.y = objectAttribs.texcoords[2 * idx.texcoord_index + 1];
    }

    if (idx.normal_index == -1) {
      objectVertices[index].Color = (glm::normalize(objectVertices[index].Position) * 0.5f) + 0.5f;
    } else {
      objectVertices[index].Color.r =
          (objectAttribs.normals[3 * idx.normal_index + 0] * 0.5f) + 0.5f;
      objectVertices[index].Color.g =
          (objectAttribs.normals[3 * idx.normal_index + 1] * 0.5f) + 0.5f;
      objectVertices[index].Color.b =
          (objectAttribs.normals[3 * idx.normal_index + 2] * 0.5f) + 0.5f;
    }
    objectIndices.push_back(static_cast<U16>(index));
  }

  /*
  U64 indexOffset = 0;
  for (U32 faceIndex = 0; faceIndex < objectShapes[0].mesh.num_face_vertices.size(); faceIndex++) {
    U32 fv = objectShapes[0].mesh.num_face_vertices[faceIndex];
    ASSERT_MSG(fv == 3, "Renderer needs object to be triangulated!");
    for (U32 faceVertexIndex = 0; faceVertexIndex < fv; faceVertexIndex++) {
      tinyobj::index_t idx = objectShapes[0].mesh.indices[indexOffset + faceVertexIndex];
      objectVertices[idx.vertex_index].Position.x =
          objectAttribs.vertices[3 * idx.vertex_index + 0];
      objectVertices[idx.vertex_index].Position.y =
          objectAttribs.vertices[3 * idx.vertex_index + 1];
      objectVertices[idx.vertex_index].Position.z =
          objectAttribs.vertices[3 * idx.vertex_index + 2];
      objectVertices[idx.vertex_index].Color.r = objectAttribs.normals[3 * idx.normal_index + 0];
      objectVertices[idx.vertex_index].Color.g = objectAttribs.normals[3 * idx.normal_index + 1];
      objectVertices[idx.vertex_index].Color.b = objectAttribs.normals[3 * idx.normal_index + 2];
      objectIndices.push_back(static_cast<U16>(idx.vertex_index));
    }
    indexOffset += fv;
  }
  */

  Logger::Debug("Uploading mesh with %d vertices and %d indices.", objectVertices.size(),
                objectIndices.size());
  objectMesh.Create(objectVertices, objectIndices);
  Renderer::UploadMesh(objectMesh);
#endif

  return true;
}

void Application::Shutdown() { Renderer::Shutdown(); }

void Application::Run() {
  Logger::Trace("Beginning main application loop.");
  while (!s_CloseRequested) {
    ProcessEvents();

    if (!Renderer::Frame()) {
      Logger::Error("Error during frame draw!");
    }
  }
  Logger::Trace("Main application loop ended.");
}

Extent2D Application::GetWindowExtent() {
  RECT windowRect;
  GetWindowRect(s_Window, &windowRect);
  return {static_cast<U32>(windowRect.right) - static_cast<U32>(windowRect.left),
          static_cast<U32>(windowRect.bottom) - static_cast<U32>(windowRect.top)};
}

void Application::ProcessEvents() {
  MSG message;
  while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&message);
    DispatchMessageW(&message);
  }
}

void Application::RequestClose() {
  Logger::Info("Program close requested.");
  s_CloseRequested = true;
}
}  // namespace Onyx