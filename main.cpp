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
#define WIDTH 600.0f
#define HEIGHT 600.0f

/* Model Class */
#include "Classes/Models/Model.h"
#include "Classes/Models/Player.h"
#include "Classes/Models/Skybox.h"

/* Camera Classes */
#include "Classes/Cameras/PerspectiveCamera.h"
#include "Classes/Cameras/OrthoCamera.h"

/* Light Classes */
#include "Classes/Light/DirectionalLight.h"
#include "Classes/Light/SpotLight.h"

class Environment {

public:
    Player* mainModel;
    Model* otherModel;
    Skybox *skybox;
    SpotLight* spotLight;
    DirectionalLight* directionalLight;
    Shader* playerShader;
    Shader* modelShader;
    Shader* skyboxShader;
    PerspectiveCamera* thirdPerspectiveCamera;
    PerspectiveCamera* firstPerspectiveCamera;
    OrthoCamera* orthoCamera;
    MyCamera* activeCamera;
    int cameraIndex = 0;
    int lastPerspective = 3;

    //constructor for the environment class which initializes the objects necessary to render the program such as the models, lights, shaders, and cameras
    Environment() {

        //load the shader for the players
        playerShader = new Shader("Shaders/player.vert", "Shaders/player.frag");

        //load the shader for the models
        modelShader = new Shader("Shaders/model.vert", "Shaders/model.frag");

        //load the shader for the skybox
        skyboxShader = new Shader("Shaders/skybox.vert", "Shaders/skybox.frag");

        //load the main model and its textures
        //3D model taken from Free3D.com by user printable_models (link to creation: https://free3d.com/3d-model/bird-v1--875504.html)
        mainModel = new Player("3D/submarine.obj", glm::vec3(0, 0, 0), glm::vec3(0.001f, 0.001f, 0.001f), glm::vec3(0.0f, 0.0f, 0.0f));
        mainModel->loadTexture("3D/submarine_texture.png", *playerShader, "tex0");
        mainModel->loadTexture("3D/submarine_normal.png", *playerShader, "norm_tex");

        otherModel = new Model("3D/bird.obj", glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 0.0f, 0.0f));
        otherModel->loadTexture("3D/ayaya.png", *modelShader, "tex0");
        //otherModel->loadTexture("3D/bird_normal.jpg", *mainShader, "norm_tex");

        skybox = new Skybox("Skybox/uw_rt.jpg", "Skybox/uw_lf.jpg", "Skybox/uw_up.jpg", "Skybox/uw_dn.jpg", "Skybox/uw_ft.jpg", "Skybox/uw_bk.jpg");

        //create a point light and load the sphere object
        //3D model for sphere taken from the MIT website (http://web.mit.edu/djwendel/www/weblogo/shapes/basic-shapes/sphere/sphere.obj)
        spotLight = new SpotLight(0.5, 1.0f, 16.0f, glm::vec3(1, 1, 1), 0.5f, glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), 12.5);

        //create a directional light with a position of (4, 11, -3)
        directionalLight = new DirectionalLight(0.1f, 0.5f, 16.0f, glm::vec3(1, 1, 1), 1.0f, glm::vec3(0, -1, 0));

        //create a third person perspective camera
        thirdPerspectiveCamera = new PerspectiveCamera(mainModel->position - 5.0f * mainModel->direction, mainModel->position, glm::vec3(0, 1.0f, 0));

        //create a first person perspective camera
        firstPerspectiveCamera = new PerspectiveCamera(mainModel->position, mainModel->position + 5.0f * mainModel->direction, glm::vec3(0, 1.0f, 0));

        //create a orthographic camera
        orthoCamera = new OrthoCamera(glm::vec3(0.0f, 10.0f, -0.1f), glm::vec3(0, 0, 0), glm::vec3(0, 1.0f, 0));

        activeCamera = thirdPerspectiveCamera;

        /* print the initial info of the submarine */
        std::cout << "Submarine system initialization... COMPLETE\n";
        std::cout << "Preparing for underwater exploration...\n\n";
        std::cout << "[SUBMARINE STATUS]\n";
        std::cout << "Current ocean depth: " << mainModel->position.y;
    }
    
    //destructor for the environment class
    ~Environment() {
        //deallocates the objects from the memory
        delete mainModel;
        delete spotLight;
        delete directionalLight;
        delete playerShader;
        delete modelShader;
        delete skyboxShader;
        delete thirdPerspectiveCamera;
        delete firstPerspectiveCamera;
        delete orthoCamera;
    }

    //updates the uniform values of the shader files and draws the objects on the screen
    void updateScreen() {        
        firstPerspectiveCamera->updateFields(mainModel->position, mainModel->direction);
        thirdPerspectiveCamera->updateFields(mainModel->position);

        spotLight->updateFields(mainModel->position, mainModel->direction);

        updateShader(*playerShader);
        updateShader(*modelShader);

        skybox->setViewMatrix(*skyboxShader, activeCamera->viewMatrix);
        skybox->setProjectionMatrix(*skyboxShader, activeCamera->projectionMatrix);

        //draws the objects on the screens
        mainModel->draw(*playerShader);
        otherModel->draw(*modelShader);
        skybox->draw(*skyboxShader);
    }

    void updateShader(Shader shader) {

        //updates the uniform values
        activeCamera->setViewMatrix(shader);
        activeCamera->setProjectionMatrix(shader);
        activeCamera->setCameraPosition(shader);


        spotLight->setAmbientStr(shader);
        spotLight->setSpecStr(shader);
        spotLight->setSpecPhong(shader);
        spotLight->setLightColor(shader);
        spotLight->setLightIntensity(shader);
        spotLight->setLightPosition(shader);
        spotLight->setLightDirection(shader);
        spotLight->setAttenuationConstants(shader);
        spotLight->setCutoff(shader);

        directionalLight->setAmbientStr(shader);
        directionalLight->setSpecStr(shader);
        directionalLight->setSpecPhong(shader);
        directionalLight->setLightColor(shader);
        directionalLight->setLightIntensity(shader);
        directionalLight->setLightDirection(shader);
    }
};

