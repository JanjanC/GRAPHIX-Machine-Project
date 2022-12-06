#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma once

class Shader {
public:
    GLuint shaderProgram; //id of the shader

    //constructor for the shader class with the path to the vertex and fragment files as parameters
    Shader(std::string vertPath, std::string fragPath) {
        //load vertex shader file
        std::fstream vertSrc(vertPath);
        std::stringstream vertBuff;
        vertBuff << vertSrc.rdbuf(); //add the file stream to the string stream
        std::string vertString = vertBuff.str(); //convert stream to a character array
        const char* v = vertString.c_str();

        //load fragment shader file
        std::fstream fragSrc(fragPath);
        std::stringstream fragBuff;
        fragBuff << fragSrc.rdbuf(); //add the file stream to the string stream
        std::string fragString = fragBuff.str(); //convert stream to a character array
        const char* f = fragString.c_str();

        //create a vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &v, NULL); //assign the source to the vertex shader
        glCompileShader(vertexShader); //compile the vertex shader

        //create a fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &f, NULL);  //assign the source to the fragment shader
        glCompileShader(fragmentShader); //compile the fragment shader

        //create the shader program
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader); //attach the compiled vertex shader
        glAttachShader(shaderProgram, fragmentShader); //attach the compiled fragment shader

        //finalize the compilation
        glLinkProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    //loads the current program as the shader
    void useProgram() {
        glUseProgram(shaderProgram);
    }
};