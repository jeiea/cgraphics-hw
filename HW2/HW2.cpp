#include <iostream>
#include <functional>
#if _WIN32 || _WIN64
#include <Windows.h>
#include <tchar.h>
int main();
int APIENTRY _tWinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPTSTR    lpCmdLine,
  int       nCmdShow)
{
  main();
}
#endif

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include "matrix.hpp"

using namespace std;

const float PI = 3.141592653589793238462643383279502884f;

matrix<float> scale(float mag) {
  return matrix<float> {
    { mag, 0 },
    { 0, mag },
  };
}

matrix<float> rotate2x2(float rad) {
  float c = cos(rad);
  float s = sin(rad);
  return matrix<float> {
    { c, -s },
    { s, +c },
  };
}

matrix<float> rotateY(float radian) {
  return {
    {cos(radian), 0, sin(radian)},
    {0, 1, 0},
    {-sin(radian), 0, cos(radian)}
  };
}

class GLWindow {
public:
  GLWindow(const char* title = nullptr, int width = 640, int height = 480);

  bool bad() { return !window; };

  virtual void onKeyInput(int key, int action) = 0;

  // Set the viewport to cover the new window
  virtual void onResize(int width, int height) {
    glViewport(0, 0, width, height);
  }

  /* Make the window's context current */
  void takeContext() { glfwMakeContextCurrent(window); }

  bool shouldClose() { return glfwWindowShouldClose(window); }

public:
  GLFWwindow* window;
};

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

  takeContext();
}

using torus = vector<vector<matrix<float>>>;

class HW2Window : public GLWindow {
public:
  HW2Window() : GLWindow("HW2 - Drawing Torus", 640, 480) {
    onResize(640, 480);
  }

  virtual void onResize(int width, int height);
  virtual void onKeyInput(int key, int action);
  void drawTorus();

protected:
  static torus torus;
  int sweepZ = 18;
  int sweepY = 36;
};

void HW2Window::onResize(int width, int height) {
  GLWindow::onResize(width, height);

  if (height == 0) height = 1;   // To prevent divide by 0
  GLfloat aspect = (GLfloat)width / (GLfloat)height;

  // Set the aspect ratio of the clipping volume to match the viewport
  glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
  glLoadIdentity();             // Reset

  // Enable perspective projection
  double distance = 10;
  gluPerspective(45.0, aspect, 0.1, distance * 10);
  gluLookAt(distance, distance, distance, 0, 0, 0, -1, 1, -1);
}

void HW2Window::onKeyInput(int key, int action)
{
  if (action != GLFW_RELEASE) return;

  switch (key) {
  case GLFW_KEY_A:
    sweepY = min(36, sweepY + 1);
    break;
  case GLFW_KEY_S:
    sweepY = max(0, sweepY - 1);
    break;
  case GLFW_KEY_J:
    sweepZ = min(18, sweepZ + 1);
    break;
  case GLFW_KEY_K:
    sweepZ = max(0, sweepZ - 1);
    break;
  }
}

torus PrepareTorus() {
  const int angleZ = 18;
  const int angleY = 36;

  auto rotSmall = rotate2x2(2 * PI / angleZ);
  vector<matrix<float>> sRing;
  sRing.emplace_back(matrix<float>{ {0}, { 1 }});
  cout << fixed;
  for (int i = 1; i < angleZ; i++) {
    sRing.emplace_back(rotSmall * *sRing.rbegin());
  }
  //for (auto& v : sRing)
  //  cout << '[' << v[0][0] << ", " << v[1][0] << ']' << endl;
  //cin.get();
  transform(sRing.begin(), sRing.end(), sRing.begin(), [=](matrix<float>& mat) {
    return matrix<float> { {mat[0][0] + 2.f}, { mat[1][0] + 2.f }, { 0 }};
  });

  torus bRing;
  bRing.push_back(sRing);
  auto rotBig = rotateY(2 * PI / angleY);
  for (int i = 1; i < angleY; i++) {
    auto beg = bRing.rbegin()->begin();
    auto end = bRing.rbegin()->end();
    vector<matrix<float>>&& next{};
    transform(beg, end, back_inserter(next), [&](matrix<float>& o) {
      return rotBig * o;
    });
    bRing.emplace_back(move(next));
  }

  return bRing;
}

torus HW2Window::torus = PrepareTorus();

void HW2Window::drawTorus() {
  int maxY = min(torus.size() - 1, sweepY);
  int maxZ = min(torus[0].size() - 1, sweepZ);
  auto glVertexMat = [](matrix<float>& v) {
    glVertex3f(v[0][0], v[1][0], v[2][0]);
  };

  glColor3f(0, 0, 1);
  for (int i = 0; i < maxY; i++) {
    glBegin(GL_QUAD_STRIP);
    auto& l = torus[i];
    auto& r = torus[i + 1];
    for (int j = 0; j < maxZ; j++) {
      glVertexMat(l[j]);
      glVertexMat(r[j]);
    }
    glEnd();
  }

  glColor3f(0, 0, 0);
  glBegin(GL_LINE_STRIP);
  for (int i = 0; i < maxY; i++) {
    for (int j = 0; j < maxZ; j++) {
      glVertexMat(torus[i + 0][j + 0]);
      glVertexMat(torus[i + 1][j + 0]);
      glVertexMat(torus[i + 1][j + 1]);
      glVertexMat(torus[i + 0][j + 1]);
      glVertexMat(torus[i + 0][j + 0]);
    }
  }

  if (sweepY >= maxY) {

  }
  glEnd();
}

void drawAxes() {
  glBegin(GL_LINES);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(100.0f, 0.0f, 0.0f);
  glColor3f(0.0f, 1.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 100.0f, 0.0f);
  glColor3f(0.0f, 1.0f, 1.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, 100.0f);
  glEnd();
}

void drawBackground() {
  /* Handler for window-repaint event.
  Called back when the window first appears and
  whenever the window needs to be re-painted. */
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  drawAxes();
}

int main() {
  /* Initialize the library */
  if (!glfwInit()) {
    cerr << "OpenGL 초기화에 실패했습니다." << endl;
    return -1;
  }

  HW2Window win;
  if (win.bad())
  {
    cerr << "OpenGL 초기화에 실패했습니다." << endl;
    glfwTerminate();
    return -1;
  }

  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  //std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

  glfwSwapInterval(1);

  /* Loop until the user closes the window */
  int count = 0;
  while (!win.shouldClose())
  {
    char buf[128];
    sprintf_s(buf, sizeof(buf), "Draw count: %d", ++count);
    cout << buf << endl;

    /* Render here */
    drawBackground();
    win.drawTorus();

    /* Swap front and back buffers */
    glfwSwapBuffers(win.window);

    /* Poll for and process events */
    glfwWaitEvents();
  }

  glfwTerminate();
  return 0;
}
