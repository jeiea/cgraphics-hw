#include <iostream>
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

tuple<float, float> apply(matrix<float> mat, float x, float y) {
  auto res = mat * matrix<float>{ { x }, { y } };
  return { res[0][0], res[0][1] };
}

matrix<float> rotateY(float radian) {
  return {
    {cos(radian), 0, sin(radian)},
    {0, 1, 0},
    {-sin(radian), 0, cos(radian)}
  };
}

void onKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (action != GLFW_RELEASE) return;

  switch (key) {
  case GLFW_KEY_A:
    break;
  case GLFW_KEY_S:
    break;
  case GLFW_KEY_J:
    break;
  case GLFW_KEY_K:
    break;
  }
}

void onResize(GLFWwindow* win, int width, int height) {
  if (height == 0) height = 1;   // To prevent divide by 0
  GLfloat aspect = (GLfloat)width / (GLfloat)height;

  // Set the viewport to cover the new window
  glViewport(0, 0, width, height);

  // Set the aspect ratio of the clipping volume to match the viewport
  glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
  glLoadIdentity();             // Reset

  // Enable perspective projection with fovy, aspect, zNear and zFar
  double distance = 10;
  gluPerspective(45.0, aspect, 0.1, distance * 10);
  gluLookAt(distance, distance, distance, 0, 0, 0, -1, 1, -1);
}

void drawParallels() {
  for (int i = -30; i <= 30; i++) {
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
  }
}

void drawRadiation() {
  float part = PI / 20;
  float r = 1000.0f;
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //glTranslatef(1.0f, 0.0f, -7.0f);  // Move right and into the screen
  glBegin(GL_LINES);
  for (float t = 0; t < 2 * PI; t += part) {
    //float blue = cos(t);
    float x = cos(t);
    float red = x / 2 + 0.5f;
    for (float p = 0; p < 2 * PI; p += part) {
      float y = sin(t) * cos(p);
      float z = cos(t) * sin(p);
      float red = (t + 1) / 2;
      float green = (p + 1) / 2;
      glColor3f(red, y / 2 + 0.5f, z / 2 + 0.5f);
      //glColor3f(1, 1, 1);
      glVertex3f(0, 0, 0);
      glVertex3f(x * r, y * r, z * r);
    }
  }
  glEnd();
}

void drawSphere() {
  float part = PI / 20;
  float r = 100.0f;
  glBegin(GL_LINES);
  for (float t = 0; t < 2 * PI; t += part) {
    float x = cos(t);
    float red = x / 2 + 0.5f;
    for (float p = 0; p < 2 * PI; p += part) {
      float y = sin(t) * cos(p);
      float z = cos(t) * sin(p);
    }
  }
  glEnd();
}

void DrawAxes() {
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

using torus = vector<vector<matrix<float>>>;

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

void drawTorus(torus& torus, int sweepZ, int sweepY) {
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

void onDraw() {
  /* Handler for window-repaint event.
  Called back when the window first appears and
  whenever the window needs to be re-painted. */
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  DrawAxes();
}

int main() {
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit()) {
    cerr << "OpenGL 초기화에 실패했습니다." << endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "HW2 - Drawing Torus", NULL, NULL);
  if (!window)
  {
    cerr << "OpenGL 초기화에 실패했습니다." << endl;
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  //std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

  glfwSetKeyCallback(window, onKeyInput);
  glfwSetFramebufferSizeCallback(window, onResize);
  onResize(window, 640, 480);
  glfwSwapInterval(1);

  char buf[128];
  auto torus = PrepareTorus();

  int sweepZ = 18;
  int sweepY = 36;
  /* Loop until the user closes the window */
  int count = 0;
  while (!glfwWindowShouldClose(window))
  {
    /* Render here */
    sprintf_s(buf, sizeof(buf), "Draw count: %d", ++count);
    cout << buf << endl;
    onDraw();
    drawTorus(torus, sweepZ, sweepY);
    //drawRadiation();
    //drawSphere();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwWaitEvents();
  }

  glfwTerminate();
  return 0;
}

int main_(void)
{
  glfwInit();
  GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  glfwMakeContextCurrent(window);
  while (!glfwWindowShouldClose(window))
  {
    float ratio;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef((float)glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
    glBegin(GL_TRIANGLES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(-0.6f, -0.4f, 0.f);
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(0.6f, -0.4f, 0.f);
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.6f, 0.f);
    glEnd();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}