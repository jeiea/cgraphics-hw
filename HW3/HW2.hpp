#pragma once
#include <vector>
#include "matrix.hpp"
#include "HW.hpp"

template <typename T>
using grid = std::vector<std::vector<T>>;
using vertices = grid<matrix<float>>;

class HW2Window : public HWWindow {
public:
  HW2Window();

  virtual void onResize(int width, int height);
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
