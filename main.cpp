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

class Environment {

public:
    Player* playerModel;
    std::vector<Model*> otherModels;
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
    int lastPerspective = 3;
    bool isMouseClicked = false;

    Player* testPlane;

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
        playerModel = new Player("3D/submarine.obj", glm::vec3(0, -10, 0), glm::vec3(0.00375f, 0.00375f, 0.00375f), glm::vec3(0.0f, 180.0f, 0.0f));
        playerModel->loadTexture("3D/submarine_texture.png", *playerShader, "tex0");
        playerModel->loadTexture("3D/submarine_normal.png", *playerShader, "norm_tex");

        Model* model;
        //load the megalodon model and its textures
        model = new Model("3D/megalodon.obj", glm::vec3(40.0f, -30.0f, -75.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(-25.0f, 225.0f, -25.0f));
        model->loadTexture("3D/megalodon_texture.png", *modelShader, "tex0");
        otherModels.push_back(model);

        model = new Model("3D/turtle.obj", glm::vec3(0.0f, -30.0f, -100.0f), glm::vec3(0.03f, 0.03f, 0.03f), glm::vec3(-25.0f, 225.0f, 0.0f));
        model->loadTexture("3D/turtle_texture.png", *modelShader, "tex0");
        otherModels.push_back(model);

        model = new Model("3D/enemy_submarine.obj", glm::vec3(40.0f, -80.0f, -20.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(45.0f, 45.0f, 0.0f));
        model->loadTexture("3D/enemy_submarine_texture.png", *modelShader, "tex0");
        otherModels.push_back(model);

        //load the seahore model and its textures
        model = new Model("3D/seahorse.obj", glm::vec3(-45.0f, -20.0f, -75.0f), glm::vec3(0.03f, 0.03f, 0.03f), glm::vec3(0.0f, 25.0f, 0.0f));
        model->loadTexture("3D/seahorse_texture.png", *modelShader, "tex0");
        otherModels.push_back(model);

        model = new Model("3D/starfish.obj", glm::vec3(0.0f, -5.0f, -50.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 25.0f, 25.0f));
        model->loadTexture("3D/starfish_texture.png", *modelShader, "tex0");
        otherModels.push_back(model);
        
        model = new Model("3D/koi.obj", glm::vec3(-65.0f, 0.0f, -50.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.0f, 0.0f, 0.0f));
        model->loadTexture("3D/koi_texture.png", *modelShader, "tex0");
        otherModels.push_back(model);

        //load the skybox
        skybox = new Skybox("Skybox/uw_rt.jpg", "Skybox/uw_lf.jpg", "Skybox/uw_up.jpg", "Skybox/uw_dn.jpg", "Skybox/uw_ft.jpg", "Skybox/uw_bk.jpg");

        //TODO:clean up
        testPlane = new Player("3D/plane.obj", glm::vec3(0, 0, 0), glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        testPlane->loadTexture("3D/brickwall.jpg", *playerShader, "tex0");
        testPlane->loadTexture("3D/brickwall_normal.jpg", *playerShader, "norm_tex");

        //create a spotlight in front of the submarine
        spotLight = new SpotLight(0.05f, 1.0f, 16.0f, glm::vec3(1, 1, 1), 0.5f, playerModel->position + playerModel->direction * 1.0f, glm::vec3(0, 0, -1), 12.5);

        //create a directional light coming from the top
        directionalLight = new DirectionalLight(0.1f, 0.5f, 16.0f, glm::vec3(1, 1, 1), 1.0f, glm::vec3(0, -1, 0));

        //create a third person perspective camera
        thirdPerspectiveCamera = new PerspectiveCamera(playerModel->position - 5.0f * playerModel->direction, playerModel->position, glm::vec3(0, 1.0f, 0), 0.1f, 40.0f);

        //create a first person perspective camera
        firstPerspectiveCamera = new PerspectiveCamera(playerModel->position, playerModel->position + 5.0f * playerModel->direction, glm::vec3(0, 1.0f, 0), 0.1f, 100.0f);

        //create an orthographic camera looking down from the top
        orthoCamera = new OrthoCamera(glm::vec3(0.0f, 10.0f, 0.1f), glm::vec3(0, 0, 0), glm::vec3(0, 1.0f, 0), -100.0f, 100.0f);

        //set the third person perspective camera as the active camera
        activeCamera = thirdPerspectiveCamera;

        // print the initial info of the submarine
        std::cout << "Submarine system initialization... COMPLETE\n";
        std::cout << "Preparing for underwater exploration...\n\n";
        std::cout << "[SUBMARINE STATUS]\n";
        std::cout << "Current ocean depth: " << playerModel->position.y;
    }

    //destructor for the environment class
    ~Environment() {
        //deallocates the objects from the memory
        delete playerModel;
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
        //update the position and target of the camera based on the player position
        firstPerspectiveCamera->updateFields(playerModel->position, playerModel->direction);
        thirdPerspectiveCamera->updateFields(playerModel->position);

        //update the spot light based on the player position
        spotLight->updateFields(playerModel->position + playerModel->direction * 1.0f, playerModel->direction);

        //update the player and model shader
        updateShader(*playerShader);
        updateShader(*modelShader);

        //update the skybox based on the camera perspective
        skybox->setViewMatrix(*skyboxShader, activeCamera->viewMatrix);
        skybox->setProjectionMatrix(*skyboxShader, activeCamera->projectionMatrix);
        skybox->setTransformationMatrix(*skyboxShader);

        //draws the objects on the screens
        //TODO: cleanup
        if (activeCamera == firstPerspectiveCamera) {
            //set the objects to a shade of color
            glEnable(GL_BLEND);
            glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_COLOR);
            glBlendEquation(GL_FUNC_ADD);
            glBlendColor(0.0f, 0.41f, 0.58f, 1.000);
        }
        else {
            //disable blending and draw the player model
            glDisable(GL_BLEND);
            playerModel->draw(*playerShader);
        }
        
        //draw all the other models
        for (int i = 0; i < otherModels.size(); i++) {
            otherModels[i]->draw(*modelShader);
        }

        //draw the skybox
        skybox->draw(*skyboxShader);
    }

    //updates the uniform values in the shader file
    void updateShader(Shader shader) {

        //updates the uniform values of the active camera
        activeCamera->setViewMatrix(shader);
        activeCamera->setProjectionMatrix(shader);
        activeCamera->setCameraPosition(shader);

        //updates the uniform values of the spot light
        spotLight->setAmbientStr(shader);
        spotLight->setSpecStr(shader);
        spotLight->setSpecPhong(shader);
        spotLight->setLightColor(shader);
        spotLight->setLightIntensity(shader);
        spotLight->setLightPosition(shader);
        spotLight->setLightDirection(shader);
        spotLight->setAttenuationConstants(shader);
        spotLight->setCutoff(shader);

        //updates the uniform values of the directional light
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
