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

#include "Classes/Models/MainModel.h"

//----------CLASSES----------

class MyCamera {

public:
    glm::vec3 position; //position of the camera in the world
    glm::vec3 target; //position of the target in the world
    glm::vec3 up; //the world up vector
    float distance; //distance from the camera to the target

    //constructor for the camera class
    MyCamera(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
        this->position = position;
        this->target = target;
        this->up = up;
        distance = glm::length(target - position); //computes for the distance from the camera to its target
    }

    //set the value of the view matrix in the shader
    void setViewMatrix(Shader shader) {
        shader.useProgram();

        //computes for the view matrix
        glm::mat4 viewMatrix = glm::lookAt(position, target, up);

        //set the value of the view matrix in the shader
        unsigned int viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    }
    //set the value of the cameraPos in the shader
    void setCameraPosition(Shader shader) {
        shader.useProgram();

        //set the value of the cameraPos in the shader
        unsigned int cameraPosLoc = glGetUniformLocation(shader.shaderProgram, "cameraPos");
        glUniform3fv(cameraPosLoc, 1, glm::value_ptr(position));
    }

    //pure virtual function that sets the value of the projection matrix in the shader
    virtual void setProjectionMatrix(Shader shader) = 0;
};

class PerspectiveCamera : public MyCamera {

public:
    float xLast, yLast; //stores the last cursor position
    bool isInitialized;//checks if the values of xLast and yLast have been initialized
    float pitch; //the rotation of the camera around the x axis
    float yaw; //the rotation of the camera around the y axis

    //constructor for the perspective camera class
    PerspectiveCamera(glm::vec3 position, glm::vec3 target, glm::vec3 up) : MyCamera(position, target, up) {
        xLast = yLast = 0.0f;
        isInitialized = false;
        pitch = 0.0f;
        yaw = 90.0f;
    }

    //set the value of the projection matrix in the shader
    void setProjectionMatrix(Shader shader) override {
        shader.useProgram();

        //compute for the projection matrix
        glm::mat4 projection_matrix = glm::perspective(
            glm::radians(45.0f),
            WIDTH / HEIGHT,
            0.1f,
            100.f
        );

        //set the value of the projection matrix in the shader
        unsigned int projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    }

    //process the mouse inputs and updates the object attributes
    void processMouse(float xPos, float yPos) {
        //initialize the value of xLast and yLast
        if (!isInitialized) {
            xLast = xPos;
            yLast = yPos;
            isInitialized = true;
        }

        float sensitivity = 1.0f; //controls rotation senstivity

        //compute the displacement from the previous position
        float xDiff = xPos - xLast;
        float yDiff = yLast - yPos;

        //update the last position
        xLast = xPos;
        yLast = yPos;

        //updates the value of the pitch and yaw
        yaw += xDiff * sensitivity;
        pitch += yDiff * sensitivity;

        //prevent lookAt flip by limiting the rotation along the x-axis to be greater than -90 deegress
        if (pitch < -90.0f) {
            pitch = -89.99f;
        }

        //prevent lookAt flip by limiting the rotation along the x-axis to be less than 90 deegress
        if (pitch > 90.0f) {
            pitch = 89.99f;
        }

        //recompute the position of the camera depending on the values of yaw and pitch
        glm::vec3 newPosition;
        newPosition.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)); //the x component is influenced by the x component of the yaw and pitch
        newPosition.y = sin(glm::radians(pitch)); //the y component is influenced by the y component of the pitch
        newPosition.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));//the z component is influenced by the y component of the yaw and the x component of the pitch

        newPosition = distance * glm::normalize(newPosition); //ensure that the distance between the camera and the target relative to the camera position is always the same

        //update the camera position
        position = newPosition;
    }
};

class OrthoCamera : public MyCamera {
public:

    //constructor for the orthographic camera class
    OrthoCamera(glm::vec3 position, glm::vec3 target, glm::vec3 up) : MyCamera(position, target, up) { }

