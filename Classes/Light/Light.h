#pragma once

class Light {

public:
    float ambientStr; //stores the value of the ambient strength
    float specStr; //stores the value of the specular strength
    float specPhong; //stores the value of the specular phong
    glm::vec3 lightColor; //stores the value of the light color
    float lightIntensity; //stores the value of the light intensity

    //constructor for the light class
    Light(float ambientStr, float specStr, float specPhong, glm::vec3 lightColor, float lightIntensity) {
        this->ambientStr = ambientStr;
        this->specStr = specStr;
        this->specPhong = specPhong;
        this->lightColor = lightColor;
        this->lightIntensity = lightIntensity;
    }

    //pure virtual function that sets the value of the ambient strength in the shader
    virtual void setAmbientStr(Shader shader) = 0;
    //pure virtual function that sets the value of the specular strength in the shader
    virtual void setSpecStr(Shader shader) = 0;
    //pure virtual function that sets the value of the specular phong in the shader
    virtual void setSpecPhong(Shader shader) = 0;
    //pure virtual function that sets the value of the light color in the shader
    virtual void setLightColor(Shader shader) = 0;
    //pure virtual function that sets the value of the light intensity in the shader
    virtual void setLightIntensity(Shader shader) = 0;

};