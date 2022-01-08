/*
Gates, a simple logic circuit simulator written in C++
Copyright (C) 2022 DarthChungo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "pch.hpp"
#include "Engine/Application.hpp"
#include "Util/Logger.hpp"

namespace Gates {
  static void glfw_error_callback(int error, const char* description) {
    Logger::Die("GLFW error code " + std::to_string(error) + ": " + std::string(description));
  }

  void Application::Construct(const glm::uvec2& size,
                              const glm::uvec2& position,
                              const char*       name,
                              glm::vec4         clear_color) {
    pWindowSize = size;
    pWindowPos  = position;
    pWindowName = name;
    pClearColor = clear_color;

    pHasBeenConstructed = true;
  }

  void Application::Launch(bool background) {
    if (pHasBeenClosed) Logger::Die("Trying to launch an application which has already been closed");
    if (!pHasBeenConstructed) Logger::Die("Trying to launch an application without constructing it first");

    std::thread thread(&Application::pPlatformThread, this);

    if (background) {
      thread.detach();

    } else {
      thread.join();
    }
  }

  void Application::Close() { pWantsToClose = true; }

  void Application::EnsureClosed() {
    while (!pHasBeenClosed) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  const glm::uvec2& Application::WindowSize() const { return pWindowSize; }
  const glm::uvec2& Application::WindowPos() const { return pWindowPos; }
  const glm::vec2&  Application::MousePos() const { return pMouse.pos; }
  const float       Application::AspectRatio() const { return (float)pWindowSize.x / (float)pWindowSize.y; }

  glm::vec2 Application::MouseWheel() const { return pMouse.wheel; }
  void      Application::ResetMouseWheel() { pMouse.wheel = {.0f, .0f}; }
  bool      Application::MouseFocus() const { return pHasMouseFocus; }

  const Button& Application::MouseButton(Gates::MouseButton button) const { return pMouse.state[(uint8_t)button]; }
  const Button& Application::KeyboardKey(Gates::KeyboardKey key) const { return pKeyboard.state[(uint16_t)key]; }

  const Mouse&    Application::MouseState() const { return pMouse; }
  const Keyboard& Application::KeyboardState() const { return pKeyboard; }

  float    Application::et() const { return pElapsedTime; }
  uint32_t Application::fps() const { return pFrameRate; }

  rcode Application::pOnUpdate() { return rcode::ok; }
  rcode Application::pOnLaunch() { return rcode::ok; }
  rcode Application::pOnImGuiRender() { return rcode::ok; }
  rcode Application::pOnRender() { return rcode::ok; }
  rcode Application::pOnClose() { return rcode::ok; }

  void Application::pHandleFrameBufferResize(GLFWwindow* window, int width, int height) {
    Application* app_instance      = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app_instance->pFrameBufferSize = glm::uvec2 {width, height};
    glViewport(0, 0, width, height);
  }

  void Application::pHandleWindowResize(GLFWwindow* window, int width, int height) {
    Application* app_instance = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app_instance->pWindowSize = glm::uvec2 {width, height};
  }

  void Application::pHandleWindowMove(GLFWwindow* window, int posx, int posy) {
    Application* app_instance = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app_instance->pWindowPos  = glm::uvec2 {posx, posy};
  }

  void Application::pHandleMouseMove(GLFWwindow* window, double posx, double posy) {
    Application* app_instance = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    auto         pos          = glm::vec2(posx, posy);

    if (!((pos.x > app_instance->pWindowSize.x) || (pos.y > app_instance->pWindowSize.y))) {
      app_instance->pMouse.pos =
          (pos / (glm::vec2)app_instance->pWindowSize) * glm::vec2(2.f, 2.f) - glm::vec2(1.f, 1.f);
    }
  }

  void Application::pHandleMouseButton(GLFWwindow* window, int button, int action, int mods) {
    Application* app_instance = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

    switch (action) {
      case GLFW_RELEASE:
        app_instance->pMouse.state_new[button] = false;
        break;

      case GLFW_PRESS:
        app_instance->pMouse.state_new[button] = true;
        break;

      default:
        break;
    }
  }

  void Application::pHandleMouseScroll(GLFWwindow* window, double deltax, double deltay) {
    Application* app_instance = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app_instance->pMouse.wheel += glm::vec2(deltax, deltay);
  }

  void Application::pHandleMouseEnter(GLFWwindow* window, int enter) {
    Application* app_instance    = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app_instance->pHasMouseFocus = enter;
  }

  void Application::pHandleKeyboardKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application* app_instance = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    if (key == -1) key = 0;

    switch (action) {
      case GLFW_RELEASE:
        app_instance->pKeyboard.state_new[key] = false;
        break;

      case GLFW_PRESS:
        app_instance->pKeyboard.state_new[key] = true;
        break;

      default:
        break;
    }
  }

  void Application::pPlatformThread() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) Logger::Die("GLFW initialization failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    pWindow = glfwCreateWindow(pWindowSize.x, pWindowSize.y, "Gates", NULL, NULL);
    if (!pWindow) Logger::Die("GLFW window creation failed");

    glfwSetFramebufferSizeCallback(pWindow, pHandleFrameBufferResize);
    glfwSetWindowSizeCallback(pWindow, pHandleWindowResize);
    glfwSetWindowPosCallback(pWindow, pHandleWindowMove);
    glfwSetCursorPosCallback(pWindow, pHandleMouseMove);
    glfwSetMouseButtonCallback(pWindow, pHandleMouseButton);
    glfwSetScrollCallback(pWindow, pHandleMouseScroll);
    glfwSetCursorEnterCallback(pWindow, pHandleMouseEnter);
    glfwSetKeyCallback(pWindow, pHandleKeyboardKey);

    glfwSetWindowPos(pWindow, pWindowPos.x, pWindowPos.y);
    glfwSetWindowUserPointer(pWindow, (void*)this);

    std::thread thread(&Application::pGraphicsThread, this);
    thread.detach();

    while (!pHasBeenClosed) {
      if (glfwWindowShouldClose(pWindow)) pWantsToClose = true;
      glfwWaitEvents();
    }

    glfwDestroyWindow(pWindow);
    glfwTerminate();
  }

  void Application::pGraphicsThread() {
    glfwMakeContextCurrent(pWindow);
    glfwSwapInterval(true);

    glewExperimental = GL_TRUE;
    if (!!glewInit()) Logger::Die("GLEW initialization failed");

    std::cout << "[INFO] Using " << glGetString(GL_RENDERER) << " driver with opengl " << glGetString(GL_VERSION)
              << "\n";

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(pWindow, true);
    ImGui_ImplOpenGL3_Init(NULL);

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    pClock1 = std::chrono::system_clock::now();
    pClock2 = std::chrono::system_clock::now();

    pThreadRunning = pOnLaunch() == rcode::ok ? true : false;

    while (pThreadRunning) {
      while (pThreadRunning) {
        pClock2       = std::chrono::system_clock::now();
        pElapsedTimer = pClock2 - pClock1;
        pClock1       = pClock2;
        pElapsedTime  = pElapsedTimer.count();

        pFrameTimer += pElapsedTime;
        pFrameCount++;

        if (pFrameTimer >= 1.0f) {
          pFrameRate = pFrameCount;
          pFrameTimer -= 1.0f;

          pWindowTittle = pWindowName + " - FPS: " + std::to_string(pFrameRate);
          glfwSetWindowTitle(pWindow, pWindowTittle.c_str());

          pFrameCount = 0;
        }

        for (uint32_t i = 0; i < 8; i++) {
          pMouse.state[i].pressed  = false;
          pMouse.state[i].released = false;

          if (pMouse.state_new[i] != pMouse.state_old[i]) {
            if (pMouse.state_new[i]) {
              pMouse.state[i].pressed = !pMouse.state[i].held;
              pMouse.state[i].held    = true;

            } else {
              pMouse.state[i].released = true;
              pMouse.state[i].held     = false;
            }
          }

          pMouse.state_old[i] = pMouse.state_new[i];
        }

        for (uint32_t i = 0; i < 512; i++) {
          pKeyboard.state[i].pressed  = false;
          pKeyboard.state[i].released = false;

          if (pKeyboard.state_new[i] != pKeyboard.state_old[i]) {
            if (pKeyboard.state_new[i]) {
              pKeyboard.state[i].pressed = !pKeyboard.state[i].held;
              pKeyboard.state[i].held    = true;

            } else {
              pKeyboard.state[i].released = true;
              pKeyboard.state[i].held     = false;
            }
          }

          pKeyboard.state_old[i] = pKeyboard.state_new[i];
        }

        if (pOnUpdate() != rcode::ok) pThreadRunning = false;

        glViewport(0, 0, pFrameBufferSize.x, pFrameBufferSize.y);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (pOnImGuiRender() != rcode::ok) pThreadRunning = false;

        ImGui::Render();

        glClearColor(
            pClearColor.x * pClearColor.w, pClearColor.y * pClearColor.w, pClearColor.z * pClearColor.w, pClearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);

        if (pOnRender() != rcode::ok) pThreadRunning = false;

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(pWindow);

        if (pWantsToClose) {
          pThreadRunning = false;
          pWantsToClose  = false;
        }
      }

      if (pOnClose() != rcode::ok) pThreadRunning = true;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    pHasBeenClosed = true;
  }
}