    //set the value of the projection matrix in the shader
    void setProjectionMatrix(Shader shader) override {
        shader.useProgram();

        //compute for the projection matrix
        glm::mat4 projection_matrix = glm::ortho(-WIDTH / 100, WIDTH / 100, -HEIGHT / 100, HEIGHT / 100, -100.0f, 100.0f);

        //set the value of the projection matrix in the shader
        unsigned int projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    }
};

class Light {

public:
    float ambientStr; //stores the value of the ambient strength
    float specStr; //stores the value of the specular strength
    float specPhong; //stores the value of the specular phong
    glm::vec3 lightColor; //stores the value of the light color
    float lightIntensity;

    //constructor for the light class
    Light(float ambientStr, float specStr, float specPhong, glm::vec3 lightColor, float lightIntensity) {
        this->ambientStr = ambientStr;
        this->specStr = specStr;
        this->specPhong = specPhong;
        this->lightColor = lightColor;
        this->lightIntensity = lightIntensity;
    }

    //pure virtual function that sets the value of the ambient strength in the shader
    virtual void setAmbientStr(Shader shader) = 0;
    //pure virtual function that sets the value of the specular strength in the shader
    virtual void setSpecStr(Shader shader) = 0;
    //pure virtual function that sets the value of the specular phong in the shader
    virtual void setSpecPhong(Shader shader) = 0;
    //pure virtual function that sets the value of the light color in the shader
    virtual void setLightColor(Shader shader) = 0;
    //pure virtual function that sets the value of the light intensity in the shader
    virtual void setLightIntensity(Shader shader) = 0;

};

class DirectionalLight : public Light {

public:
    glm::vec3 lightDir; //stores the value of the light direction

    //constructor for the directional light class
    DirectionalLight(float ambientStr, float specStr, float specPhong, glm::vec3 lightColor, float lightIntensity, glm::vec3 lightDir) : Light(ambientStr, specStr, specPhong, lightColor, lightIntensity) {
        this->lightDir = lightDir;
    }

    //set the value of the ambient strength in the shader
    void setAmbientStr(Shader shader) override {
        shader.useProgram();

        //set the value of the ambient strength in the shader
        unsigned int ambientStrLoc = glGetUniformLocation(shader.shaderProgram, "directionalLight.ambientStr");
        glUniform1f(ambientStrLoc, ambientStr);
    }

    //set the value of the specular strength in the shader
    void setSpecStr(Shader shader) override {
        shader.useProgram();

        //set the value of the specular strength in the shader
        unsigned int specStrLoc = glGetUniformLocation(shader.shaderProgram, "directionalLight.specStr");
        glUniform1f(specStrLoc, specStr);

    }
    //set the value of the specular phong in the shader
    void setSpecPhong(Shader shader) override {
        shader.useProgram();

        //set the value of the specular phong in the shader
        unsigned int specPhongLoc = glGetUniformLocation(shader.shaderProgram, "directionalLight.specPhong");
        glUniform1f(specPhongLoc, specPhong);
    }

    //set the value of the light color in the shader
    void setLightColor(Shader shader) override {
        shader.useProgram();

        //set the value of the light color in the shader
        unsigned int lightColorLoc = glGetUniformLocation(shader.shaderProgram, "directionalLight.color");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    }

    //set the value of the light intensity in the shader
    void setLightIntensity(Shader shader) override {
        shader.useProgram();

        //set the value of the light intensity in the shader
        unsigned int lightIntensityLoc = glGetUniformLocation(shader.shaderProgram, "directionalLight.intensity");
        glUniform1f(lightIntensityLoc, lightIntensity);
    }

    //set the value of the light direction in the shader
    void setLightDirection(Shader shader) {
        shader.useProgram();

        //set the value of the light direction in the shader
        unsigned int lightDirLoc = glGetUniformLocation(shader.shaderProgram, "directionalLight.direction");
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
    }

