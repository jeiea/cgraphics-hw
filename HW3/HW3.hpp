#pragma once
#include <valarray>
#include <vector>
#include "HW.hpp"

using namespace std;

using point3 = valarray<float>;
using trian = valarray<int>;

class HW3Window : public HWWindow {
public:
  HW3Window();

  virtual void onKeyInput(int key, int action);
  virtual void onDraw();

protected:
  vector<trian> triangles;
  vector<point3> normals;
  vector<point3> vertices;

  void loadModel(const char* path);
  void getFaceNormal(const trian& tri);
};
