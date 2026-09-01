#pragma once

#include <GLFW/glfw3.h>
#include "Materials.h"

class MaterialSelector
{
    private:
        GLFWwindow* window;

    public:
        MaterialSelector(GLFWwindow* window);

        const Material* update();
};
