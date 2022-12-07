#include "Model3D.h"
#pragma once

class Model : public Model3D {
public:

    glm::vec3 direction;

    //constructor for the main model class
    Model(std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 theta) : Model3D(modelPath, position, scale, theta) {
        direction = glm::normalize(glm::vec3(sin(glm::radians(theta.y)), 0, cos(glm::radians(theta.y))));
        loadObject(modelPath);
    }

    //load the vertex attributes from the obj file
    void loadObject(std::string path) override {
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

    //process keyboard inputs and update the object attributes
    void processKeyboard(int key) {
        float rot_sensitivity = 1.0f;
        float vert_sensitivity = 0.2f;
        float horz_sensitivity = 0.4f;

        //update the direction of the model
        direction = glm::normalize(glm::vec3(sin(glm::radians(theta.y)), 0, cos(glm::radians(theta.y))));

        if (key == GLFW_KEY_Q || key == GLFW_KEY_E) {
            /*Elevate*/
            if (key == GLFW_KEY_Q && position.y) {
                position.y = glm::min(position.y + vert_sensitivity, 0.0f);
            }
            /*Descend*/
            if (key == GLFW_KEY_E) {
                position.y -= vert_sensitivity;
            }
            /*Update depth info*/
            printf("\rCurrent ocean depth: %.2f ", position.y);
        }

        /*Traverse Forward*/
        if (key == GLFW_KEY_W) {
            position += direction * horz_sensitivity;
        }
        /*Traverse Backward*/
        if (key == GLFW_KEY_S) {
            position -= direction * horz_sensitivity;
        }
        /*Rotate to left*/
        if (key == GLFW_KEY_A) {
            theta.y += rot_sensitivity + 1.0;
        }
        /*Rotate to right*/
        if (key == GLFW_KEY_D) {
            theta.y -= rot_sensitivity + 1.0;
        }        
    }
};