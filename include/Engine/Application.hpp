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

#ifndef GATES_APPLICATION_HPP
#define GATES_APPLICATION_HPP

#include "pch.hpp"

namespace std {
  template <typename T>
  inline std::string to_string(const glm::vec<2, T>& vector) {
    return std::to_string(vector.x) + "," + std::to_string(vector.y);
  }
}

namespace Gates {
  enum class rcode : uint8_t {
    ok    = 0,
    quit  = 1,
    error = 2,
  };

  struct Button {
    bool pressed  = false;
    bool held     = false;
    bool released = false;
  };

  enum class KeyboardKey : uint16_t {
    KEY_UNKNOWN       = 0,
    KEY_SPACE         = 32,
    KEY_APOSTROPHE    = 39,
    KEY_COMMA         = 44,
    KEY_MINUS         = 45,
    KEY_PERIOD        = 46,
    KEY_SLASH         = 47,
    KEY_0             = 48,
    KEY_1             = 49,
    KEY_2             = 50,
    KEY_3             = 51,
    KEY_4             = 52,
    KEY_5             = 53,
    KEY_6             = 54,
    KEY_7             = 55,
    KEY_8             = 56,
    KEY_9             = 57,
    KEY_SEMICOLON     = 59,
    KEY_EQUAL         = 61,
    KEY_A             = 65,
    KEY_B             = 66,
    KEY_C             = 67,
    KEY_D             = 68,
    KEY_E             = 69,
    KEY_F             = 70,
    KEY_G             = 71,
    KEY_H             = 72,
    KEY_I             = 73,
    KEY_J             = 74,
    KEY_K             = 75,
    KEY_L             = 76,
    KEY_M             = 77,
    KEY_N             = 78,
    KEY_O             = 79,
    KEY_P             = 80,
    KEY_Q             = 81,
    KEY_R             = 82,
    KEY_S             = 83,
    KEY_T             = 84,
    KEY_U             = 85,
    KEY_V             = 86,
    KEY_W             = 87,
    KEY_X             = 88,
    KEY_Y             = 89,
    KEY_Z             = 90,
    KEY_LEFT_BRACKET  = 91,
    KEY_BACKSLASH     = 92,
    KEY_RIGHT_BRACKET = 93,
    KEY_GRAVE_ACCENT  = 96,
    KEY_WORLD_1       = 161,
    KEY_WORLD_2       = 162,
    KEY_ESCAPE        = 256,
    KEY_ENTER         = 257,
    KEY_TAB           = 258,
    KEY_BACKSPACE     = 259,
    KEY_INSERT        = 260,
    KEY_DELETE        = 261,
    KEY_RIGHT         = 262,
    KEY_LEFT          = 263,
    KEY_DOWN          = 264,
    KEY_UP            = 265,
    KEY_PAGE_UP       = 266,
    KEY_PAGE_DOWN     = 267,
    KEY_HOME          = 268,
    KEY_END           = 269,
    KEY_CAPS_LOCK     = 280,
    KEY_SCROLL_LOCK   = 281,
    KEY_NUM_LOCK      = 282,
    KEY_PRINT_SCREEN  = 283,
    KEY_PAUSE         = 284,
    KEY_F1            = 290,
    KEY_F2            = 291,
    KEY_F3            = 292,
    KEY_F4            = 293,
    KEY_F5            = 294,
    KEY_F6            = 295,
    KEY_F7            = 296,
    KEY_F8            = 297,
    KEY_F9            = 298,
    KEY_F10           = 299,
    KEY_F11           = 300,
    KEY_F12           = 301,
    KEY_F13           = 302,
    KEY_F14           = 303,
    KEY_F15           = 304,
    KEY_F16           = 305,
    KEY_F17           = 306,
    KEY_F18           = 307,
    KEY_F19           = 308,
    KEY_F20           = 309,
    KEY_F21           = 310,
    KEY_F22           = 311,
    KEY_F23           = 312,
    KEY_F24           = 313,
    KEY_F25           = 314,
    KEY_KP_0          = 320,
    KEY_KP_1          = 321,
    KEY_KP_2          = 322,
    KEY_KP_3          = 323,
    KEY_KP_4          = 324,
    KEY_KP_5          = 325,
    KEY_KP_6          = 326,
    KEY_KP_7          = 327,
    KEY_KP_8          = 328,
    KEY_KP_9          = 329,
    KEY_KP_DECIMAL    = 330,
    KEY_KP_DIVIDE     = 331,
    KEY_KP_MULTIPLY   = 332,
    KEY_KP_SUBTRACT   = 333,
    KEY_KP_ADD        = 334,
    KEY_KP_ENTER      = 335,
    KEY_KP_EQUAL      = 336,
    KEY_LEFT_SHIFT    = 340,
    KEY_LEFT_CONTROL  = 341,
    KEY_LEFT_ALT      = 342,
    KEY_LEFT_SUPER    = 343,
    KEY_RIGHT_SHIFT   = 344,
    KEY_RIGHT_CONTROL = 345,
    KEY_RIGHT_ALT     = 346,
    KEY_RIGHT_SUPER   = 347,
    KEY_MENU          = 348
  };

