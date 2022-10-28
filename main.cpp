#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <string>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float mod_x = 0;
float mod_y = 0;
float mod_z = 0;
void Key_Callback(GLFWwindow* window, 
    int key, //key code
    int scanCode, //scan code
    int action,  //press/release
    int mods //modifier keys (e.g., shift, alt, ctrl)
) {

    if (key == GLFW_KEY_A &&
        action == GLFW_PRESS) {
        mod_x -= 0.5f;
    }

    if (key == GLFW_KEY_D &&
        action == GLFW_PRESS) {
        mod_x += 0.5f;
    }

    if (key == GLFW_KEY_S &&
        action == GLFW_PRESS) {
        mod_y -= 0.5f;
    }

    if (key == GLFW_KEY_W &&
        action == GLFW_PRESS) {
        mod_y += 0.5f;
    }

    if (key == GLFW_KEY_Q &&
        action == GLFW_PRESS) {
        mod_z -= 0.5f;
    }

    if (key == GLFW_KEY_E &&
        action == GLFW_PRESS) {
        mod_z += 0.5f;
    }
}


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    float screenWidth = 600.0f;
    float screenHeight = 600.0f;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screenWidth, screenHeight, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    // Initialize GLAD
    gladLoadGL(); // comes after glfwMakeContextCurrent

    stbi_set_flip_vertically_on_load(true);
    int img_width, img_height, color_channels;
    unsigned char* tex_bytes = stbi_load(
        "3D/partenza.jpg",
        &img_width,
        &img_height,
        &color_channels,
        0);

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0); //we are modifying the texture at index 0 //set texture 1 too if you have 2 texture
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB, //GL_RGB - jpegs / pngs without alpha, GL_RGBA - png or images with alpha
        img_width,
        img_height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE, //what type is the image
        tex_bytes
    );

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, screenWidth, screenHeight); //edit screen space (comes after gladLoadGL())

    glfwSetKeyCallback(window, Key_Callback);

    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();
    std::string vertString = vertBuff.str();
    const char* v = vertString.c_str();

    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragString = fragBuff.str();
    const char* f = fragString.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    std::string path = "3D/djSword.obj";
    std::vector<tinyobj::shape_t> shapes; //how many shapes
    std::vector<tinyobj::material_t> materials;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    GLfloat UV[]{
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        0.f, 0.f
    };

    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &materials,
        &warning,
        &error,
        path.c_str()
    );

    std::vector<GLuint> mesh_indices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(
            shapes[0].mesh.indices[i].vertex_index
        );
    }

    std::vector<GLfloat> fullVertexData;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        //x
        int vertexIndex = vData.vertex_index * 3; //3 - xyz 
        int normalIndex = vData.normal_index * 3;
        int uvIndex = vData.texcoord_index * 2; //2 - uv

        fullVertexData.push_back(
            attributes.vertices[vertexIndex]
        );

        //y
        fullVertexData.push_back(
            attributes.vertices[vertexIndex + 1]
        );

        //z
        fullVertexData.push_back(
            attributes.vertices[vertexIndex + 2]
        );

        fullVertexData.push_back(
            attributes.normals[normalIndex]
        );

        fullVertexData.push_back(
            attributes.normals[normalIndex + 1]
        );

        fullVertexData.push_back(
            attributes.normals[normalIndex + 2]
        );

        fullVertexData.push_back(
            attributes.texcoords[uvIndex]
        );

        fullVertexData.push_back(
            attributes.texcoords[uvIndex + 1]
        );
    }

    GLfloat vertices[]{ //x, y, z
        0.f, 0.5f, 0.f, //vertex 0
        -0.5f, -0.5f, 0.f, //vertex 1
        0.5f, -0.5f, 0.f //vertex
    };

    //EBO
    GLuint indices[]{
        0, 1, 2 //triangle 1
    };

    //initialize VAO, VBO, EBO
    GLuint VAO, VBO;// EBO, VBO_UV; 
    //generate and assign id to VAO
    glGenVertexArrays(1, &VAO); 
    //generate and assign id to VBO
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &VBO_UV);
    //generate and assign id to EBO
    //glGenBuffers(1, &EBO);

    glBindVertexArray(VAO); //tell OpenGL we are working on VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //assign VBO to VAO, convert to GL_ARRAY_BUFFER
    glBufferData( //assign data to VBO
        GL_ARRAY_BUFFER,
        //sizeof(vertices),
        //sizeof(vertices),
        sizeof(GL_FLOAT) * fullVertexData.size(), //sizeof(GL_FLOAT) * attributes.vertices.size()
        //vertices,
        //&attributes.vertices[0],
        fullVertexData.data(),
        GL_STATIC_DRAW
    );

    //tell the VAO how to interpret the VBO
    glVertexAttribPointer(
        0, //position
        3, //x, y, z
        GL_FLOAT, //type of array
        GL_FALSE, //should we normalize?
        8 * sizeof(GL_FLOAT), //size of the vertex data  5 * sizeof(GL_FLOAT)
        (void*) 0
    );

    GLintptr normPtr = 3 * sizeof(GLfloat);
    glVertexAttribPointer(
        1, //position
        3, //x, y, z
        GL_FLOAT, //type of array
        GL_FALSE, //should we normalize?
        8 * sizeof(GL_FLOAT), //size of the vertex data  5 * sizeof(GL_FLOAT)
        (void*)normPtr
    );

    GLintptr uvPtr = 6 * sizeof(GLfloat);
    glVertexAttribPointer(
        2,
        2, //uv
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)uvPtr
    );


    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //we are working on EBO
    //glBufferData(
    //    GL_ELEMENT_ARRAY_BUFFER,
    //    //sizeof(indices),
    //    sizeof(GLuint) * mesh_indices.size(),
    //    //indices,
    //    mesh_indices.data(),
    //    GL_STATIC_DRAW
    //);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
    //glBufferData(
    //    GL_ARRAY_BUFFER,
    //    sizeof(GL_FLOAT) * (sizeof(UV) / sizeof(UV[0])),
    //    &UV[0],
    //    GL_STATIC_DRAW
    //);

    //glVertexAttribPointer(
    //    2, //Tex Coords / UV index = 2.
    //    2, //U and V
    //    GL_FLOAT,
    //    GL_FALSE,
    //    2 * sizeof(GL_FLOAT), //2 -> u and v
    //    (void*)0
    //);


    //0 Position of the vertex
    //1 Normals
    //2 Textures
    glEnableVertexAttribArray(0); //enable once glVertexAttrib is used
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2); //bec we used index 2

    glBindBuffer(GL_ARRAY_BUFFER, 0); //done modifying buffer
    glBindVertexArray(0); //done modifying the VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //create 3x3 identity matrix
    glm::mat3 identity_matrix3 = glm::mat3(1.0f);
    //create 4x4 identity matrix
    glm::mat4 identity_matrix = glm::mat4(1.0f);

    float x, y, z;
    x = y = z = 0.0f; //0.0 no movement
    //glm::mat4 translation = glm::translate(identity_matrix, glm::vec3(x, y, z));

    z = -1.f;

    float scale_x, scale_y, scale_z;
    scale_x = scale_y = scale_z = 0.1f; //1.0 = original size
    //glm::mat4 scale = glm::scale(identity_matrix, glm::vec3(scale_x, scale_y, scale_z));

    float rot_x, rot_y, rot_z;
    rot_x = rot_y = rot_z = 0; //axis of rotation
    rot_y = 1.0f; //should not all be zero

    float theta = 90.0f;

    //the bunny will be half the size because the default value was changed from 1.0 to 2.0 
    //note: the view was changed not the actual size
    //larger view -> smaller objects
                                             //L to R     B to T       ZNear to ZFar != 0
    //glm::mat4 projection_matrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -10.0f, 10.0f); //L < R, B < T, N < F

    glm::mat4 projection_matrix = glm::perspective(
        glm::radians(60.f), //fov in degrees
        screenHeight / screenWidth, //aspect ratio
        0.1f, //zNear
        100.f //zFar
    );

    //glm::vec3 lightPos = glm::vec3(0, 0, 0);
    glm::vec3 lightColor = glm::vec3(1, 1, 1);

    float ambientStr = 0.1;
    glm::vec3 ambientColor = glm::vec3(1, 1, 1);

    float specStr = 1.0f; //normally less than 1
    float specPhong = 16.0f;

    //normalized vector = magntitude / length = 1
    //glm::mat4 rotation = glm::rotate(identity_matrix, glm::radians(theta), glm::normalize(glm::vec3(rot_x, rot_y, rot_z)));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        theta += 0.01f;
        //mod_x += 0.001f;

        glm::vec3 lightPos = glm::vec3(mod_x, mod_y, mod_z);

        //unsigned int xLoc = glGetUniformLocation(shaderProgram, "x");
        //glUniform1f(xLoc, mod_x);

        glm::vec3 cameraPos = glm::vec3(0, 0, 10.0f); //position of the camera in the world (10 units towards me)
        glm::mat4 cameraPosMatrix = glm::translate(glm::mat4(1.0f), cameraPos * -1.0f);

        glm::vec3 worldUp = glm::vec3(0, 1.0f, 0);
        glm::vec3 cameraCenter = glm::vec3(0, 0, 0);

        glm::vec3 F = glm::vec3(cameraCenter - cameraPos);
        F = glm::normalize(F);

        glm::vec3 R = glm::cross(F, worldUp);
        R = glm::normalize(R);

        glm::vec3 U = glm::cross(R, F);
        U = glm::normalize(U);

        glm::mat4 cameraOrientationMatrix = glm::mat4(1.0f);
        
        //Matrix[Col][Row]
        cameraOrientationMatrix[0][0] = R.x;
        cameraOrientationMatrix[1][0] = R.y;
        cameraOrientationMatrix[2][0] = R.z;

        cameraOrientationMatrix[0][1] = U.x;
        cameraOrientationMatrix[1][1] = U.y;
        cameraOrientationMatrix[2][1] = U.z;

        cameraOrientationMatrix[0][2] = -F.x;
        cameraOrientationMatrix[1][2] = -F.y;
        cameraOrientationMatrix[2][2] = -F.z;
        

        glm::mat4 viewMatrix = cameraOrientationMatrix * cameraPosMatrix;
        //glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraCenter, worldUp);

        glm::mat4 transformation_matrix = glm::mat4(1.0f);
        //translation
        transformation_matrix = glm::translate(transformation_matrix, glm::vec3(x, y, z));
        //scale
        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale_x, scale_y, scale_z));
        //translation
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta), glm::normalize(glm::vec3(rot_x, rot_y, rot_z)));

        GLuint tex0Address = glGetUniformLocation(shaderProgram, "tex0");
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(tex0Address, 0);

        unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

        unsigned int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

        unsigned int ambientStrLoc = glGetUniformLocation(shaderProgram, "ambientStr");
        glUniform1f(ambientStrLoc, ambientStr);

        unsigned int ambientColorLoc = glGetUniformLocation(shaderProgram, "ambientColor");
        glUniform3fv(ambientColorLoc, 1, glm::value_ptr(ambientColor));

        unsigned int cameraPosLoc = glGetUniformLocation(shaderProgram, "cameraPos");
        glUniform3fv(cameraPosLoc, 1, glm::value_ptr(cameraPos));

        unsigned int specStrLoc = glGetUniformLocation(shaderProgram, "specStr");
        glUniform1f(specStrLoc, specStr);

        unsigned int specPhongLoc = glGetUniformLocation(shaderProgram, "specPhong");
        glUniform1f(specPhongLoc, specPhong);



        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        unsigned int transformationLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformationLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

        glUseProgram(shaderProgram); //the next actions will use shaderProgram

        glBindVertexArray(VAO); // the next instructions will use the VAO

        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0); //EBOs allows us to optimize the vertex count
        //glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);

        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8);


        transformation_matrix = glm::mat4(1.0f);
        transformation_matrix = glm::translate(transformation_matrix, glm::vec3(3, y, z));
        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale_x, scale_y, scale_z));
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta), glm::normalize(glm::vec3(rot_x, rot_y, rot_z)));
        glUniformMatrix4fv(transformationLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8);


        transformation_matrix = glm::mat4(1.0f);
        transformation_matrix = glm::translate(transformation_matrix, glm::vec3(-3, y, z));
        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale_x, scale_y, scale_z));
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta), glm::normalize(glm::vec3(rot_x, rot_y, rot_z)));
        glUniformMatrix4fv(transformationLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8);


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}