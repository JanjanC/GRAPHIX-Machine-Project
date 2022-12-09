#pragma once

class Environment {

public:
    Player* playerModel;
    std::vector<Model*> otherModels;
    Skybox* skybox;
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

    //constructor for the environment class which initializes the objects necessary to render the program such as the models, lights, shaders, and cameras
    Environment() {

        //load the shader for the players
        playerShader = new Shader("Shaders/player.vert", "Shaders/player.frag");

        //load the shader for the models
        modelShader = new Shader("Shaders/model.vert", "Shaders/model.frag");

        //load the shader for the skybox
        skyboxShader = new Shader("Shaders/skybox.vert", "Shaders/skybox.frag");

        //load the main model and its textures
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

        //create a spotlight in front of the submarine
        spotLight = new SpotLight(0.05f, 1.0f, 16.0f, glm::vec3(1, 1, 1), 0.5f, playerModel->position + playerModel->direction * 1.0f, glm::vec3(0, 0, -1), 25.0f, 35.0f);

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
        for (int i = 0; i < otherModels.size(); i++) {
            delete otherModels[i];
        }
        delete skybox;
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
        if (activeCamera == firstPerspectiveCamera) {
            //set the objects to a shade of color
            modelShader->useProgram();
            glUniform1i(glGetUniformLocation(modelShader->shaderProgram, "useTexture"), false);
            glEnable(GL_BLEND);
            glBlendFunc(GL_CONSTANT_COLOR, GL_ZERO);
            glBlendEquation(GL_FUNC_ADD);
            glBlendColor(0.0f, 1.0f, 0.25f, 1.0f);
        }
        else {
            modelShader->useProgram();
            glUniform1i(glGetUniformLocation(modelShader->shaderProgram, "useTexture"), true);
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
        spotLight->setOuterCutoff(shader);

        //updates the uniform values of the directional light
        directionalLight->setAmbientStr(shader);
        directionalLight->setSpecStr(shader);
        directionalLight->setSpecPhong(shader);
        directionalLight->setLightColor(shader);
        directionalLight->setLightIntensity(shader);
        directionalLight->setLightDirection(shader);
    }
};