    //process keyboard inputs and update the object attributes
    void processKeyboard(int key) {
        float sensitivity = 0.025;

        //increase the light intensity
        if (key == GLFW_KEY_RIGHT) {
            lightIntensity += sensitivity;
        }

        //decrease the light intensity
        if (key == GLFW_KEY_LEFT) {
            lightIntensity -= sensitivity;
            lightIntensity = glm::max(lightIntensity, 0.0f); //prevent negative intensity
        }
    }
};

class PointLight : public Light, public Model3D {

public:
    //fixed values for attenuation
    float constant;
    float linear;
    float quadratic;

    glm::vec3 revolution; //stores the theta values of the revolution around the origin
    glm::vec3 initPosition; //stores the initial position of the point light

    bool isActive; //indiciate if the light is selected

    //constructor for the point light class
    PointLight(std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 theta, float ambientStr, float specStr, float specPhong, glm::vec3 lightColor, float lightIntensity) :
        Light(ambientStr, specStr, specPhong, lightColor, lightIntensity), Model3D(modelPath, position, scale, theta) {
        // values from https ://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
        constant = 1.0f;
        linear = 0.0014f;
        quadratic = 0.000007f;

        revolution = glm::vec3(0.0f, 0.0f, 0.0f);
        initPosition = position;

        isActive = false;
    }

    //set the value of the ambient strength in the shader
    void setAmbientStr(Shader shader) override {
        shader.useProgram();

        //set the value of the ambient strength in the shader
        unsigned int ambientStrLoc = glGetUniformLocation(shader.shaderProgram, "pointLight.ambientStr");
        glUniform1f(ambientStrLoc, ambientStr);
    }

    //set the value of the specular strength in the shader
    void setSpecStr(Shader shader) override {
        shader.useProgram();

        //set the value of the specular strength in the shader
        unsigned int specStrLoc = glGetUniformLocation(shader.shaderProgram, "pointLight.specStr");
        glUniform1f(specStrLoc, specStr);
    }

    //set the value of the specular phong in the shader
    void setSpecPhong(Shader shader) override {
        shader.useProgram();

        //set the value of the specular phong in the shader
        unsigned int specPhongLoc = glGetUniformLocation(shader.shaderProgram, "pointLight.specPhong");
        glUniform1f(specPhongLoc, specPhong);
    }

    //set the value of the light color in the shader
    void setLightColor(Shader shader) override {
        shader.useProgram();

        //set the value of the light color in the shader
        unsigned int lightColorLoc = glGetUniformLocation(shader.shaderProgram, "pointLight.color");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    }

    //set the value of the light intensity in the shader
    void setLightIntensity(Shader shader) override {
        shader.useProgram();

        //set the value of the light intensity in the shader
        unsigned int lightIntensityLoc = glGetUniformLocation(shader.shaderProgram, "pointLight.intensity");
        glUniform1f(lightIntensityLoc, lightIntensity);
    }

