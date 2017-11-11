#pragma once
#include <array>
#include "HW.hpp"

using namespace std;

using vec3f = array<float, 3>;
using vec3i = array<int, 3>;

class HW3Window : public HWWindow {
public:
  HW3Window();

  virtual void onKeyInput(int key, int action);
  virtual void onDraw();

protected:
  vector<vec3i> triangles;
  vector<vec3f> normals;
  vector<vec3f> vertices;

  void loadModel(const char* path);
  //void getFaceNormal(const trian& tri);
};
