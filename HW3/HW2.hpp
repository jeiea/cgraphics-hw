#pragma once
#include <vector>
#include "matrix.hpp"
#include "HW.hpp"

template <typename T>
using grid = std::vector<std::vector<T>>;
using vertices = grid<matrix<float>>;

const float PI = 3.141592653589793238462643383279502884f;

class HW2Window : public HWWindow {
public:
  HW2Window();

  virtual void onKeyInput(int key, int action);
  virtual void onDraw();

protected:
  static vertices torus;
  static vertices centers;
  static vertices normals;
  static grid<bool> gridRB;
  int sweepZ;
  int sweepY;
};

vertices prepare_torus(int angleZ, int angleY);
tuple<vertices, vertices, grid<bool>> prepare_normals(const vertices& torus);

matrix<float> scale(float mag);
matrix<float> rotate2x2(float rad);
matrix<float> rotateY(float radian);
