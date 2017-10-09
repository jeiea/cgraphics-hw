#include <iostream>
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

#define GLFW_INCLUDE_GLU

#include <GLFW/glfw3.h>

using namespace std;

void onKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_E && action == GLFW_PRESS);
}

void onResize(GLFWwindow* win, int width, int height) {
  if (height == 0) height = 1;                // To prevent divide by 0
  GLfloat aspect = (GLfloat)width / (GLfloat)height;

  // Set the viewport to cover the new window
  glViewport(0, 0, width, height);

  // Set the aspect ratio of the clipping volume to match the viewport
  glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
  glLoadIdentity();             // Reset
                                // Enable perspective projection with fovy, aspect, zNear and zFar
  gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

int main() {
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit()) {
    cerr << "OpenGL 초기화에 실패했습니다." << endl;
    return -1;
  }

  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "HW2 - Drawing Torus", NULL, NULL);
  if (!window)
  {
    cerr << "OpenGL 초기화에 실패했습니다." << endl;
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  //std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

  glfwSetKeyCallback(window, onKeyInput);
  glfwSetFramebufferSizeCallback(window, onResize);
  glfwSetWindowSize(window, 640, 480);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);
    //glVertex3f();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwWaitEvents();
  }

  glfwTerminate();
  return 0;
}
