#pragma once

#include <GLFW/glfw3.h>

class Mouse
{
    private:
        GLFWwindow* window;

        int width;
        int height;

        int x;
        int y;
        int brush_radius = 8;

        bool leftPressed;
        bool erasePressed;

    public:
        Mouse(GLFWwindow* window, int width, int height);

        void update();

        int getX() const;
        int getY() const;

        bool isLeftPressed() const;
        bool isErasePressed() const;

        int getBrushRadius() const;
        void setBrushRadius(int radius);
        void scroll(float yoffset);
};
