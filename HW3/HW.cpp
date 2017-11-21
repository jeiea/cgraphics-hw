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
  if (glfwGetKey(*this, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) return;
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
  gluPerspective(45.0, aspect, distance, distance * 6);
  gluLookAt(distance, distance, distance, 0, 0, 0, -1, 1, -1);
}

void drawAxes() {
  glBegin(GL_LINES);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(100.0f, 0.0f, 0.0f);
  glColor3f(0.0f, 1.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 100.0f, 0.0f);
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, 100.0f);
  glEnd();
}

void drawBackground() {
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  drawAxes();
}
