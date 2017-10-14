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

template <typename T>
using grid = vector<vector<T>>;
using vertices = grid<matrix<float>>;

tuple<vertices, vertices, vertices, grid<bool>> PrepareTorus() {
  const int angleZ = 18;
  const int angleY = 36;

  vector<matrix<float>> sRing;
  {
    sRing.reserve(angleZ);
    sRing.emplace_back(matrix<float>{ { 1 }, { 0 } });
    auto rotSmall = rotate2x2(2 * PI / angleZ);
    for (int i = 1; i <= angleZ; i++)
      sRing.emplace_back(rotSmall * *sRing.rbegin());
    auto b = sRing.begin();
    auto e = sRing.end();
    auto toRightUp = [=](matrix<float>& mat) {
      return matrix<float> {
        { mat[0][0] + 2.f },
        { mat[1][0] + 2.f },
        { 0 }
      }; };
    transform(b, e, b, toRightUp);
  }

  vertices bRing;
  {
    bRing.reserve(angleY);
    bRing.push_back(sRing);
    auto rotBig = rotateY(2 * PI / angleY);
    auto rotater = [=](matrix<float>& o) { return rotBig * o; };
    for (int i = 1; i <= angleY; i++) {
      auto& last = *bRing.rbegin();
      auto beg = last.begin();
      auto end = last.end();
      vector<matrix<float>>&& next{};
      transform(beg, end, back_inserter(next), rotater);
      bRing.emplace_back(next);
    }
  }

  vertices centers, normals;
  vector<vector<bool>> tfros;
  auto toEye = matrix<float>{ { 1, 0.75f, 1 } };
  for (auto l = bRing.begin(), r = l + 1; r != bRing.end(); l++, r++) {
    vector<matrix<float>>&& center{}, normal{};
    vector<bool>&& tfro{};
    auto lb = l->begin(), lt = lb + 1;
    auto rb = r->begin(), rt = rb + 1;
    while (lt != l->end()) {
      center.emplace_back((*lb + *lt + *rb + *rt) / 4);

      auto v1 = *rb - *lb;
      auto v2 = *lt - *lb;
      auto cross = cross_product(v1, v2).transpose();
      normal.emplace_back(*center.rbegin() + cross);

      float dot = (toEye * cross)[0][0];
      tfro.push_back(dot >= 0);

      lb++; lt++; rb++; rt++;
    }
    centers.emplace_back(center);
    normals.emplace_back(normal);
    tfros.emplace_back(tfro);
  }

  return make_tuple(bRing, centers, normals, tfros);
}

class HW2Window : public GLWindow {
public:
  HW2Window() : GLWindow("HW2 - Drawing Torus", 640, 480) {
    onResize(640, 480);
    tie(torus, centers, normals, tfros) = PrepareTorus();
    sweepZ = static_cast<int>(torus[0].size()) - 1;
    sweepY = static_cast<int>(torus.size()) - 1;
  }

  virtual void onResize(int width, int height);
  virtual void onKeyInput(int key, int action);
  void drawTorus();

protected:
  static vertices torus;
  static vertices centers;
  static vertices normals;
  static grid<bool> tfros;
  int sweepZ;
  int sweepY;
};

void HW2Window::onResize(int width, int height) {
  GLWindow::onResize(width, height);

  if (height == 0) height = 1;   // To prevent divide by 0
  GLfloat aspect = (GLfloat)width / (GLfloat)height;

  // Set the aspect ratio of the clipping volume to match the viewport
  glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
  glLoadIdentity();             // Reset

  // Enable perspective projection
  double distance = 8;
  gluPerspective(45.0, aspect, distance, distance * 2);
  gluLookAt(distance, distance, distance, 0, 0, 0, -1, 1, -1);
}

void HW2Window::onKeyInput(int key, int action)
{
  if (action == GLFW_RELEASE) return;

  switch (key) {
  case GLFW_KEY_A:
    sweepY = min(+static_cast<int>(torus.size() - 1), sweepY + 1);
    break;
  case GLFW_KEY_S:
    sweepY = max(-static_cast<int>(torus.size() - 1), sweepY - 1);
    break;
  case GLFW_KEY_J:
    sweepZ = min(+static_cast<int>(torus[0].size() - 1), sweepZ + 1);
    break;
  case GLFW_KEY_K:
    sweepZ = max(-static_cast<int>(torus[0].size() - 1), sweepZ - 1);
    break;
  }
  cout << "X: " << sweepZ << ", Y: " << sweepY << endl;
}

vertices HW2Window::torus;
vertices HW2Window::centers;
vertices HW2Window::normals;
grid<bool> HW2Window::tfros;

void HW2Window::drawTorus() {
  int ay = abs(sweepY);
  int az = abs(sweepZ);
  if (ay == 0 || az == 0) return;
  int dy = sweepY >= 0 ? 1 : -1;
  int dz = sweepZ >= 0 ? 1 : -1;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glClearDepth(1);
  glTranslatef(0, 0, 0);

  auto glVertexMat = [](matrix<float>& v) {
    glVertex3f(v[0][0], v[1][0], v[2][0]);
  };

  for (int i = 1; i <= ay; i++) {
    glBegin(GL_QUAD_STRIP);
    auto& l = torus[dy < 0 ? torus.size() - i : i - 1];
    auto& r = torus[dy < 0 ? torus.size() - 1 - i : i];
    auto& tfro = tfros[dy < 0 ? tfros.size() - i : i - 1];
    for (int j = 0; j < az; j++) {
      bool isInside = tfro[dz < 0 ? static_cast<int>(tfro.size() - 1) - j : j];
      if (isInside) glColor3f(1, 0, 0);
      else glColor3f(0, 0, 1);
      int rj = dz < 0 ? static_cast<int>(torus[0].size() - 2) - j : j;
      glVertexMat(l[rj]);
      glVertexMat(r[rj]);
      glVertexMat(l[rj + 1]);
      glVertexMat(r[rj + 1]);
    }
    glEnd();
  }

  // Draw wireframe
  glColor3f(0, 0, 0);
  glTranslatef(0.01f, 0.00f, 0.01f);

  for (int i = 0; i <= ay; i++) {
    glBegin(GL_LINE_STRIP);
    for (int j = 0; j <= az; j++)
      glVertexMat(torus
        [dy < 0 ? torus.size() - 1 - i : i]
    [dz < 0 ? torus[0].size() - 1 - j : j]);
    glEnd();
  }

  for (int i = 0; i <= az; i++) {
    glBegin(GL_LINE_STRIP);
    for (int j = 0; j <= ay; j++)
      glVertexMat(torus
        [dy < 0 ? torus.size() - 1 - j : j]
    [dz < 0 ? torus[0].size() - 1 - i : i]);
    glEnd();
  }

  glBegin(GL_LINES);
  for (int i = 0; i < ay; i++)
    for (int j = 0; j < az; j++) {
      glVertexMat(centers
        [dy < 0 ? centers.size() - 1 - i : i]
      [dz < 0 ? centers[0].size() - 1 - j : j]);
      glVertexMat(normals
        [dy < 0 ? normals.size() - 1 - i : i]
      [dz < 0 ? normals[0].size() - 1 - j : j]);
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
