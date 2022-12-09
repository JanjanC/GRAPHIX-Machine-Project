//add glad and glfw libaries
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//define tinyobjloader implementation
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

//define stbimage implementation
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//libraries to load shaders
#include <string>
#include <iostream>

//glm headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//width and height of the window
#define WIDTH 720.0f
#define HEIGHT 720.0f

// Model Class
#include "Classes/Models/Model.h"
#include "Classes/Models/Player.h"
#include "Classes/Models/Skybox.h"

// Camera Classes
#include "Classes/Cameras/PerspectiveCamera.h"
#include "Classes/Cameras/OrthoCamera.h"

// Light Classes
#include "Classes/Light/DirectionalLight.h"
#include "Classes/Light/SpotLight.h"

// Environment Class
#include "Classes/Environment.h"

//----------GLOBAL VARIABLES----------
Environment* environment; //pointer to the environment object

//----------CALLBACK FUNCTIONS----------
//callback function for key presses
void Key_Callback(GLFWwindow* window, int key, int scanCode, int action, int mods) {

    // player movement
    if ((key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D || key == GLFW_KEY_Q || key == GLFW_KEY_E) && action == GLFW_REPEAT) {
        // insert a flag for the camera being used e.g. camera is 1st person or 3rd person
        if (environment->activeCamera == environment->thirdPerspectiveCamera || environment->activeCamera == environment->firstPerspectiveCamera) {
            environment->playerModel->processKeyboard(key);
        }
        /* Birds-eye view */
        if (environment->activeCamera == environment->orthoCamera) {
            environment->orthoCamera->processKeyboard(key);
        }
    }

    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        environment->x += 0.1;
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        environment->x -= 0.1;
    }

    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        environment->y += 0.1;
    }

    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        environment->y -= 0.1;
    }

    // cycle the light intensity
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        environment->spotLight->processKeyboard(key);
    }

    // change the camera view
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        if (environment->activeCamera == environment->firstPerspectiveCamera) {
            environment->activeCamera = environment->thirdPerspectiveCamera;
            environment->lastPerspective = 3;
        }
        else
        if (environment->activeCamera == environment->thirdPerspectiveCamera) {
            environment->activeCamera = environment->firstPerspectiveCamera;
            environment->lastPerspective = 1;
        }
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        // toggle off - the current camera is in ortho already
        if (environment->activeCamera == environment->orthoCamera) {
            switch (environment->lastPerspective) {
                case 1:
                    environment->activeCamera = environment->firstPerspectiveCamera;
                    break;
                case 3:
                    environment->activeCamera = environment->thirdPerspectiveCamera;
                    break;
            }
        }
        // toggle on - save the last used perspective and switch the camera to ortho
        else {
            // set the position and target of ortho be on top of the player
            environment->orthoCamera->position.x = environment->playerModel->position.x;
            environment->orthoCamera->target.x = environment->playerModel->position.x;
            environment->orthoCamera->position.z = environment->playerModel->position.z + 0.1f; // 0.1f add to avoid looking straight down exactly
            environment->orthoCamera->target.z = environment->playerModel->position.z;

            // set the camera to switch to ortho
            environment->activeCamera = environment->orthoCamera;
        }
    }

    // escaping the game
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

//callback function for cursor movement
void Cursor_Callback(GLFWwindow* window, double xPos, double yPos) {
    //move the camera view for the third person perspective camera
    if (environment->activeCamera == environment->thirdPerspectiveCamera) {
        environment->thirdPerspectiveCamera->processMouse(xPos, yPos, environment->isMouseClicked);
    }

    //pan the camera view for the orthographic camera
    if (environment->activeCamera == environment->orthoCamera) {
        environment->orthoCamera->processMouse(xPos, yPos, environment->isMouseClicked);
    }
    
}

void Mouse_Button_Callback(GLFWwindow* window, int button, int action, int mods) {

    if (environment->activeCamera == environment->thirdPerspectiveCamera || environment->activeCamera == environment->orthoCamera) {

        //implement dragging using mouse
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            environment->isMouseClicked = true;
            //prevent the mouse from going out the window
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        //implement dragging using mouse
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            environment->isMouseClicked = false;
            //revert to normal cursor behavior
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

int main(void)
{
    GLFWwindow* window;

    //initialize the library
    if (!glfwInit())
        return -1;

    //create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(WIDTH, HEIGHT, "[Group 24] GRAPHIX Machine Project - No Man's Submarine", NULL, NULL);

    //terminate the program if a window is not created
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    //make the window's context current
    glfwMakeContextCurrent(window);

    //load the glad library
    gladLoadGL();

    //set callbacks for key presses and cursor movement
    glfwSetKeyCallback(window, Key_Callback);
    glfwSetCursorPosCallback(window, Cursor_Callback);
    glfwSetMouseButtonCallback(window, Mouse_Button_Callback);

    //create an environment object which stores the models, lights, shaders, and cameras
    environment = new Environment();

    //set the size of the viewport
    glViewport(0, 0, WIDTH, HEIGHT);

    //loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //update the uniform values in the shaders and draw the object on the screen
        environment->updateScreen();

        //swap front and back buffers
        glfwSwapBuffers(window);

        //poll for and process events
        glfwPollEvents();
    }

    delete environment; //deallocate the memory for environment

    glfwTerminate();

    return 0;
}
