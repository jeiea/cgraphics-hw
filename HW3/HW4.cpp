#include <fstream>
#include "HW4.hpp"
using namespace std;

const int angleZ = 18;
const int angleY = 36;

vector<unsigned char> raw;
unsigned readTexture(const char* path, int w, int h, int format) {
  auto f = ifstream(path, ios_base::binary);
  if (!f) {
    cerr << "Failed to read " << path << endl;
    return -1;
  }

  if (format == GL_RGB) {
    raw.resize(h * w * 3);
    for (int i = h - 1; i >= 0; i--) {
      f.read(reinterpret_cast<char*>(raw.data()) + i * w * 3, w * 3);
    }
  }
  else {
    raw.resize(h * w * 4);
    for (int i = h - 1; i >= 0; i--) {
      auto p = raw.begin() + i * w * 4;
      for (int j = 0; j < w; j++) {
        int alpha = f.get();
        *p++ = *p++ = *p++ = *p++ = alpha;
      }
    }
    format = GL_RGBA;
  }

  unsigned id;
  glGenTextures(1, &id);

  glBindTexture(GL_TEXTURE_2D, id);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, raw.data());

  return id;
}

HW4Window::HW4Window() : HWWindow("HW4 - Texture Mapping", 640, 480) {
  HWWindow::onResize(640, 480);

  torus.reserve(angleZ * angleY);
  for (auto& small : prepare_torus(angleZ, angleY))
    for (auto& v : small)
      torus.emplace_back(vec3f{ v[0][0], v[1][0], v[2][0] });

  vector<matrix<float>> small;
  small.reserve(angleZ);
  normals.reserve(angleZ * angleY);
  {
    small.emplace_back(matrix<float>{ { 1 }, { 0 } });
    auto rot_small = rotate2x2(2 * PI / angleZ);
    for (int i = 0; i < angleZ; i++)
      small.emplace_back(rot_small * *rbegin(small));
    transform(begin(small), end(small), begin(small), [](auto v) {
      return matrix<float>{ {v[0][0]}, { v[1][0] }, { 0.f } };
    });
    for (auto& v : small)
      normals.emplace_back(vec3f{ v[0][0], v[1][0], v[2][0] });
  }

  auto rot_big = rotateY(2 * PI / angleY);
  for (int i = 1; i < angleY; i++) {
    for (auto& v : small) {
      v = rot_big * v;
      normals.emplace_back(vec3f{ v[0][0], v[1][0], v[2][0] });
    }
  }

  tex_id = readTexture("Material/check.raw", 512, 512, GL_RGB);
}

void HW4Window::onKeyInput(int key, int action)
{
  HWWindow::onKeyInput(key, action);
  if (action == GLFW_RELEASE) return;

  unsigned old_tex_id = tex_id;
  switch (key) {
  case GLFW_KEY_S:
    tex_id = readTexture("Material/wood.raw", 512, 512, GL_RGB);
    break;
  case GLFW_KEY_T:
    tex_id = readTexture("Material/check.raw", 512, 512, GL_RGB);
    break;
  case GLFW_KEY_U:
    tex_id = readTexture("Material/donut.raw", 512, 512, GL_RGB);
    break;
  case GLFW_KEY_V:
    tex_id = readTexture("Material/marble.raw", 512, 512, GL_RGB);
    break;
  case GLFW_KEY_W:
    tex_id = readTexture("Material/logo2.raw", 512, 512, GL_RED);
    break;
  case GLFW_KEY_X:
    tex_id = readTexture("Material/grayscale_ornament.raw", 512, 512, GL_RED);
    break;
  default:
    return;
  }
  glDeleteTextures(1, &old_tex_id);
}

void test(GLuint tex_id) {
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  //glEnableClientState(GL_NORMAL_ARRAY);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glPointSize(10);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  //glColor4f(0, 0, 0, -1);
  glColor3f(1, 1, 1);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  array<float, 12> tmp_vx{ {
      0.f, 2.f, 2.f, // lt
      0.f, 0.f, 2.f, // lb
      2.f, 0.f, 0.f, // rb
      2.f, 2.f, 0.f, // rt
  } };
  glVertexPointer(3, GL_FLOAT, 0, tmp_vx.data());

  array<float, 8> tex_vx{ {
      0.f, 1.f,
      0.f, 0.f,
      1.f, 0.f,
      1.f, 1.f,
  } };
  glTexCoordPointer(2, GL_FLOAT, 0, tex_vx.data());

  array<int, 10> poly{ {
      0, 1, 2, 3, 0, 2
  } };
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, poly.data());

  glDisable(GL_TEXTURE_2D);
}

void HW4Window::onDraw() {
  drawBackground();

  test(tex_id);
  return;

  glEnable(GL_TEXTURE_2D);
  //glEnable(GL_DEPTH_TEST);
  //glEnable(GL_LIGHTING);
  //glEnable(GL_COLOR_MATERIAL);

  glFrontFace(GL_CCW);
  //glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  GLfloat full[4]{ 1, 1, 1, 1 };
  GLfloat null[4]{ 0, 0, 0, 0 };
  GLfloat point_lit_pos[4]{ 4, 4, 2, 1 };

  //glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, full);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, full);
  glLightfv(GL_LIGHT0, GL_POSITION, point_lit_pos);
  glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 128.f);

  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  //glEnable(GL_POLYGON_OFFSET_FILL);
  //glPolygonOffset(1, 1);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, torus.data());
  glNormalPointer(GL_FLOAT, 0, normals.data());

  const int lenZ = angleZ + 1;
  const int lenY = angleY + 1;

  array<float, lenZ * lenY * 2> tex;
  {
    auto p = begin(tex);
    for (float i = 0; i < 18; i++) {
      for (float j = 0; j < lenZ; j++) {
        *p++ = i / angleY;
        *p++ = j / angleZ;
      }
    }
  }

  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, tex.data());
  //glBindTexture(GL_TEXTURE_2D, 0);
  glBindTexture(GL_TEXTURE_2D, tex_id);

  array<int, lenZ * angleY * 2> buf;
  for (int i = 0; i < lenZ * angleY; i++) {
    buf[2 * i + 0] = i;
    buf[2 * i + 1] = i + lenZ;
  }
  glDrawElements(GL_TRIANGLE_STRIP, static_cast<int>(buf.size()), GL_UNSIGNED_INT, buf.data());

  glPolygonOffset(0, 0);
  glDisable(GL_POLYGON_OFFSET_FILL);
  glDisable(GL_TEXTURE_2D);

  glDisable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);

  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
}
