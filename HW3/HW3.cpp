#include "HW3.hpp"

HW3Window::HW3Window() : HWWindow("HW3 - Lighting with bunny", 640, 480) {
  onResize(640, 480);
}

void HW3Window::onKeyInput(int key, int action) {
  HWWindow::onKeyInput(key, action);

  switch (key) {
  case GLFW_KEY_P:
  case GLFW_KEY_D:
  case GLFW_KEY_S:
  case GLFW_KEY_C:
  case GLFW_KEY_N:
    break;
  }
}

void HW3Window::onDraw() {
  drawBackground();

}
