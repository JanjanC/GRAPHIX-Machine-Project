#include "MyCamera.h"
#pragma once

class PerspectiveCamera : public MyCamera {

public:
    float xLast, yLast; //stores the last cursor position
    bool isInitialized;//checks if the values of xLast and yLast have been initialized

    float distance; //distance from the camera to the target
    float pitch; //the rotation of the camera around the x axis
    float yaw; //the rotation of the camera around the y axis

    //constructor for the perspective camera class
    PerspectiveCamera(glm::vec3 position, glm::vec3 target, glm::vec3 up) : MyCamera(position, target, up) {
        xLast = yLast = 0.0f;
        isInitialized = false;

        //target = position + front;
        //front = target - position;

        distance = glm::length(position - target);

        pitch = 0.0f;
        yaw = -90.0f;
    }

    //set the value of the projection matrix in the shader
    void setProjectionMatrix(Shader shader) override {
        shader.useProgram();

        //compute for the projection matrix
        projectionMatrix = glm::perspective(
            glm::radians(45.0f),
            WIDTH / HEIGHT,
            0.1f,
            100.f
        );

        //set the value of the projection matrix in the shader
        unsigned int projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    }

    void updateFields(glm::vec3 position, glm::vec3 target) {
        this->position = position;
        this->target = target;
    }

    void updateFields(glm::vec3 position) {
        //recompute the position of the camera depending on the values of yaw and pitch
        glm::vec3 newPosition;
        newPosition.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)); //the x component is influenced by the x component of the yaw and pitch
        newPosition.y = sin(glm::radians(pitch)); //the y component is influenced by the y component of the pitch
        newPosition.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));//the z component is influenced by the y component of the yaw and the x component of the pitch

        newPosition = distance * glm::normalize(newPosition); //ensure that the distance between the camera and the target relative to the camera position is always the same

        this->position = position + newPosition;
        this->target = position;
    }

    //process the mouse inputs and updates the object attributes
    void processMouse(float xPos, float yPos) {
        //initialize the value of xLast and yLast
        if (!isInitialized) {
            xLast = xPos;
            yLast = yPos;
            isInitialized = true;
        }

        float sensitivity = 1.0f; //controls rotation senstivity

        //compute the displacement from the previous position
        float xDiff = xPos - xLast;
        float yDiff = yLast - yPos;

        //update the last position
        xLast = xPos;
        yLast = yPos;

        //updates the value of the pitch and yaw
        yaw += xDiff * sensitivity;
        pitch += yDiff * sensitivity;

        //prevent lookAt flip by limiting the rotation along the x-axis to be greater than -90 deegress
        if (pitch < -90.0f) {
            pitch = -89.99f;
        }

        //prevent lookAt flip by limiting the rotation along the x-axis to be less than 90 deegress
        if (pitch > 90.0f) {
            pitch = 89.99f;
        }
    }
};