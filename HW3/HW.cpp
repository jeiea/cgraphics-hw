#include "HW.hpp"

static void resizeHandler(GLFWwindow* window, int width, int height) {
  auto win = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));
  win->onResize(width, height);
}

static void keyHandler(GLFWwindow* window, int key, int scancode, int action, int mods) {
  auto win = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));
  win->onKeyInput(key, action);
}

GLWindow::GLWindow(const char* title, int width, int height) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  /* Create a windowed mode window and its OpenGL context */
  auto caption = title ? title : "OpenGL Window";
  window = glfwCreateWindow(640, 480, caption, nullptr, nullptr);
  if (!window) return;

  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, resizeHandler);
  glfwSetKeyCallback(window, keyHandler);

  glfwMakeContextCurrent(*this);
}

void HWWindow::onKeyInput(int key, int action) {
  if (GLFW_KEY_F1 <= key && key <= GLFW_KEY_F12) {
    exitMessage = key - GLFW_KEY_F1 + 1;
    glfwSetWindowShouldClose(window, 1);
  }
}

void HWWindow::onResize(int width, int height) {
  GLWindow::onResize(width, height);

  if (height == 0) height = 1;   // To prevent divide by 0
  GLfloat aspect = (GLfloat)width / (GLfloat)height;

  // Set the aspect ratio of the clipping volume to match the viewport
  glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
  glLoadIdentity();             // Reset

  // Enable perspective projection
  double distance = 8;
  //double zNear = distance + 4, zFar = zNear + 8;
  gluPerspective(45.0, aspect, distance, distance * 2);
  gluLookAt(distance, distance, distance, 0, 0, 0, -1, 1, -1);
}
