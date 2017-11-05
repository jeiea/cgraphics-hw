#include <fstream>
#include <vector>
#include "HW3.hpp"

using namespace std;

HW3Window::HW3Window() : HWWindow("HW3 - Lighting with bunny", 640, 480) {
  onResize(640, 480);

  LoadModel("bunny.txt");
}

void HW3Window::LoadModel(const char* path) {
  auto f = ifstream(path);
  if (!f) return;

  int ver_count, tri_count;
  f >> ver_count >> tri_count;

  vertices.reserve(ver_count);
  for (int i = 0; i < ver_count; i++) {
    float x, y, z;
    if (!(f >> x >> y >> z)) break;
    vertices.emplace_back(forward_as_tuple(x, y, z));
  }

  triangles.reserve(tri_count);
  for (int i = 0; i < tri_count; i++) {
    int a, b, c;
    if (!(f >> a >> b >> c)) break;
    triangles.emplace_back(forward_as_tuple(a, b, c));
  }
}

void HW3Window::onKeyInput(int key, int action) {
  HWWindow::onKeyInput(key, action);

  switch (key) {
  case GLFW_KEY_P:
  case GLFW_KEY_D:
  case GLFW_KEY_S:
  case GLFW_KEY_C:
  case GLFW_KEY_N:
    break;
  }
}

void HW3Window::onDraw() {
  drawBackground();

}
