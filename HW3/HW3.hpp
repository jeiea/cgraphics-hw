#pragma once
#include "HW.hpp"

class HW3Window : public HWWindow {
public:
  HW3Window();

  virtual void onKeyInput(int key, int action);
  virtual void onDraw();

protected:
};
