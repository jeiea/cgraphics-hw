#include <fstream>
#include <vector>
#include <valarray>
#include "HW3.hpp"

using namespace std;

HW3Window::HW3Window() : HWWindow("HW3 - Lighting with bunny", 640, 480) {
  onResize(640, 480);

  loadModel("bunny.txt");
}

point3 get_cross(const point3& p1, const point3& p2) {
  return {
    p1[1] * p2[2] - p1[2] * p2[1],
    p1[2] * p2[0] - p1[0] * p2[2],
    p1[0] * p2[1] - p1[1] * p2[0],
  };
}

point3 get_face_normal(const vector<point3>& vertices, const trian& tri) {
  auto& v0 = vertices[tri[0]];
  auto& v1 = vertices[tri[1]];
  auto& v2 = vertices[tri[2]];

  auto l = v1 - v0;
  auto r = v2 - v0;

  return l * r;
}

void HW3Window::loadModel(const char* path) {
  auto f = ifstream(path);
  if (!f) return;

  int ver_count, tri_count;
  f >> ver_count >> tri_count;

  vertices.reserve(ver_count);
  for (int i = 0; i < ver_count; i++) {
    float x, y, z;
    if (!(f >> x >> y >> z)) break;
    vertices.emplace_back(x, y, z);
  }

  triangles.reserve(tri_count);
  for (int i = 0; i < tri_count; i++) {
    int a, b, c;
    if (!(f >> a >> b >> c)) break;

    trian triangle{ a, b, c };
    triangles.push_back(triangle);
    //normals.push_back(get_face_normal(vertices, triangle));
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

  for (auto& tri : triangles) {
  }
}