    //set the value of the light position in the shader
    void setLightPosition(Shader shader) {
        shader.useProgram();

        //set the value of the light position in the shader
        unsigned int lightPosLoc = glGetUniformLocation(shader.shaderProgram, "pointLight.position");
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(position));
    }

    //set the value of the attenuation constants in the shader
    void setAttenuationConstants(Shader shader) {
        shader.useProgram();

        //set the value of the constant value
        unsigned int constantLoc = glGetUniformLocation(shader.shaderProgram, "pointLight.constant");
        glUniform1f(constantLoc, constant);

        //set the value of the linear value
        unsigned int linearLoc = glGetUniformLocation(shader.shaderProgram, "pointLight.linear");
        glUniform1f(linearLoc, linear);

        //set the value of the quadratic value
        unsigned int quadraticLoc = glGetUniformLocation(shader.shaderProgram, "pointLight.quadratic");
        glUniform1f(quadraticLoc, quadratic);
    }

    //set the value the custom light color in the shader
    void setCustomColor(Shader shader) {
        shader.useProgram();

        //set the value of the custom light color in the shader
        unsigned int customColorLoc = glGetUniformLocation(shader.shaderProgram, "customColor");
        glUniform3fv(customColorLoc, 1, glm::value_ptr(lightColor));
    }

    //process keyboard inputs and update the object attributes
    void processKeyboard(int key) {

        float revSensitivity = 5.0;

        //revolve around the x axis
        if (key == GLFW_KEY_W) {
            revolution.x -= revSensitivity;
        }

        if (key == GLFW_KEY_S) {
            revolution.x += revSensitivity;
        }

        //revolve around the y axis
        if (key == GLFW_KEY_A) {
            revolution.y -= revSensitivity;
        }

        if (key == GLFW_KEY_D) {
            revolution.y += revSensitivity;
        }

        //revolve around the z axis
        if (key == GLFW_KEY_Q) {
            revolution.z -= revSensitivity;
        }

        if (key == GLFW_KEY_E) {
            revolution.z += revSensitivity;
        }

        //calculate for the rotation of the point light
        glm::mat4 transformation_matrix = glm::mat4(1.0f);
        //rotate the point light along the x-axis
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(revolution.x), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
        //rotate the point light along the y-axis
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(revolution.y), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
        //rotate the point light along the z-axis
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(revolution.z), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
        //calculate for the new position of the matrix by applying the transformation matrix to the initial position of the point light
        position = transformation_matrix * glm::vec4(initPosition, 1.0);

        //select and deselect the point light
        if (key == GLFW_KEY_SPACE) {
            isActive = !isActive; //select and deselect the point light

            //change the color to green (selected)
            if (isActive) {
                lightColor = glm::vec3(0.5f, 1.0f, 0.5f);
            }
            //change the color to white (not selected)
            else {
                lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
            }
        }

        float lightSensitivity = 0.025;
        //increase the light intensity
        if (key == GLFW_KEY_UP) {
            lightIntensity += lightSensitivity;
        }

        //decrease the light intensity
        if (key == GLFW_KEY_DOWN) {
            lightIntensity -= lightSensitivity;
            lightIntensity = glm::max(lightIntensity, 0.0f); //prevent negative intensity
        }

    }

};

class Environment {

public:
    MainModel* mainModel;
    PointLight* pointLight;
    DirectionalLight* directionalLight;
    Shader* mainShader;
    Shader* sphereShader;
    PerspectiveCamera* perspectiveCamera;
    OrthoCamera* orthoCamera;

    int activeCamera = 0;

