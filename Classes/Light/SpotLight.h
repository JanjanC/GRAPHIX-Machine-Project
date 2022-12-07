#include "Light.h"
#pragma once

class SpotLight : public Light {

public:
    glm::vec3 position;
    glm::vec3 direction;
    float cutoff;

    //fixed values for attenuation
    float constant;
    float linear;
    float quadratic;

    glm::vec3 revolution; //stores the theta values of the revolution around the origin
    glm::vec3 initPosition; //stores the initial position of the point light
    bool isActive; //indiciate if the light is selected
    
    int intensityLevel;

    //constructor for the point light class
    SpotLight(float ambientStr, float specStr, float specPhong, glm::vec3 lightColor, float lightIntensity, glm::vec3 position, glm::vec3 direction, float cutoff) :
        Light(ambientStr, specStr, specPhong, lightColor, lightIntensity) {
        
        this->position = position;
        this->direction = direction;
        this->cutoff = cutoff;
        
        // values from https ://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
        constant = 1.0f;
        linear = 0.0014f;
        quadratic = 0.000007f;

        revolution = glm::vec3(0.0f, 0.0f, 0.0f);
        initPosition = position;

        isActive = false;
        intensityLevel = 0;
    }

    void updateAttributes(glm::vec3 position, glm::vec3 direction) {
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

    //process keyboard inputs and update the object attributes
    void processKeyboard(int key) {

        float revSensitivity = 5.0;

        //revolve around the x axis
        if (key == GLFW_KEY_W) {
            revolution.x -= revSensitivity;
        }

        if (key == GLFW_KEY_S) {
            revolution.x += revSensitivity;
        }

        //revolve around the y axis
        if (key == GLFW_KEY_A) {
            revolution.y -= revSensitivity;
        }

        if (key == GLFW_KEY_D) {
            revolution.y += revSensitivity;
        }

        //revolve around the z axis
        if (key == GLFW_KEY_Q) {
            revolution.z -= revSensitivity;
        }

        if (key == GLFW_KEY_E) {
            revolution.z += revSensitivity;
        }

        //calculate for the rotation of the point light
        glm::mat4 transformation_matrix = glm::mat4(1.0f);
        //rotate the point light along the x-axis
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(revolution.x), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
        //rotate the point light along the y-axis
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(revolution.y), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
        //rotate the point light along the z-axis
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(revolution.z), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
        //calculate for the new position of the matrix by applying the transformation matrix to the initial position of the point light
        position = transformation_matrix * glm::vec4(initPosition, 1.0);


        if (key == GLFW_KEY_SPACE) {
            isActive = !isActive; //select and deselect the point light
            //change the color to green (selected)
            if (isActive) {
                lightColor = glm::vec3(0.5f, 1.0f, 0.5f);
            }
            //change the color to white (not selected)
            else {
                lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
            }
        }

        float lightSensitivity = 0.025;
        //increase the light intensity
        if (key == GLFW_KEY_F) {
            intensityLevel += 1;
            lightIntensity = (intensityLevel % 3 + 1);
        }

    }

};