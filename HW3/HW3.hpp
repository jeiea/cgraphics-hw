#pragma once
#include <tuple>
#include <vector>
#include "HW.hpp"

using namespace std;

class HW3Window : public HWWindow {
public:
  HW3Window();

  virtual void onKeyInput(int key, int action);
  virtual void onDraw();

protected:
  vector<tuple<float, float, float>> vertices;
  vector<tuple<int, int, int>> triangles;

  void LoadModel(const char* path);
};
