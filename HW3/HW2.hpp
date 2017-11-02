#pragma once
#include <vector>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include "matrix.hpp"

class GLWindow {
public:
  GLWindow(const char* title = nullptr, int width = 640, int height = 480);

  bool bad() { return !window; };

  virtual void onKeyInput(int key, int action) = 0;

  // Set the viewport to cover the new window
  virtual void onResize(int width, int height) {
    glViewport(0, 0, width, height);
  }

  operator GLFWwindow*() { return window; }
  /* Make the window's context current */
  //void takeContext() { glfwMakeContextCurrent(window); }

  //bool shouldClose() { return glfwWindowShouldClose(window); }

public:
  GLFWwindow* window;
};

template <typename T>
using grid = std::vector<std::vector<T>>;
using vertices = grid<matrix<float>>;

class HW2Window : public GLWindow {
public:
  HW2Window();

  virtual void onResize(int width, int height);
  virtual void onKeyInput(int key, int action);
  void drawTorus();
  void drawTorusFace();

protected:
  static vertices torus;
  static vertices centers;
  static vertices normals;
  static grid<bool> gridRB;
  int sweepZ;
  int sweepY;
};

void drawBackground();