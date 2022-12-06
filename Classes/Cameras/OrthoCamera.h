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
        glm::mat4 projection_matrix = glm::ortho(-WIDTH / 100, WIDTH / 100, -HEIGHT / 100, HEIGHT / 100, -100.0f, 100.0f);

        //set the value of the projection matrix in the shader
        unsigned int projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    }
};