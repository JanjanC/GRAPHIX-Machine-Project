#include "Model3D.h"
#pragma once

class Player : public Model3D {
public:

    glm::vec3 direction;

    //constructor for the main model class
    Player(std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 theta) : Model3D(modelPath, position, scale, theta) {
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

        std::vector<glm::vec3> tangents; //collection of tangent
        std::vector<glm::vec3> bitangents; //collection of tangent

        //calculate for the tangent and bitangent
        for (int i = 0; i < shapes[0].mesh.indices.size(); i += 3) {
            //vertices of the triangle
            tinyobj::index_t vData1 = shapes[0].mesh.indices[i]; //v1
            tinyobj::index_t vData2 = shapes[0].mesh.indices[i + 1]; //v2
            tinyobj::index_t vData3 = shapes[0].mesh.indices[i + 2]; //v3

            //xyz components of vertex 1
            glm::vec3 v1 = glm::vec3(
                attributes.vertices[vData1.vertex_index * 3],
                attributes.vertices[vData1.vertex_index * 3 + 1],
                attributes.vertices[vData1.vertex_index * 3 + 2]
            );

            //xyz components of vertex 2
            glm::vec3 v2 = glm::vec3(
                attributes.vertices[vData2.vertex_index * 3],
                attributes.vertices[vData2.vertex_index * 3 + 1],
                attributes.vertices[vData2.vertex_index * 3 + 2]
            );

            //xyz components of vertex 3
            glm::vec3 v3 = glm::vec3(
                attributes.vertices[vData3.vertex_index * 3],
                attributes.vertices[vData3.vertex_index * 3 + 1],
                attributes.vertices[vData3.vertex_index * 3 + 2]
            );

            //uv components of vertex 1
            glm::vec2 uv1 = glm::vec2(
                attributes.texcoords[vData1.texcoord_index * 2],
                attributes.texcoords[vData1.texcoord_index * 2 + 1]
            );

            //uv components of vertex 2
            glm::vec2 uv2 = glm::vec2(
                attributes.texcoords[vData2.texcoord_index * 2],
                attributes.texcoords[vData2.texcoord_index * 2 + 1]
            );

            //uv components of vertex 3
            glm::vec2 uv3 = glm::vec2(
                attributes.texcoords[vData3.texcoord_index * 2],
                attributes.texcoords[vData3.texcoord_index * 2 + 1]
            );

            //edges of the triangle
            glm::vec3 deltaPos1 = v2 - v1;
            glm::vec3 deltaPos2 = v3 - v1;

            //uv delta
            glm::vec2 deltaUV1 = uv2 - uv1;
            glm::vec2 deltaUV2 = uv3 - uv1;

            float r = 1.f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));
            //tangent
            glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
            //bitangent
            glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

            //push the tangent and bitangent 3 times for the 3 vertices of the triangle
            tangents.push_back(tangent);
            tangents.push_back(tangent);
            tangents.push_back(tangent);

            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
        }

        //determine if the obj file contains vertices, normals, and texture coordinates
        bool hasVertices = attributes.vertices.size() != 0;
        bool hasNormals = attributes.normals.size() != 0;
        bool hasTexCoords = attributes.texcoords.size() != 0;
        bool hasTangents = tangents.size() != 0;
        bool hasBitangents = bitangents.size() != 0;

        //calculate for the total number of attributes for a single vertex
        attribCount = hasVertices * 3 + hasNormals * 3 + hasTexCoords * 2 + hasTangents * 3 + hasBitangents * 3;

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

            if (hasTangents) {
                //tangent x
                fullVertexData.push_back(
                    tangents[i].x
                );

                //tangent y
                fullVertexData.push_back(
                    tangents[i].y
                );

                //tangent z
                fullVertexData.push_back(
                    tangents[i].z
                );
            }

            if (hasBitangents) {
                //bitangent x
                fullVertexData.push_back(
                    bitangents[i].x
                );

                // bitangent y
                fullVertexData.push_back(
                    bitangents[i].y
                );

                // bitangent z
                fullVertexData.push_back(
                    bitangents[i].z
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

        if (hasTangents) {
            GLintptr tangentPtr = attribOffset * sizeof(GLfloat); //caculate for the offset of the tangent
            //assign tangent data to the vao
            glVertexAttribPointer(
                3,
                3,
                GL_FLOAT,
                GL_FALSE,
                attribCount * sizeof(GL_FLOAT),
                (void*)tangentPtr
            );

            glEnableVertexAttribArray(3); //enable vertex attribute 3

            attribOffset += 3; //update the offset by 2 (xyz)
        }

        if (hasBitangents) {
            GLintptr bitangentPtr = attribOffset * sizeof(GLfloat); //caculate for the offset of the bitangent
            //assign bitangent data to the vao
            glVertexAttribPointer(
                4,
                3,
                GL_FLOAT,
                GL_FALSE,
                attribCount * sizeof(GL_FLOAT),
                (void*)bitangentPtr
            );

            glEnableVertexAttribArray(4); //enable vertex attribute 4

            attribOffset += 3; //update the offset by 2 (xyz)
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