    //constructor for the environment class which initializes the objects necessary to render the program such as the models, lights, shaders, and cameras
    Environment() {
        //load the main model and its textures
        //3D model taken from Free3D.com by user printable_models (link to creation: https://free3d.com/3d-model/bird-v1--875504.html)
        mainModel = new MainModel("3D/bird.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 0.0f, 0.0f));
        mainModel->loadTexture("3D/bird.jpg");

        //create a point light and load the sphere object
        //3D model for sphere taken from the MIT website (http://web.mit.edu/djwendel/www/weblogo/shapes/basic-shapes/sphere/sphere.obj)
        pointLight = new PointLight("3D/sphere.obj", glm::vec3(3.0f, 3.0f, 0), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5, 1.0f, 16.0f, glm::vec3(1, 1, 1), 1.0f);

        //create a directional light with a position of (4, 11, -3)
        directionalLight = new DirectionalLight(0.5, 1.0f, 16.0f, glm::vec3(1, 1, 1), 1.0f, glm::vec3(0, 0, 0) - glm::vec3(4, 11, -3));

        //load the shader for the main object
        mainShader = new Shader("Shaders/bird.vert", "Shaders/bird.frag");

        //load the shader for the point light
        sphereShader = new Shader("Shaders/sphere.vert", "Shaders/sphere.frag");

        //create a perspective camera
        perspectiveCamera = new PerspectiveCamera(glm::vec3(0, 0, 10.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1.0f, 0));

        //create a orthographic camera
        orthoCamera = new OrthoCamera(glm::vec3(0, 10.0f, 1.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1.0f, 0));
    }
    
    //destructor for the environment class
    ~Environment() {
        //deallocates the objects from the memory
        delete mainModel;
        delete pointLight;
        delete directionalLight;
        delete mainShader;
        delete sphereShader;
        delete perspectiveCamera;
        delete orthoCamera;
    }

    //updates the uniform values of the shader files and draws the objects on the screen
    void update() {

        //updates the uniform values
        if (activeCamera == 0) {
            perspectiveCamera->setViewMatrix(*mainShader);
            perspectiveCamera->setProjectionMatrix(*mainShader);

            perspectiveCamera->setViewMatrix(*sphereShader);
            perspectiveCamera->setProjectionMatrix(*sphereShader);

            perspectiveCamera->setCameraPosition(*mainShader);
        }

        if (activeCamera == 1) {
            orthoCamera->setViewMatrix(*mainShader);
            orthoCamera->setProjectionMatrix(*mainShader);

            orthoCamera->setViewMatrix(*sphereShader);
            orthoCamera->setProjectionMatrix(*sphereShader);

            orthoCamera->setCameraPosition(*mainShader);
        }

        pointLight->setCustomColor(*sphereShader);

        pointLight->setAmbientStr(*mainShader);
        pointLight->setSpecStr(*mainShader);
        pointLight->setSpecPhong(*mainShader);
        pointLight->setLightColor(*mainShader);
        pointLight->setLightIntensity(*mainShader);
        pointLight->setLightPosition(*mainShader);
        pointLight->setAttenuationConstants(*mainShader);

        directionalLight->setAmbientStr(*mainShader);
        directionalLight->setSpecStr(*mainShader);
        directionalLight->setSpecPhong(*mainShader);
        directionalLight->setLightColor(*mainShader);
        directionalLight->setLightIntensity(*mainShader);
        directionalLight->setLightDirection(*mainShader);

        //draws the objects on the screens
        mainModel->draw(*mainShader);
        pointLight->draw(*sphereShader);
    }
};

//----------GLOBAL VARIABLES----------
Environment* environment; //pointer to the environment object

//----------CALLBACK FUNCTIONS----------
//callback function for key presses
void Key_Callback(GLFWwindow* window, int key, int scanCode, int action, int mods) {

    //model rotation and light movement
    if ((key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D || key == GLFW_KEY_Q || key == GLFW_KEY_E) && action == GLFW_REPEAT) {

        //rotate the point light when the point light is selected
        if (!environment->pointLight->isActive) {
            environment->mainModel->processKeyboard(key);
        }

        //rotate the main model when the point light is not selected
        if (environment->pointLight->isActive) {
            environment->pointLight->processKeyboard(key);
        }
    }

    //select and deselect the point light
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        environment->pointLight->processKeyboard(key);
    }

    //increase and decrease the light intensity
    if ((key == GLFW_KEY_UP || key == GLFW_KEY_DOWN || key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        environment->directionalLight->processKeyboard(key);
        environment->pointLight->processKeyboard(key);
    }

    //change the camera view
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        environment->activeCamera = 0;
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        environment->activeCamera = 1;
    }
}

//callback function for cursor movement
void Mouse_Callback(GLFWwindow* window, double xPos, double yPos) {
    if (environment->activeCamera == 0) {
        environment->perspectiveCamera->processMouse(xPos, yPos);
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
        environment->update();

        //swap front and back buffers
        glfwSwapBuffers(window);

        //poll for and process events
        glfwPollEvents();
    }

    delete environment; //deallocate the memory for environment

    glfwTerminate();

    return 0;
}
