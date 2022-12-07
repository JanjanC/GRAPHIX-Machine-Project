#include "Shader.h"
#pragma once

//Model3D class stores the transformation properties of a model
class Skybox {
public:
    std::vector<std::string> faces; //contains the vertex data (vertex, normal, and texture coordinates)
    GLuint VAO, VBO, EBO, texture; //vao, vbo, ebo, and texture id of the model

    //constructor for the model class
    Skybox(std::string skyboxRt, std::string skyboxLf, std::string skyboxUp, std::string skyboxDn, std::string skyboxFt, std::string skyboxBk) {
        //sets the value of the class attributes
        faces = { skyboxRt, skyboxLf, skyboxUp, skyboxDn, skyboxFt, skyboxBk };

        loadObject();
    }

    //destructor for the model class
    ~Skybox() {
        //delete vertex arrays and buffers
        glDeleteVertexArrays(1, &VAO);
        glDeleteVertexArrays(1, &VBO);
        glDeleteVertexArrays(1, &EBO);
    }

    //load the vertex attributes from the obj file
    void loadObject() {
        //vertices for the skybox cube
        float skyboxVertices[]{
            -1.f, -1.f, 1.f, //0
            1.f, -1.f, 1.f,  //1
            1.f, -1.f, -1.f, //2
            -1.f, -1.f, -1.f,//3
            -1.f, 1.f, 1.f,  //4
            1.f, 1.f, 1.f,   //5
            1.f, 1.f, -1.f,  //6
            -1.f, 1.f, -1.f  //7
        };

        //indices for the skybox cube
        unsigned int skyboxIndices[]{
            1,2,6,
            6,5,1,

            0,4,7,
            7,3,0,

            4,5,6,
            6,7,4,

            0,3,2,
            2,1,0,

            0,1,5,
            5,4,0,

            3,7,6,
            6,2,3
        };

        //generate id for the vao, vbo, and ebo
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        //binds the vao of the skybox
        glBindVertexArray(VAO);

        //binds the vbo of the skybox
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        //assign data to the vbo
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

        //assign data to the vao
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
        glEnableVertexAttribArray(0); //enable vertex attribute 0

        //binds the ebo of the skybox
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skyboxIndices, GL_STATIC_DRAW);

        glBindVertexArray(0); //finish modifying the vao
        glBindBuffer(GL_ARRAY_BUFFER, 0); //finish modifying the vbo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //finish modifying the ebo

        //initialize textures
        glGenTextures(1, &texture);
        //initialize the texture as a cubemap
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        //prevent pixelating
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //prevent tiling
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        for (unsigned int i = 0; i < 6; i++) {

            //temporarily disable image flipping
            stbi_set_flip_vertically_on_load(false);

            int img_width, img_height, color_channels;
            //load the texture and fill out the variables
            unsigned char* tex_bytes = stbi_load(
                faces[i].c_str(),
                &img_width,
                &img_height,
                &color_channels,
                0
            );

            //if texture is loaded properly
            if (tex_bytes) {
                //assign the loaded texture
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    GL_RGB,
                    img_width,
                    img_height,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    tex_bytes
                );

                //free up the loaded bytes
                stbi_image_free(tex_bytes);
            }
        }

        //reenable image flipping
        stbi_set_flip_vertically_on_load(true);
    }

    //set the value of the projection matrix in the shader
    void setProjectionMatrix(Shader shader, glm::mat4 projectionMatrix) {
        shader.useProgram();
        //set the value of the projection matrix in the shader
        unsigned int projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    }

    // set the value of the view matrix in the shader
    void setViewMatrix(Shader shader, glm::mat4 viewMatrix) {
        shader.useProgram();

        //remove translations
        glm::mat4 sky_view = glm::mat4(1.f);
        sky_view = glm::mat4(glm::mat3(viewMatrix));

        float scale_x, scale_y, scale_z;
        scale_x = scale_y = scale_z = HEIGHT/50 + 2.0f;

        sky_view = glm::scale(sky_view, glm::vec3(scale_x, scale_y, scale_z));

        // set the value of the view matrix in the shader
        unsigned int viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(sky_view));
    }

    void draw(Shader shader) {

        shader.useProgram();
        
        //disable depth mask
        glDepthMask(GL_FALSE);

        //change the depth function to <=
        glDepthFunc(GL_LEQUAL);

        //use the vao of the skybox
        glBindVertexArray(VAO);

        //set the active texture index
        glActiveTexture(GL_TEXTURE0);
        //bind the cubemap to the texture index
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        //draw the skybox
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //reset depth testing to true
        glDepthMask(GL_TRUE);

        //reset depth function to normal
        glDepthFunc(GL_LESS);
    }
};