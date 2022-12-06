#include "Model3D.h"
#pragma once

class MainModel : public Model3D {
public:

    //constructor for the main model class
    MainModel(std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 theta) : Model3D(modelPath, position, scale, theta) {}

    //process keyboard inputs and update the object attributes
    void processKeyboard(int key) {
        float sensitivity = 5.0f;

        //rotate around the x axis
        if (key == GLFW_KEY_W) {
            theta.x -= sensitivity;
        }

        //rotate around the x axis
        if (key == GLFW_KEY_S) {
            theta.x += sensitivity;
        }

        //rotate around the y axis 
        if (key == GLFW_KEY_A) {
            theta.y -= sensitivity;
        }

        //rotate around the y axis
        if (key == GLFW_KEY_D) {
            theta.y += sensitivity;
        }

        //rotate around the z axis
        if (key == GLFW_KEY_E) {
            theta.z -= sensitivity;
        }

        //rotate around the z axis
        if (key == GLFW_KEY_Q) {
            theta.z += sensitivity;
        }
    }
};