//----------GLOBAL VARIABLES----------
Environment* environment; //pointer to the environment object

//----------CALLBACK FUNCTIONS----------
//callback function for key presses
void Key_Callback(GLFWwindow* window, int key, int scanCode, int action, int mods) {

    //model rotation and light movement
    /*
    if ((key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D || key == GLFW_KEY_Q || key == GLFW_KEY_E) && action == GLFW_REPEAT) {

        //rotate the point light when the point light is selected
        if (!environment->spotLight->isActive) {
            environment->mainModel->processKeyboard(key);
        }

        //rotate the main model when the point light is not selected
        if (environment->spotLight->isActive) {
            environment->spotLight->processKeyboard(key);
        }
    }
    */

    /* Player Control Movement */
    if ((key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D || key == GLFW_KEY_Q || key == GLFW_KEY_E) && action == GLFW_REPEAT) {
        /* Insert a flag for the camera being used e.g. camera is 1st person or 3rd person */
        if (environment->activeCamera == environment->thirdPerspectiveCamera || environment->activeCamera == environment->firstPerspectiveCamera) {
            environment->mainModel->processKeyboard(key);
        }
        /* Birds-eye view */
        if (environment->activeCamera == environment->orthoCamera) {
            environment->orthoCamera->processKeyboard(key);
        }
    }

    //cycle the light intensity
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        environment->spotLight->processKeyboard(key);
    }

    //change the camera view
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
        /* Toggle off - the current camera is in ortho already */
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
        /* Toggle on - save the last used perspective and switch the camera to ortho */
        else {
            /* Set the position and target of ortho be on top of the player */
            environment->orthoCamera->position.x = environment->mainModel->position.x;
            environment->orthoCamera->target.x = environment->mainModel->position.x;            
            environment->orthoCamera->position.z = environment->mainModel->position.z - 0.1f; // 0.1f subtraction to avoid looking straight down exactly
            environment->orthoCamera->target.z = environment->mainModel->position.z;

            /* Set the camera to switch to ortho */
            environment->activeCamera = environment->orthoCamera;
        }   
    }

    /* Escaping the game */
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

//callback function for cursor movement
void Mouse_Callback(GLFWwindow* window, double xPos, double yPos) {
    if (environment->activeCamera == environment->thirdPerspectiveCamera) {
        environment->thirdPerspectiveCamera->processMouse(xPos, yPos);
    }
}

int main(void)
{
    GLFWwindow* window;

    //initialize the library
    if (!glfwInit())
        return -1;

    //create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(WIDTH, HEIGHT, "Programming Challenge 2 - John Vincent Chua", NULL, NULL);

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
    glfwSetCursorPosCallback(window, Mouse_Callback);

    //prevent the mouse from going out the window
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
