#pragma once
#include "HW2.hpp"
#include "HW3.hpp"

using vec4i = array<int, 4>;

class HW4Window : public HWWindow {
public:
  HW4Window();

  virtual void onKeyInput(int key, int action);
  virtual void onDraw();

protected:
  vector<vec3f> torus;
  vector<vec3f> normals;
  vector<vec3f> i_normals;
  vector<int> indices;
  unsigned tex_id;
  bool tex_alpha;
  // positive means absolute time
  // negative means time offset
  double animation = 0.0;
};
