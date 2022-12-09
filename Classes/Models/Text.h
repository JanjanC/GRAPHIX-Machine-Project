#pragma once

//REFERENCE: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-11-2d-text/
//Note: the code from the tutorial was modified accordingly based on our use case

class Text {
public:
    GLuint VAO, VBO, texture;

    Text(std::string path) {
        initText2D(path);
    }

    void initText2D(std::string path) {

        //generate id for the vao and vbo;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

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

        glGenTextures(1, &texture);
        //set the texture at index i as the active texture
        glActiveTexture(GL_TEXTURE0); //we are modifying the texture at index 0 //set texture 1 too if you have 2 texture

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
    }

    void printText2D(Shader shader, std::string text, int x, int y, int size) {


        std::vector<GLfloat> fullVertexData;
        for (unsigned int i = 0; i < text.length(); i++) {

            //vertex (i.e., the boxes containing the text)
            glm::vec2 vertex_up_left = glm::vec2(x + i * size, y + size);
            glm::vec2 vertex_up_right = glm::vec2(x + i * size + size, y + size);
            glm::vec2 vertex_down_right = glm::vec2(x + i * size + size, y);
            glm::vec2 vertex_down_left = glm::vec2(x + i * size, y);

            //extract the character
            char character = text.at(i);
            float uv_x = (character % 16) / 16.0f;
            float uv_y = (character / 16) / 16.0f;

            glm::vec2 uv_up_left = glm::vec2(uv_x, 1.0f - uv_y);
            glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, 1.0f - uv_y);
            glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f, 1.0f - (uv_y + 1.0f / 16.0f));
            glm::vec2 uv_down_left = glm::vec2(uv_x, 1.0f - (uv_y + 1.0f / 16.0f));
            //triangle 1
            //upper left texture and uv
            fullVertexData.push_back(vertex_up_left.x);
            fullVertexData.push_back(vertex_up_left.y);
            fullVertexData.push_back(uv_up_left.x);
            fullVertexData.push_back(uv_up_left.y);

            //down left texture and uv
            fullVertexData.push_back(vertex_down_left.x);
            fullVertexData.push_back(vertex_down_left.y);
            fullVertexData.push_back(uv_down_left.x);
            fullVertexData.push_back(uv_down_left.y);

            //upper right texture and uv
            fullVertexData.push_back(vertex_up_right.x);
            fullVertexData.push_back(vertex_up_right.y);
            fullVertexData.push_back(uv_up_right.x);
            fullVertexData.push_back(uv_up_right.y);

            //triangle 2
            //down right texture and uv
            fullVertexData.push_back(vertex_down_right.x);
            fullVertexData.push_back(vertex_down_right.y);
            fullVertexData.push_back(uv_down_right.x);
            fullVertexData.push_back(uv_down_right.y);

            //upper right texture and uv
            fullVertexData.push_back(vertex_up_right.x);
            fullVertexData.push_back(vertex_up_right.y);
            fullVertexData.push_back(uv_up_right.x);
            fullVertexData.push_back(uv_up_right.y);

            //down left texture and uv
            fullVertexData.push_back(vertex_down_left.x);
            fullVertexData.push_back(vertex_down_left.y);
            fullVertexData.push_back(uv_down_left.x);
            fullVertexData.push_back(uv_down_left.y);
        }

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

        //vertex x, y
        glVertexAttribPointer(
            0,
            2,
            GL_FLOAT,
            GL_FALSE,
            4 * sizeof(GL_FLOAT),
            (void*)0
        );
        glEnableVertexAttribArray(0); //enable vertex attribute 0

        //texture coordinates u, v
        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            4 * sizeof(GL_FLOAT),
            (void*)2
        );
        glEnableVertexAttribArray(2); //enable vertex attribute 2

        glBindBuffer(GL_ARRAY_BUFFER, 0); //finish modifying the buffer
        glBindVertexArray(0); //finish modifying the vao

        shader.useProgram();

        glBindVertexArray(VAO);

        glActiveTexture(GL_TEXTURE0);
        //set the value of the tex0Address in the shader
        GLuint tex0Address = glGetUniformLocation(shader.shaderProgram, "tex0");
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(tex0Address, 0);

        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 4);
    }
};