  enum class MouseButton : uint8_t {
    BUTTON_1 = 0,
    BUTTON_2 = 1,
    BUTTON_3 = 2,
    BUTTON_4 = 3,
    BUTTON_5 = 4,
    BUTTON_6 = 5,
    BUTTON_7 = 6,
    BUTTON_8 = 7
  };

  class Mouse {
   public:
    friend class Application;

    glm::vec2 pos   = {};
    glm::vec2 wheel = {};

    inline const Button& button(const MouseButton& button) const { return state[(uint8_t)button]; }

   private:
    Button state[8]     = {};
    bool   state_old[8] = {};
    bool   state_new[8] = {};
  };

  class Keyboard {
   public:
    friend class Application;

    inline const Button& key(const KeyboardKey& key) const { return state[(uint8_t)key]; }

   private:
    Button state[512]     = {};
    bool   state_old[512] = {};
    bool   state_new[512] = {};
  };

  class Application {
   public:
    void Construct(const glm::uvec2& size,
                   const glm::uvec2& position,
                   const char*       name,
                   glm::vec4         clear_color = glm::vec4(.0f, .0f, .0f, 1.00f));
    void Launch(bool background = false);
    void Close();
    void EnsureClosed();

   private:
    static void pHandleFrameBufferResize(GLFWwindow* window, int width, int height);
    static void pHandleWindowResize(GLFWwindow* window, int width, int height);
    static void pHandleWindowMove(GLFWwindow* window, int posx, int posy);
    static void pHandleMouseMove(GLFWwindow* window, double posx, double posy);
    static void pHandleMouseButton(GLFWwindow* window, int button, int action, int mods);
    static void pHandleMouseScroll(GLFWwindow* window, double deltax, double deltay);
    static void pHandleMouseEnter(GLFWwindow* window, int enter);
    static void pHandleKeyboardKey(GLFWwindow* window, int key, int scancode, int action, int mods);

    void pPlatformThread();
    void pGraphicsThread();

   protected:
    virtual rcode pOnUpdate();
    virtual rcode pOnLaunch();
    virtual rcode pOnImGuiRender();
    virtual rcode pOnRender();
    virtual rcode pOnClose();

   public:
    const glm::uvec2& ScreenSize() const;
    const glm::uvec2& WindowSize() const;
    const glm::uvec2& WindowPos() const;
    const float       AspectRatio() const;

    const glm::vec2& MousePos() const;
    glm::vec2        MouseWheel() const;
    void             ResetMouseWheel();
    bool             MouseFocus() const;

    const Button& MouseButton(Gates::MouseButton button) const;
    const Button& KeyboardKey(Gates::KeyboardKey key) const;

    const Mouse&    MouseState() const;
    const Keyboard& KeyboardState() const;

    float    et() const;
    uint32_t fps() const;

   private:
    std::string pWindowTittle;

    bool pHasBeenClosed      = false;
    bool pHasBeenConstructed = false;

    std::atomic<bool> pThreadRunning {false};
    std::atomic<bool> pWantsToClose {false};

    std::chrono::system_clock::time_point pClock1;
    std::chrono::system_clock::time_point pClock2;
    std::chrono::duration<float>          pElapsedTimer;

    GLFWwindow* pWindow     = nullptr;
    std::string pWindowName = "Application";

    glm::vec4 pClearColor = {.0f, .0f, .0f, 1.f};

    float    pElapsedTime = 0.0f;
    float    pFrameTimer  = 0.0f;
    uint32_t pFrameCount  = 0;
    uint32_t pFrameRate   = 0;

    glm::uvec2 pFrameBufferSize = {};
    glm::uvec2 pWindowSize      = {};
    glm::uvec2 pWindowPos       = {};

    Mouse    pMouse    = {};
    Keyboard pKeyboard = {};

    bool pHasMouseFocus = false;
  };
}

#endif
