#include "MyCamera.h"
#pragma once

class OrthoCamera : public MyCamera {
public:
    float xLast, yLast; //stores the last cursor position
    bool isInitialized;//checks if the values of xLast and yLast have been initialized

    //constructor for the orthographic camera class
    OrthoCamera(glm::vec3 position, glm::vec3 target, glm::vec3 up) : MyCamera(position, target, up) {
        xLast = yLast = 0.0f;
        isInitialized = false;
    }

    //set the value of the projection matrix in the shader
    void setProjectionMatrix(Shader shader) override {
        shader.useProgram();

        //compute for the projection matrix
        projectionMatrix = glm::ortho(-WIDTH / 50, WIDTH / 50, -HEIGHT / 50, HEIGHT / 50, -100.0f, 100.0f);

        //set the value of the projection matrix in the shader
        unsigned int projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    }

    //process keyboard inputs and update the object attributes
    void processKeyboard(int key) {
        float sensitivity = 1.0f;

        // traverse forward
        if (key == GLFW_KEY_W) {           
            position.z -= sensitivity;
            target.z -= sensitivity;
        }
        // traverse backward
        if (key == GLFW_KEY_S) {
            position.z += sensitivity;
            target.z += sensitivity;
        }
        // rotate to left
        if (key == GLFW_KEY_A) {
            position.x -= sensitivity;
            target.x -= sensitivity;
        }
        // rotate to right
        if (key == GLFW_KEY_D) {
            position.x += sensitivity;
            target.x += sensitivity;
        }
    }

    //process the mouse inputs and updates the object attributes
    void processMouse(float xPos, float yPos) {
        //initialize the value of xLast and yLast
        if (!isInitialized) {
            xLast = xPos;
            yLast = yPos;
            isInitialized = true;
        }

        float sensitivity = 0.01f; //controls rotation senstivity

        //compute the displacement from the previous position
        float xDiff = xPos - xLast;
        float yDiff = yLast - yPos;

        //update the last position
        xLast = xPos;
        yLast = yPos;

        //update camera position and target based on camera movement
        position.x += xDiff * sensitivity;
        target.x += xDiff * sensitivity;

        position.z -= yDiff * sensitivity;
        target.z -= yDiff * sensitivity;
    }

};