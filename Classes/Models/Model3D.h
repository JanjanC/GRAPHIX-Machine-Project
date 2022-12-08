#include "Shader.h"
#pragma once

//Model3D class stores the transformation properties of a model
class Model3D {
public:
    std::vector<GLfloat> fullVertexData; //contains the vertex data (vertex, normal, and texture coordinates)
    int attribCount; //the number of attributes in a set of vertex in fullVertexData
    GLuint VAO, VBO; //vao and vbo id of the model
    std::vector<GLuint> textures; //stores the list of textures used by the model
    std::vector<GLuint > textureAddresses;
    glm::vec3 position, scale, theta; //stores the information to be used for transformation
    int blend = 0;

    //constructor for the model class
    Model3D(std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 theta) {
        //sets the value of the class attributes
        this->position = position;
        this->scale = scale;
        this->theta = theta;
    }

    //destructor for the model class
    ~Model3D() {
        //delete vertex arrays and buffers
        glDeleteVertexArrays(1, &VAO);
        glDeleteVertexArrays(1, &VBO);
    }

    virtual void loadObject(std::string path) = 0;

    void loadTexture(std::string path, Shader shader, std::string textureName) {
        stbi_set_flip_vertically_on_load(true); //flip the texture

        int img_width, img_height, color_channels;
        //load the texture and fill out the variables
        unsigned char* tex_bytes = stbi_load(
            path.c_str(),
            &img_width,
            &img_height,
            &color_channels,
            0
        );

        //initialize textures
        GLuint texture;
        glGenTextures(1, &texture);
        //set the texture at index i as the active texture
        glActiveTexture(GL_TEXTURE0 + textures.size()); //we are modifying the texture at index 0 //set texture 1 too if you have 2 texture

        //bind the texture at index i
        glBindTexture(GL_TEXTURE_2D, texture);

        GLenum format;
        if (color_channels == 3) format = GL_RGB; 
        if (color_channels == 4) format = GL_RGBA;

        //assign the loaded texture
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            img_width,
            img_height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            tex_bytes
        );

        //generate mipmap
        glGenerateMipmap(GL_TEXTURE_2D);

        //free up the loaded bytes
        stbi_image_free(tex_bytes);

        //enable depth testing
        glEnable(GL_DEPTH_TEST);

        //insert to the list of textures
        textures.push_back(texture);


        textureAddresses.push_back(glGetUniformLocation(shader.shaderProgram, textureName.c_str())); //get the address of the texture name
    }

    //draws the model on the screen after applying the appropiate transformation
    void draw(Shader shader) {

        shader.useProgram();

        //use the vao of the model
        glBindVertexArray(VAO);

        //loads the texture(s) of the model
        for (int i = 0; i < textures.size() && i < textureAddresses.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            //set the value of the tex0Address in the shader
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glUniform1i(textureAddresses[i], i); //texture at i
        }

        //compute for the transformation matrix of the model
        glm::mat4 transformation_matrix = glm::mat4(1.0f);
        //translate the model
        transformation_matrix = glm::translate(transformation_matrix, position);
        //scale the model
        transformation_matrix = glm::scale(transformation_matrix, scale);
        //rotate the model along the x-axis
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta.x), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
        //rotate the model along the y-axis
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta.y), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
        //rotate the model along the z-axis
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta.z), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));

        //retrieve the location of the transform variable in the  shader
        unsigned int transformationLoc = glGetUniformLocation(shader.shaderProgram, "transform");
        //set the value of transform in the vertex shader
        glUniformMatrix4fv(transformationLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));
        unsigned int blendLoc = glGetUniformLocation(shader.shaderProgram, "blend");
        glUniform1i(blendLoc, blend);

        //draw the model
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / attribCount);
    }
};