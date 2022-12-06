#include "Light.h"
#pragma once

class DirectionalLight : public Light {

public:
    glm::vec3 lightDir; //stores the value of the light direction

    //constructor for the directional light class
    DirectionalLight(float ambientStr, float specStr, float specPhong, glm::vec3 lightColor, float lightIntensity, glm::vec3 lightDir) : Light(ambientStr, specStr, specPhong, lightColor, lightIntensity) {
        this->lightDir = lightDir;
    }

    //set the value of the ambient strength in the shader
    void setAmbientStr(Shader shader) override {
        shader.useProgram();

        //set the value of the ambient strength in the shader
        unsigned int ambientStrLoc = glGetUniformLocation(shader.shaderProgram, "directionalLight.ambientStr");
        glUniform1f(ambientStrLoc, ambientStr);
    }

    //set the value of the specular strength in the shader
    void setSpecStr(Shader shader) override {
        shader.useProgram();

        //set the value of the specular strength in the shader
        unsigned int specStrLoc = glGetUniformLocation(shader.shaderProgram, "directionalLight.specStr");
        glUniform1f(specStrLoc, specStr);

    }
    //set the value of the specular phong in the shader
    void setSpecPhong(Shader shader) override {
        shader.useProgram();

        //set the value of the specular phong in the shader
        unsigned int specPhongLoc = glGetUniformLocation(shader.shaderProgram, "directionalLight.specPhong");
        glUniform1f(specPhongLoc, specPhong);
    }

    //set the value of the light color in the shader
    void setLightColor(Shader shader) override {
        shader.useProgram();

        //set the value of the light color in the shader
        unsigned int lightColorLoc = glGetUniformLocation(shader.shaderProgram, "directionalLight.color");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    }

    //set the value of the light intensity in the shader
    void setLightIntensity(Shader shader) override {
        shader.useProgram();

        //set the value of the light intensity in the shader
        unsigned int lightIntensityLoc = glGetUniformLocation(shader.shaderProgram, "directionalLight.intensity");
        glUniform1f(lightIntensityLoc, lightIntensity);
    }

    //set the value of the light direction in the shader
    void setLightDirection(Shader shader) {
        shader.useProgram();

        //set the value of the light direction in the shader
        unsigned int lightDirLoc = glGetUniformLocation(shader.shaderProgram, "directionalLight.direction");
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
    }

    //process keyboard inputs and update the object attributes
    void processKeyboard(int key) {
        float sensitivity = 0.025;

        //increase the light intensity
        if (key == GLFW_KEY_RIGHT) {
            lightIntensity += sensitivity;
        }

        //decrease the light intensity
        if (key == GLFW_KEY_LEFT) {
            lightIntensity -= sensitivity;
            lightIntensity = glm::max(lightIntensity, 0.0f); //prevent negative intensity
        }
    }
};