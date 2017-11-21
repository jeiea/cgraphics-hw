#pragma once
#define GLFW_INCLUDE_GLU
#include <glfw3.h>

class GLWindow {
public:
  GLWindow(const char* title = nullptr, int width = 640, int height = 480);

  bool bad() { return !window; };

  virtual void onKeyInput(int key, int action) = 0;

  // Set the viewport to cover the new window
  virtual void onResize(int width, int height) {
    glViewport(0, 0, width, height);
  }

  virtual void onDraw() {};

  operator GLFWwindow*() { return window; }
  virtual ~GLWindow() { glfwDestroyWindow(window); }

protected:
  GLFWwindow* window;
};

class HWWindow : public GLWindow {
public:
  HWWindow(const char* title = nullptr, int width = 640, int height = 480)
    : GLWindow(title, width, height) {}

  virtual void onKeyInput(int key, int action);

  // Set the viewport to cover the new window
  virtual void onResize(int width, int height);

  // If user press function key, the number is recorded.
  int exitMessage = 0;
};

void drawBackground();
