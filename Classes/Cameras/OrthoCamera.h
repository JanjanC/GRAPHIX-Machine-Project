#include "MyCamera.h"
#pragma once

class OrthoCamera : public MyCamera {
public:

    //constructor for the orthographic camera class
    OrthoCamera(glm::vec3 position, glm::vec3 target, glm::vec3 up) : MyCamera(position, target, up) { }

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

        /*Traverse Forward*/
        if (key == GLFW_KEY_W) {           
            position.z -= sensitivity;
            target.z -= sensitivity;
        }
        /*Traverse Backward*/
        if (key == GLFW_KEY_S) {
            position.z += sensitivity;
            target.z += sensitivity;
        }
        /*Rotate to left*/
        if (key == GLFW_KEY_A) {
            position.x -= sensitivity;
            target.x -= sensitivity;
        }
        /*Rotate to right*/
        if (key == GLFW_KEY_D) {
            position.x += sensitivity;
            target.x += sensitivity;
        }
    }
};