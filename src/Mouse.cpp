#include <algorithm>
#include "Mouse.h"

constexpr int MIN_BRUSH_RADIUS = 1;
constexpr int MAX_BRUSH_RADIUS = 50;

Mouse::Mouse( GLFWwindow* window, int width, int height)
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

    x = static_cast<int>( mouseX * width / windowWidth);

    y = static_cast<int>( mouseY * height / windowHeight);

    leftPressed = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    erasePressed = glfwGetKey( window, GLFW_KEY_E) == GLFW_PRESS;
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

int Mouse::getBrushRadius() const
{
    return brush_radius;
}

void Mouse::setBrushRadius(int radius) 
{ 
    brush_radius = std::clamp(
        radius,
        MIN_BRUSH_RADIUS,
        MAX_BRUSH_RADIUS
    );
}

void Mouse::scroll(float yoffset)
{
    if (yoffset > 0)
        setBrushRadius(getBrushRadius() + 1);
    else if (yoffset < 0)
        setBrushRadius(getBrushRadius() - 1);
}
