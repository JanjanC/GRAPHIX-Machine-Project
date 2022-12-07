#include "Model3D.h"
#pragma once

class Model : public Model3D {
public:

    glm::vec3 direction;

    //constructor for the main model class
    Model(std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 theta) : Model3D(modelPath, position, scale, theta) {
        direction = glm::normalize(glm::vec3(sin(glm::radians(theta.y)), 0, cos(glm::radians(theta.y))));
    }

    //process keyboard inputs and update the object attributes
    void processKeyboard(int key) {
        float rot_sensitivity = 1.0f;
        float vert_sensitivity = 0.2f;
        float horz_sensitivity = 0.4f;

        //update the direction of the model
        direction = glm::normalize(glm::vec3(sin(glm::radians(theta.y)), 0, cos(glm::radians(theta.y))));

        if (key == GLFW_KEY_Q || key == GLFW_KEY_E) {
            /*Elevate*/
            if (key == GLFW_KEY_Q && position.y) {
                position.y = glm::min(position.y + vert_sensitivity, 0.0f);
            }
            /*Descend*/
            if (key == GLFW_KEY_E) {
                position.y -= vert_sensitivity;
            }
            /*Update depth info*/
            printf("\rCurrent ocean depth: %.2f ", position.y);
        }

        /*Traverse Forward*/
        if (key == GLFW_KEY_W) {
            position += direction * horz_sensitivity;
        }
        /*Traverse Backward*/
        if (key == GLFW_KEY_S) {
            position -= direction * horz_sensitivity;
        }
        /*Rotate to left*/
        if (key == GLFW_KEY_A) {
            theta.y += rot_sensitivity + 1.0;
        }
        /*Rotate to right*/
        if (key == GLFW_KEY_D) {
            theta.y -= rot_sensitivity + 1.0;
        }        
    }
};