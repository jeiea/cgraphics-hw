#if _WIN32 || _WIN64
#include <Windows.h>
#include <tchar.h>
int main();
int APIENTRY _tWinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPTSTR    lpCmdLine,
  int       nCmdShow)
{
  main();
}
#endif
#include <iostream>
#include "HW2.hpp"
#include "HW3.hpp"

using namespace std;

int loop(HWWindow& win) {
  if (win.bad())
  {
    cerr << "OpenGL 초기화에 실패했습니다." << endl;
    glfwTerminate();
    return -1;
  }

  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

  glfwSwapInterval(1);

  /* Loop until the user closes the window */
  int count = 0;
  while (!glfwWindowShouldClose(win))
  {
    char buf[128];
    sprintf_s(buf, sizeof(buf), "Draw count: %d", ++count);
    cout << buf << endl;

    /* Render here */
    win.onDraw();

    /* Swap front and back buffers */
    glfwSwapBuffers(win);

    /* Poll for and process events */
    glfwWaitEvents();
  }

  return win.exitMessage;
}

int main() {
  /* Initialize the library */
  if (!glfwInit()) {
    cerr << "OpenGL 초기화에 실패했습니다." << endl;
    return -1;
  }

  for (int kind = 3; kind > 0;) {
    switch (kind) {
    case 2:
      kind = loop(HW2Window());
      break;
    case 3:
      kind = loop(HW3Window());
      break;
    default:
      kind = 0;
      break;
    }
  }

  glfwTerminate();
  return 0;
}
