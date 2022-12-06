#include "Shader.h"
#pragma once

//Model3D class stores the transformation properties of a model
class Model3D {
public:
    std::vector<GLfloat> fullVertexData; //contains the vertex data (vertex, normal, and texture coordinates)
    int attribCount; //the number of attributes in a set of vertex in fullVertexData
    GLuint VAO, VBO; //vao and vbo id of the model
    std::vector<GLuint> textures; //stores the list of textures used by the model
    glm::vec3 position, scale, theta; //stores the information to be used for transformation

    //constructor for the model class
    Model3D(std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 theta) {
        //sets the value of the class attributes
        this->position = position;
        this->scale = scale;
        this->theta = theta;

        //load the vertex attributes from the obj file to the fullVertexData array
        loadObject(modelPath);
    }

    //destructor for the model class
    ~Model3D() {
        //delete vertex arrays and buffers
        glDeleteVertexArrays(1, &VAO);
        glDeleteVertexArrays(1, &VBO);
    }

    //load the vertex attributes from the obj file
    void loadObject(std::string path) {
        std::vector<tinyobj::shape_t> shapes; //stores the shapes of the mesh
        std::vector<tinyobj::material_t> materials; //stores shapes of the mesh
        std::string warning, error; //stores warning or error messages
        tinyobj::attrib_t attributes; //stores mesh attributes

        //loads the mesh
        bool success = tinyobj::LoadObj(
            &attributes,
            &shapes,
            &materials,
            &warning,
            &error,
            path.c_str()
        );

        //determine if the obj file contains vertices, normals, and texture coordinates
        bool hasVertices = attributes.vertices.size() != 0;
        bool hasNormals = attributes.normals.size() != 0;
        bool hasTexCoords = attributes.texcoords.size() != 0;

        //calculate for the total number of attributes for a single vertex
        attribCount = hasVertices * 3 + hasNormals * 3 + hasTexCoords * 2;

        //process the vertex attributes
        for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
            tinyobj::index_t vData = shapes[0].mesh.indices[i];

            //process the vertices if it exists
            if (hasVertices) {
                int vertexIndex = vData.vertex_index * 3; //3 - xyz 

                //vertex x
                fullVertexData.push_back(
                    attributes.vertices[vertexIndex]
                );

                //vertex y
                fullVertexData.push_back(
                    attributes.vertices[vertexIndex + 1]
                );

                //vertex z
                fullVertexData.push_back(
                    attributes.vertices[vertexIndex + 2]
                );
            }

            //process the normals if it exists
            if (hasNormals) {
                int normalIndex = vData.normal_index * 3;

                //normal x
                fullVertexData.push_back(
                    attributes.normals[normalIndex]
                );

                //normal y
                fullVertexData.push_back(
                    attributes.normals[normalIndex + 1]
                );

                //normal z
                fullVertexData.push_back(
                    attributes.normals[normalIndex + 2]
                );
            }

            //process the texture coordinates if it exists
            if (hasTexCoords) {
                int uvIndex = vData.texcoord_index * 2; //2 - uv
                //texture u
                fullVertexData.push_back(
                    attributes.texcoords[uvIndex]
                );

                //texture v
                fullVertexData.push_back(
                    attributes.texcoords[uvIndex + 1]
                );
            }
        }

        //generate id for the vao and vbo;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        //binds the vao of the current model
        glBindVertexArray(VAO);

        //binds the vbo of the current model
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        //assign data to the vbo
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(GL_FLOAT) * fullVertexData.size(),
            fullVertexData.data(),
            GL_STATIC_DRAW
        );

        int attribOffset = 0; //determines the offset of the vertex attribute

        if (hasVertices) {
            GLintptr vertPtr = attribOffset * sizeof(GLfloat); //caculate for the offset of the vertex
            //assign vertex data to the vao
            glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                attribCount * sizeof(GL_FLOAT),
                (void*)vertPtr
            );
            glEnableVertexAttribArray(0); //enable vertex attribute 0

            attribOffset += 3;//update the offset by 3 (xyz)
        }

        if (hasNormals) {
            GLintptr normPtr = attribOffset * sizeof(GLfloat); //caculate for the offset of the normal
            //assign normal data to the vao
            glVertexAttribPointer(
                1,
                3,
                GL_FLOAT,
                GL_FALSE,
                attribCount * sizeof(GL_FLOAT),
                (void*)normPtr
            );
            glEnableVertexAttribArray(1); //enable vertex attribute 1

            attribOffset += 3; //update the offset by 3 (xyz)
        }

        if (hasTexCoords) {
            GLintptr uvPtr = attribOffset * sizeof(GLfloat); //caculate for the offset of the texture
            //assign texture data to the vao
            glVertexAttribPointer(
                2,
                2, //uv
                GL_FLOAT,
                GL_FALSE,
                attribCount * sizeof(GL_FLOAT),
                (void*)uvPtr
            );
            glEnableVertexAttribArray(2); //enable vertex attribute 2

            attribOffset += 2; //update the offset by 2 (uv)
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0); //finish modifying the buffer
        glBindVertexArray(0); //finish modifying the vao
    }

    void loadTexture(std::string path) {
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

        //assign the loaded texture
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB, //GL_RGB - jpgs / pngs without alpha, GL_RGBA - png or images with alpha
            img_width,
            img_height,
            0,
            GL_RGB,
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
    }

    //draws the model on the screen after applying the appropiate transformation
    void draw(Shader shader) {

        shader.useProgram();

        //use the vao of the model
        glBindVertexArray(VAO);

        //loads the texture(s) of the model
        for (int i = 0; i < textures.size(); i++) {
            //set the value of the tex0Address in the shader
            GLuint tex0Address = glGetUniformLocation(shader.shaderProgram, "tex0"); //get the
            glUniform1i(tex0Address, i); //texture at i

            glBindTexture(GL_TEXTURE_2D, textures[i]);
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

        //draw the model
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / attribCount);
    }
};