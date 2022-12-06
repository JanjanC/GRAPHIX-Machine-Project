#pragma once

class MyCamera {

public:
    glm::vec3 position; //position of the camera in the world
    glm::vec3 target; //position of the target in the world
    glm::vec3 up; //the world up vector
    float distance; //distance from the camera to the target

    //constructor for the camera class
    MyCamera(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
        this->position = position;
        this->target = target;
        this->up = up;
        distance = glm::length(target - position); //computes for the distance from the camera to its target
    }

    //set the value of the view matrix in the shader
    void setViewMatrix(Shader shader) {
        shader.useProgram();

        //computes for the view matrix
        glm::mat4 viewMatrix = glm::lookAt(position, target, up);

        //set the value of the view matrix in the shader
        unsigned int viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    }
    //set the value of the cameraPos in the shader
    void setCameraPosition(Shader shader) {
        shader.useProgram();

        //set the value of the cameraPos in the shader
        unsigned int cameraPosLoc = glGetUniformLocation(shader.shaderProgram, "cameraPos");
        glUniform3fv(cameraPosLoc, 1, glm::value_ptr(position));
    }

    //pure virtual function that sets the value of the projection matrix in the shader
    virtual void setProjectionMatrix(Shader shader) = 0;
};