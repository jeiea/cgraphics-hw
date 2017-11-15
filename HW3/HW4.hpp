#pragma once
#include <vector>
#include "HW2.hpp"

template <typename T>
using grid = std::vector<std::vector<T>>;
using vertices = grid<matrix<float>>;

class HW4Window : public HWWindow {
public:
  HW4Window();

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
