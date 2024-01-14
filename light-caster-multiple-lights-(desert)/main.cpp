/*
https://learnopengl.com/Lighting/Light-casters
https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <texture.h>

#include <iostream>


void drawLight(unsigned int shaderId, unsigned int VAO, glm::mat4 view, glm::mat4 projection, glm::vec3 diffuseLight, glm::vec3 lightPos);

void relayDirectionLightParams(unsigned int shaderId);
void relayPointLightParams(unsigned int shaderId, unsigned int i, glm::vec3 lightPos, glm::vec3 diffuseLight);
void relaySpotlightParams(unsigned int shaderId);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Texture helper
Texture textureHelper;

// Camera
Camera camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));

bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// Keep track of frame times
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

glm::vec3 lightPos(5.0f, 2.0f, -2.5f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable z-buffer
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("shader.vs", "shader.fs");
    Shader lightShader("lightShader.vs", "lightShader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
         -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
          0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
          0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
          0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
         -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

         -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
          0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

         -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

          0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
          0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
          0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
          0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
          0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
          0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
          0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
          0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // load and create a textures
    // -------------------------
    unsigned int diffuseMap = textureHelper.generateTexture("../resources/textures/container2.png");
    unsigned int specularMap = textureHelper.generateTexture("../resources/textures/container2_specular.png");

    // Light VAO
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set the vertex attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // Set Texture in shader
    glUseProgram(ourShader.ID);
    glUniform1i(glGetUniformLocation(ourShader.ID, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(ourShader.ID, "material.specular"), 1);

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    glm::vec3 pointLightDiffuse[] = {
        glm::vec3(0.8f, 0.7f, 0.4f),
        glm::vec3(0.9f, 0.6f, 0.3f),
        glm::vec3(0.7f, 0.4f, 0.2f),
        glm::vec3(0.6f, 0.1f, 0.1f)
    };

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.9f, 0.5f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

        // render container
        glUseProgram(ourShader.ID);

        // bind Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        relayDirectionLightParams(ourShader.ID);

        for (unsigned int i = 0; i < (sizeof(pointLightPositions) / sizeof(*pointLightPositions)); i++) {
            relayPointLightParams(ourShader.ID, i, pointLightPositions[i], pointLightDiffuse[i]);
        }

        relaySpotlightParams(ourShader.ID);

        // material shininess
        unsigned int shininessLoc = glGetUniformLocation(ourShader.ID, "material.shininess");
        glUniform1f(shininessLoc, 2.0f);

        for (unsigned int i = 0; i < (sizeof(cubePositions) / sizeof(*cubePositions)); i++) {

            unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            unsigned int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glBindVertexArray(VAO);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            float angle = 20.0f * i;
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            // View
            unsigned int viewPosLoc = glGetUniformLocation(ourShader.ID, "viewPos");
            glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.Position));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        for (unsigned int i = 0; i < (sizeof(pointLightPositions) / sizeof(*pointLightPositions)); i++) {
            drawLight(lightShader.ID, lightVAO, view, projection, pointLightDiffuse[i], pointLightPositions[i]);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void relaySpotlightParams(unsigned int shaderId) {
    unsigned int lightAmbientLoc = glGetUniformLocation(shaderId, "spotlight.ambient");
    glUniform3fv(lightAmbientLoc, 1, glm::value_ptr(glm::vec3(0.1f)));

    glm::vec3 diffuseLight(2.f);
    unsigned int lightDiffuseLoc = glGetUniformLocation(shaderId, "spotlight.diffuse");
    glUniform3fv(lightDiffuseLoc, 1, glm::value_ptr(diffuseLight));

    unsigned int lightSpecularLoc = glGetUniformLocation(shaderId, "spotlight.specular");
    glUniform3fv(lightSpecularLoc, 1, glm::value_ptr(glm::vec3(1.0f)));

    unsigned int lightDirectionLoc = glGetUniformLocation(shaderId, "spotlight.direction");
    glUniform3fv(lightDirectionLoc, 1, glm::value_ptr(camera.Front));

    unsigned int lightPositionLoc = glGetUniformLocation(shaderId, "spotlight.position");
    glUniform3fv(lightPositionLoc, 1, glm::value_ptr(camera.Position));

    unsigned int lightCutOffLoc = glGetUniformLocation(shaderId, "spotlight.cutOff");
    glUniform1f(lightCutOffLoc, glm::cos(glm::radians(1.5)));

    unsigned int lightOuterCutOffLoc = glGetUniformLocation(shaderId, "spotlight.outerCutOff");
    glUniform1f(lightOuterCutOffLoc, glm::cos(glm::radians(15.0)));

    // attenuation levels
    // https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
    unsigned int lightConstantLoc = glGetUniformLocation(shaderId, "spotlight.constant");
    glUniform1f(lightConstantLoc, 1.0f);

    unsigned int lightLinearLoc = glGetUniformLocation(shaderId, "spotlight.linear");
    glUniform1f(lightLinearLoc, 0.09f);

    unsigned int lightQuadraticLoc = glGetUniformLocation(shaderId, "spotlight.quadratic");
    glUniform1f(lightQuadraticLoc, 0.032f);
}

void relayPointLightParams(unsigned int shaderId, unsigned int i, glm::vec3 lightPos, glm::vec3 diffuseLight) {
    unsigned int lightAmbientLoc = glGetUniformLocation(shaderId, (std::string("pointLights[") + std::to_string(i) + std::string("].ambient")).c_str());
    glUniform3fv(lightAmbientLoc, 1, glm::value_ptr(diffuseLight));

    unsigned int lightDiffuseLoc = glGetUniformLocation(shaderId, (std::string("pointLights[") + std::to_string(i) + std::string("].diffuse")).c_str());
    glUniform3fv(lightDiffuseLoc, 1, glm::value_ptr(diffuseLight));

    unsigned int lightSpecularLoc = glGetUniformLocation(shaderId, (std::string("pointLights[") + std::to_string(i) + std::string("].specular")).c_str());
    glUniform3fv(lightSpecularLoc, 1, glm::value_ptr(diffuseLight + glm::vec3(0.2f)));

    unsigned int lightPositionLoc = glGetUniformLocation(shaderId, (std::string("pointLights[") + std::to_string(i) + std::string("].position")).c_str());
    glUniform3fv(lightPositionLoc, 1, glm::value_ptr(lightPos));

    // attenuation levels
    unsigned int lightConstantLoc = glGetUniformLocation(shaderId, (std::string("pointLights[") + std::to_string(i) + std::string("].constant")).c_str());
    glUniform1f(lightConstantLoc, 1.0f);

    unsigned int lightLinearLoc = glGetUniformLocation(shaderId, (std::string("pointLights[") + std::to_string(i) + std::string("].linear")).c_str());
    glUniform1f(lightLinearLoc, 0.09f);

    unsigned int lightQuadraticLoc = glGetUniformLocation(shaderId, (std::string("pointLights[") + std::to_string(i) + std::string("].quadratic")).c_str());
    glUniform1f(lightQuadraticLoc, 0.032f);
}

void relayDirectionLightParams(unsigned int shaderId) {
    unsigned int lightAmbientLoc = glGetUniformLocation(shaderId, "dirLight.ambient");
    glUniform3fv(lightAmbientLoc, 1, glm::value_ptr(glm::vec3(0.1f)));

    glm::vec3 diffuseLight(0.3f);
    unsigned int lightDiffuseLoc = glGetUniformLocation(shaderId, "dirLight.diffuse");
    glUniform3fv(lightDiffuseLoc, 1, glm::value_ptr(diffuseLight));

    unsigned int lightSpecularLoc = glGetUniformLocation(shaderId, "dirLight.specular");
    glUniform3fv(lightSpecularLoc, 1, glm::value_ptr(glm::vec3(1.0f)));

    unsigned int lightDirectionLoc = glGetUniformLocation(shaderId, "dirLight.direction");
    glUniform3fv(lightDirectionLoc, 1, glm::value_ptr(lightPos));
}

void drawLight(unsigned int shaderId, unsigned int VAO, glm::mat4 view, glm::mat4 projection, glm::vec3 diffuseLight, glm::vec3 lightPos) {
    // Draw Light
    glUseProgram(shaderId);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
    unsigned int modelLoc = glGetUniformLocation(shaderId, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    unsigned int viewLoc = glGetUniformLocation(shaderId, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    unsigned int projectionLoc = glGetUniformLocation(shaderId, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    unsigned int lightBoxColorLoc = glGetUniformLocation(shaderId, "lightColor");
    glUniform3fv(lightBoxColorLoc, 1, glm::value_ptr(diffuseLight));
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    const float cameraSpeed = 5.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}