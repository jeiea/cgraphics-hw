#include <cmath>
#include <fstream>
#include <vector>
#include "HW3.hpp"

using namespace std;

HW3Window::HW3Window() : HWWindow("HW3 - Lighting with bunny", 640, 480) {
  onResize(640, 480);

  loadModel("bunny.txt");
}

vec3f cross_product(const vec3f& p1, const vec3f& p2) {
  return {
    p1[1] * p2[2] - p1[2] * p2[1],
    p1[2] * p2[0] - p1[0] * p2[2],
    p1[0] * p2[1] - p1[1] * p2[0],
  };
}

vec3f face_normal(const vector<vec3f>& vertices, const vec3i& tri) {
  auto& v0 = vertices[tri[0]];
  auto& v1 = vertices[tri[1]];
  auto& v2 = vertices[tri[2]];

  vec3f l, r;
  transform(begin(v1), end(v1), begin(v0), begin(l), minus<>());
  transform(begin(v2), end(v2), begin(v0), begin(r), minus<>());

  return cross_product(l, r);
}

vec3f normalize(const vec3f& v) {
  float l = hypot(hypot(v[0], v[1]), v[2]);
  return { v[0] / l, v[1] / l, v[2] / l };
}

void HW3Window::loadModel(const char* path) {
  auto f = ifstream(path);
  if (!f) return;

  int ver_count, tri_count;
  f >> ver_count >> tri_count;

  vertices.reserve(ver_count);
  const float zoom = 10.0f;
  for (int i = 0; i < ver_count; i++) {
    float x, y, z;
    if (!(f >> x >> y >> z)) break;
    vertices.emplace_back(vec3f{ (x - 1.f) * zoom, y * zoom, z * zoom });
  }

  triangles.reserve(tri_count);
  normals.resize(ver_count);
  for (int i = 0; i < tri_count; i++) {
    int a, b, c;
    if (!(f >> a >> b >> c)) break;

    vec3i triangle{ a - 1, b - 1, c - 1 };
    triangles.push_back(triangle);

    // normal accumulation
    auto normal = normalize(face_normal(vertices, triangle));
    for (auto vi : triangle) {
      auto& n = normals[vi];
      transform(begin(n), end(n), begin(normal), begin(n), plus<>());
    }
  }

  // normal average & offset
  for (int i = 0; i < normals.size(); i++) {
    auto& n = normals[i];
    auto& v = vertices[i];
    n = normalize(n);
    n[0] += v[0];
    n[1] += v[1];
    n[2] += v[2];
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

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glClearDepth(1);

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertices.data());

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1, 1);
  glColor3f(0, 0, 1);
  glDrawElements(GL_TRIANGLES, triangles.size() * 3, GL_UNSIGNED_INT, triangles.data());
  glDisable(GL_POLYGON_OFFSET_FILL);

  // Draw wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3f(0, 0, 0);
  //glEnable(GL_LINE_SMOOTH);
  glDrawElements(GL_TRIANGLES, triangles.size() * 3, GL_UNSIGNED_INT, triangles.data());

  glBegin(GL_LINES);
  for (int i = 0; i < normals.size(); i++) {
    auto& v = vertices[i];
    auto& n = normals[i];
    glVertex3f(v[0], v[1], v[2]);
    glVertex3f(n[0], n[1], n[2]);
  }
  glEnd();
}
