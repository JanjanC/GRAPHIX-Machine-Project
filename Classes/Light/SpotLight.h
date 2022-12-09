#include "Light.h"
#pragma once

class SpotLight : public Light {

public:
    glm::vec3 position;
    glm::vec3 direction;
    float cutoff, outerCutoff;

    //fixed values for attenuation
    float constant;
    float linear;
    float quadratic;
    
    int intensityLevel;

    //constructor for the point light class
    SpotLight(float ambientStr, float specStr, float specPhong, glm::vec3 lightColor, float lightIntensity, glm::vec3 position, glm::vec3 direction, float cutoff, float outerCutoff) :
        Light(ambientStr, specStr, specPhong, lightColor, lightIntensity) {
        
        this->position = position;
        this->direction = direction;
        this->cutoff = cutoff;
        this->outerCutoff = outerCutoff;
        
        // values from https ://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
        constant = 1.0f;
        linear = 0.045f;
        quadratic = 0.0075f;

        intensityLevel = 0;
    }

    void updateFields(glm::vec3 position, glm::vec3 direction) {
        this->position = position;
        this->direction = direction;
    }

    //set the value of the ambient strength in the shader
    void setAmbientStr(Shader shader) override {
        shader.useProgram();

        //set the value of the ambient strength in the shader
        unsigned int ambientStrLoc = glGetUniformLocation(shader.shaderProgram, "spotLight.ambientStr");
        glUniform1f(ambientStrLoc, ambientStr);
    }

    //set the value of the specular strength in the shader
    void setSpecStr(Shader shader) override {
        shader.useProgram();

        //set the value of the specular strength in the shader
        unsigned int specStrLoc = glGetUniformLocation(shader.shaderProgram, "spotLight.specStr");
        glUniform1f(specStrLoc, specStr);
    }

    //set the value of the specular phong in the shader
    void setSpecPhong(Shader shader) override {
        shader.useProgram();

        //set the value of the specular phong in the shader
        unsigned int specPhongLoc = glGetUniformLocation(shader.shaderProgram, "spotLight.specPhong");
        glUniform1f(specPhongLoc, specPhong);
    }

    //set the value of the light color in the shader
    void setLightColor(Shader shader) override {
        shader.useProgram();

        //set the value of the light color in the shader
        unsigned int lightColorLoc = glGetUniformLocation(shader.shaderProgram, "spotLight.color");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    }

    //set the value of the light intensity in the shader
    void setLightIntensity(Shader shader) override {
        shader.useProgram();

        //set the value of the light intensity in the shader
        unsigned int lightIntensityLoc = glGetUniformLocation(shader.shaderProgram, "spotLight.intensity");
        glUniform1f(lightIntensityLoc, lightIntensity);
    }

    //set the value of the light position in the shader
    void setLightPosition(Shader shader) {
        shader.useProgram();

        //set the value of the light position in the shader
        unsigned int lightPosLoc = glGetUniformLocation(shader.shaderProgram, "spotLight.position");
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(position));
    }

    //set the value of the light direction in the shader
    void setLightDirection(Shader shader) {
        shader.useProgram();

        //set the value of the light direction in the shader
        unsigned int lightDirLoc = glGetUniformLocation(shader.shaderProgram, "spotLight.direction");
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(direction));
    }

    //set the value of the attenuation constants in the shader
    void setAttenuationConstants(Shader shader) {
        shader.useProgram();

        //set the value of the constant value
        unsigned int constantLoc = glGetUniformLocation(shader.shaderProgram, "spotLight.constant");
        glUniform1f(constantLoc, constant);

        //set the value of the linear value
        unsigned int linearLoc = glGetUniformLocation(shader.shaderProgram, "spotLight.linear");
        glUniform1f(linearLoc, linear);

        //set the value of the quadratic value
        unsigned int quadraticLoc = glGetUniformLocation(shader.shaderProgram, "spotLight.quadratic");
        glUniform1f(quadraticLoc, quadratic);
    }

    //set the value of the cutoff in the shader
    void setCutoff(Shader shader) {
        shader.useProgram();

        //set the value of the quadratic value
        unsigned int cutoffLoc = glGetUniformLocation(shader.shaderProgram, "spotLight.cutoff");
        glUniform1f(cutoffLoc, glm::cos(glm::radians(cutoff)));
    }

    //set the value of the cutoff in the shader
    void setOuterCutoff(Shader shader) {
        shader.useProgram();

        //set the value of the quadratic value
        unsigned int outerCutoffLoc = glGetUniformLocation(shader.shaderProgram, "spotLight.outerCutoff");
        glUniform1f(outerCutoffLoc, glm::cos(glm::radians(outerCutoff)));
    }

    //process keyboard inputs and update the object attributes
    void processKeyboard(int key) {

        float lightSensitivity = 0.025;
        //increase the light intensity
        if (key == GLFW_KEY_F) {
            intensityLevel += 1;
            lightIntensity = (intensityLevel % 3 + 1);
        }

    }

};