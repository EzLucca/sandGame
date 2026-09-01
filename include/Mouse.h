#pragma once

#include <GLFW/glfw3.h>

class Mouse
{
public:
    Mouse(GLFWwindow* window, int width, int height);

    void update();

    int getX() const;
    int getY() const;

    bool isLeftPressed() const;
    bool isErasePressed() const;

private:
    GLFWwindow* window;

    int width;
    int height;

    int x;
    int y;

    bool leftPressed;
    bool erasePressed;
};
