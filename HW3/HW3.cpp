#include <cmath>
#include <fstream>
#include <vector>
#include "matrix.hpp"
#include "HW3.hpp"

using namespace std;

const float PI = 3.141592653589793238462643383279502884f;

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

void draw_model(
  const vector<vec3f>& vertices,
  const vector<vec3f>& normals,
  const vector<vec3i>& triangles)
{
  const auto total_vx = static_cast<GLsizei>(triangles.size() * 3);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glClearDepth(1);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertices.data());
  glNormalPointer(GL_FLOAT, 0, normals.data());

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1, 1);
  glColor3f(0, 0, 1);
  glDrawElements(GL_TRIANGLES, total_vx, GL_UNSIGNED_INT, triangles.data());
  glDisable(GL_POLYGON_OFFSET_FILL);

  // Draw wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3f(0, 0, 0);
  //glEnable(GL_LINE_SMOOTH);
  glDrawElements(GL_TRIANGLES, total_vx, GL_UNSIGNED_INT, triangles.data());
}

void draw_vertex_normals(
  const vector<vec3f>& vertices,
  const vector<vec3f>& normals)
{
  glBegin(GL_LINES);
  for (int i = 0; i < normals.size(); i++) {
    auto& v = vertices[i];
    auto& n = normals[i];
    glVertex3f(v[0], v[1], v[2]);
    glVertex3f(n[0], n[1], n[2]);
  }
  glEnd();
}

// axis should be 1x3 matrix. axis' last should be zero.
matrix<float> axis_rotate(vec3f axis, float rad) {
  auto n = normalize(axis);
  matrix<float> norm { {n[0], n[1], n[2]} };
  auto proj = norm.transpose() * norm;
  auto iden = matrix<float>(3, 1.f);
  auto dual = matrix<float>{
    {0, -norm[0][2], norm[0][1]},
    {norm[0][2], 0, -norm[0][0]},
    {-norm[0][1], norm[0][0], 0},
  };
  return proj + cos(rad) * (iden - proj) + sin(rad) * dual;
}

void HW3Window::onDraw() {
  draw_background();

  glPointSize(10.f);
  glBegin(GL_POINTS);
  matrix<float> init{ { 0 },{ 8 },{ 0 } };
  init *= 0.3f;
  vec3f axis{ { 1, 1, 1 } };
  double off_dir = secDir < 0 ? -secDir : glfwGetTime() - secDir;
  auto pos_dir = axis_rotate(axis, off_dir * 0.5f * PI) * init;
  glVertex3f(pos_dir[0][0], pos_dir[1][0], pos_dir[2][0]);
  cout << pos_dir[0][0] << ' ' << pos_dir[1][0] << ' ' <<  pos_dir[2][0] << endl;
  glEnd();

  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);

  GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1.f };
  GLfloat diffuse[4] = { 1, 1, 1, 1 };
  GLfloat null[4] = { 0, 0, 0, 0 };
  GLfloat lightPos[4] = { pos_dir[0][0], pos_dir[1][0], pos_dir[2][0], 0 };
  GLfloat direction[4] = { -1,-1,-1,0 };

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, null);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

  GLfloat specular[4] = { 1,1,1,1 };

  //glEnable(GL_LIGHT1);
  //glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
  //glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction);

  GLfloat mat_ambient[4] = { 0.3,0.3,1,1 };
  GLfloat mat_diffuse[4] = { 0.6,0.6,0.6,1 };

  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  //glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

  draw_model(vertices, normals, triangles);
  draw_vertex_normals(vertices, normals);
}
