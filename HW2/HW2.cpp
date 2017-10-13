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
  int sweepZ = torus[0].size() - 1;
  int sweepY = torus.size() - 1;
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
  gluPerspective(45.0, aspect, 0.1, distance * 2);
  gluLookAt(distance, distance, distance, 0, 0, 0, -1, 1, -1);
}

void HW2Window::onKeyInput(int key, int action)
{
  if (action == GLFW_RELEASE) return;

  switch (key) {
  case GLFW_KEY_A:
    sweepY = min(+static_cast<int>(torus.size()), sweepY + 1);
    break;
  case GLFW_KEY_S:
    sweepY = max(-static_cast<int>(torus.size()), sweepY - 1);
    break;
  case GLFW_KEY_J:
    sweepZ = min(+static_cast<int>(torus[0].size()), sweepZ + 1);
    break;
  case GLFW_KEY_K:
    sweepZ = max(-static_cast<int>(torus[0].size()), sweepZ - 1);
    break;
  }
  cout << "X: " << sweepZ << ", Y: " << sweepY << endl;
}

torus PrepareTorus() {
  const int angleZ = 18;
  const int angleY = 36;

  auto rotSmall = rotate2x2(2 * PI / angleZ);
  vector<matrix<float>> sRing;
  sRing.emplace_back(matrix<float>{ { 1 }, { 0 } });
  cout << fixed;
  for (int i = 1; i <= angleZ; i++) {
    sRing.emplace_back(rotSmall * *sRing.rbegin());
  }
  transform(sRing.begin(), sRing.end(), sRing.begin(), [=](matrix<float>& mat) {
    return matrix<float> { {mat[0][0] + 2.f}, { mat[1][0] + 2.f }, { 0 }};
  });

  torus bRing;
  bRing.push_back(sRing);
  auto rotBig = rotateY(2 * PI / angleY);
  for (int i = 1; i <= angleY; i++) {
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

template <typename PIt>
void drawTorusInner(PIt by, int sy, PIt bz, int sz) {
  auto glVertexMat = [](matrix<float>& v) {
    glVertex3f(v[0][0], v[1][0], v[2][0]);
  };

  glColor3f(0, 0, 1);
  for (PIt y = by + 1, ey = by + sy; i != ey; i++) {
    glBegin(GL_QUAD_STRIP);
    auto& l = *(y - 1);
    auto& r = *y;
    for (auto z = 0; z < sz; z++) {
      glVertexMat(l[z]);
      glVertexMat(r[z]);
    }
    glEnd();
  }

  // draw wireframe
  glColor3f(0, 0, 0);
  glTranslatef(0.0000001, 0.0000001, 0.0000001);
  if (sweepY <= 1 || sweepZ <= 1) return;


  for (PIt y = by, ey = by + sy; y != ey; y++) {
    glBegin(GL_LINE_STRIP);
    for (PIt z = 0; z < sz; z++)
      glVertexMat(y[z]);
    glEnd();
  }

  for (It z = bz; i < sweepZ; i++) {
    glBegin(GL_LINE_STRIP);
    for (int j = 0; j < sweepY; j++)
      glVertexMat(torus[j][i]);
    glEnd();
  }
}

void HW2Window::drawTorus() {
  int dy = sweepY >= 0 ? 1 : -1;
  int dz = sweepZ >= 0 ? 1 : -1;
  int ay = abs(sweepY);
  int az = abs(sweepZ);
  glEnable(GL_DEPTH_TEST);
  glClearDepth(1);
  glDepthFunc(GL_LESS);

  auto glVertexMat = [](matrix<float>& v) {
    glVertex3f(v[0][0], v[1][0], v[2][0]);
  };
  glColor3f(0, 0, 1);
  for (int i = 1; i < ay; i++) {
    glBegin(GL_QUAD_STRIP);
    auto& l = torus[dy < 0 ? torus.size() - i : i - 1];
    auto& r = torus[dy < 0 ? torus.size() - 1 - i : i];
    for (int j = 0; j < az; j++) {
      int rj = dz < 0 ? torus[0].size() - 1 - j : j;
      glVertexMat(l[rj]);
      glVertexMat(r[rj]);
    }
    glEnd();
  }

  // draw wireframe
  glColor3f(0, 0, 0);
  glTranslatef(0.001, 0.001, 0.001);
  if (ay <= 1 || az <= 1) return;

  for (int i = 0; i < ay; i++) {
    glBegin(GL_LINE_STRIP);
    for (int j = 0; j < az; j++)
      glVertexMat(torus
        [dy < 0 ? torus.size() - 1 - i : i]
        [dz < 0 ? torus[0].size() - 1 - j : j]);
    glEnd();
  }

  for (int i = 0; i < az; i++) {
    glBegin(GL_LINE_STRIP);
    for (int j = 0; j < ay; j++)
      glVertexMat(torus[dy < 0 ? torus.size() - 1 - j : j][dz < 0 ? torus[0].size() - 1 - i : i]);
    glEnd();
  }

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
