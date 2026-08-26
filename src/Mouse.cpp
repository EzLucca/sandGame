#include "Mouse.hpp"

Mouse::Mouse(
        GLFWwindow* window,
        int width,
        int height)
    :
        window(window),
        width(width),
        height(height),
        x(0),
        y(0),
        leftPressed(false),
        erasePressed(false)
{
}

void Mouse::update()
{
    double mouseX;
    double mouseY;

    glfwGetCursorPos( window, &mouseX, &mouseY);

    int windowWidth;
    int windowHeight;

    glfwGetWindowSize( window, &windowWidth, &windowHeight);

    // Convert window coordinates
    // to simulation coordinates.

    x = static_cast<int>(
            mouseX * width / windowWidth
            );

    y = static_cast<int>(
            mouseY * height / windowHeight
            );

    leftPressed =
        glfwGetMouseButton(
                window,
                GLFW_MOUSE_BUTTON_LEFT
                ) == GLFW_PRESS;

    erasePressed =
        glfwGetKey(
                window,
                GLFW_KEY_E
                ) == GLFW_PRESS;
}

int Mouse::getX() const
{
    return x;
}

int Mouse::getY() const
{
    return y;
}

bool Mouse::isLeftPressed() const
{
    return leftPressed;
}

bool Mouse::isErasePressed() const
{
    return erasePressed;
}
