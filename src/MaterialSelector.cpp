#include "MaterialSelector.h"

MaterialSelector::MaterialSelector(GLFWwindow* window)
    : window(window)
{
}

const Material* MaterialSelector::update()
{
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        return &sandMaterial;
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        return &stoneMaterial;
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        return &fireMaterial;
    }

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        return &waterMaterial;
    }

    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        return &smokeMaterial;
    }

    return nullptr;
}
