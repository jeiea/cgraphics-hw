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
    //vertices.emplace_back(vec3f{ x, y, z });
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

  auto bn = begin(normals);
  transform(bn, end(normals), bn, normalize);
}

void HW3Window::onKeyInput(int key, int action) {
  HWWindow::onKeyInput(key, action);
  if (action != GLFW_PRESS) return;

  auto t = glfwGetTime();
  switch (key) {
  case GLFW_KEY_P:
    secPt = secPt < 0 ? t + secPt : -(t - secPt);
    break;
  case GLFW_KEY_D:
    secDir = secDir < 0 ? t + secDir : -(t - secDir);
    break;
  case GLFW_KEY_S:
    secSpot = secSpot < 0 ? t + secSpot : -(t - secSpot);
    break;
  case GLFW_KEY_C:
    secCutOff = secCutOff < 0 ? t + secCutOff : -(t - secCutOff);
    break;
  case GLFW_KEY_N:
    secShiny = secShiny < 0 ? t + secShiny : -(t - secShiny);
    break;
  }
}

void drawModel(
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

void drawVertexNormals(
  const vector<vec3f>& vertices,
  const vector<vec3f>& normals)
{
  glBegin(GL_LINES);
  for (int i = 0; i < normals.size(); i++) {
    auto& v = vertices[i];
    auto& n = normals[i];
    glVertex3f(v[0], v[1], v[2]);
    glVertex3f(v[0] + n[0], v[1] + n[1], v[2] + n[2]);
  }
  glEnd();
}

// axis should be 1x3 matrix. axis' last should be zero.
matrix<float> axis_rotate(vec3f axis, float rad) {
  auto n = normalize(axis);
  matrix<float> norm{ {n[0], n[1], n[2]} };
  auto proj = norm.transpose() * norm;
  auto iden = matrix<float>(3, 1.f);
  auto dual = matrix<float>{
    {0, -norm[0][2], norm[0][1]},
    {norm[0][2], 0, -norm[0][0]},
    {-norm[0][1], norm[0][0], 0},
  };
  return proj + cos(rad) * (iden - proj) + sin(rad) * dual;
}

vec3f swing(float sec) {
  matrix<float> init{ { 0 },{ 8 },{ 0 } };
  vec3f axis{ { 1, 1, 1 } };
  double off_dir = sec < 0 ? -sec : glfwGetTime() - sec;
  auto pos_dir = axis_rotate(axis, off_dir * 0.5f * PI) * init;
  return { pos_dir[0][0], pos_dir[1][0], pos_dir[2][0] };
}

// Negative period means discontinuous animations.
// Negative offset means stop state.
// It returns [0, 1] value.
float tween(float period, float offset) {
  float cont = period < 0.f ? 1.f : 2.f;
  float diff = abs(offset < 0.f ? offset : glfwGetTime() - offset);
  float val = fmod(diff * cont, period * cont);
  val = val < period ? val : period * 2 - val;
  return val / period;
}

void HW3Window::onDraw() {
  drawBackground();

  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);

  GLfloat full[4]{ 1, 1, 1, 1 };
  GLfloat null[4]{ 0, 0, 0, 0 };
  auto point_pos = swing(secPt);
  GLfloat point_lit_pos[4]{ point_pos[0], point_pos[1], point_pos[2], 1 };

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, null);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, full);
  glLightfv(GL_LIGHT0, GL_POSITION, point_lit_pos);
  glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 128.f);

  //glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.f);
  //glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.f);
  //glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.f);

  GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1 };
  GLfloat diffuse[4] = { 1, 1, 1, 1 };
  auto dir_pos = swing(secDir);
  GLfloat directional_lit_pos[4] = { dir_pos[0], dir_pos[1], dir_pos[2], 0 };

  //glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, directional_lit_pos);

  auto spot_pos = swing(secSpot);
  GLfloat spot_lit_pos[4] = { spot_pos[0], spot_pos[1], spot_pos[2], 1 };
  GLfloat spot_dir[4] = { -spot_pos[0], -spot_pos[1], -spot_pos[2], 1 };

  //glEnable(GL_LIGHT2);
  glLightfv(GL_LIGHT2, GL_SPECULAR, full);
  glLightfv(GL_LIGHT2, GL_DIFFUSE, full);
  glLightfv(GL_LIGHT2, GL_POSITION, spot_lit_pos);
  glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_dir);
  glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, tween(4.f, secCutOff) * 30.f);

  GLfloat mat_diffuse[4] = { 0.6f, 0.6f, 0.6f, 1 };
  GLfloat mat_specular[4] = { 0.6f, 0.6f, 0.6f, 1 };

  //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, null);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialf(GL_FRONT, GL_SHININESS, tween(4.f, secShiny + 2.f) * 128.f);

  drawModel(vertices, normals, triangles);

  glDisable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_NORMALIZE);

  drawVertexNormals(vertices, normals);
}
