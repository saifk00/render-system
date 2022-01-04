#include <resources.h>

#include <iostream>
#include <string>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/mesh.h>
#include <rendersystem/Shader.h>
#include <rendersystem/Camera.h>
#include <rendersystem/Sphere.h>
#include <rendersystem/Model.h>
#include <rendersystem/Lights.h>
#include <rendersystem/Cuboid.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


// settings
float fov = 45.0f;
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
Camera camera;
float lastX = 400, lastY = 300;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
// lighting
glm::vec3 lightPos(1.5f, 0.5f, 2.0f);
float near = 0.1f;
float far = 100.0f;
bool camRot = false;
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    // build and compile our shader zprogram
    // ------------------------------------

    // build and compile our shader zprogram
    // ------------------------------------
    Shader loadedModelShader(SHADERS_DIR "colors.vert", SHADERS_DIR "colors.frag");
    Shader defaultShader(SHADERS_DIR "colors.vert", SHADERS_DIR "colors.frag");
    Shader lightCubeShader(SHADERS_DIR "light_cube.vert", SHADERS_DIR "light_cube.frag");
    Shader shaderSingleColor(SHADERS_DIR "single_color.vert", SHADERS_DIR "single_color.frag");
    PointLight mainLight = PointLight::DefaultPointLight();
    DirectionalLight dirLight = DirectionalLight::DefaultDirectionalLight();

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    Cuboid lightCube(
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        0.2f,0.2f,0.2f);

    Cuboid platform(glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f),
        10.0f, 1.0f, 10.0f, 0.0f,
        ASSETS_DIR "awesome.png");
    platform.SetPosition(glm::vec3(0.0f, -3.0f, 0.0f));

    Sphere sphere(
        glm::vec3(0.0f, 0.0f, -3.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        0.5f);
    sphere.AddTexture(ASSETS_DIR "eye.png", "texture_diffuse");

    Sphere sphere_outline(sphere);
    sphere_outline.Scale(1.1f);

    // load model
    Model loaded_model(MODELS_DIR "backpack/backpack.obj");
    loaded_model.Rotate(180.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(0x00);

        if (camRot) {
            camera.Rotate((float)glm::radians(0.01f), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, near, far);
        glm::mat4 view = camera.GetViewMatrix();

        // rotate the light
        glm::quat rot = glm::angleAxis((float)glm::radians(0.05f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightPos = rot * glm::vec4(lightPos, 1.0f);

        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        lightCube.SetPosition(lightPos);
        lightCube.Draw(lightCubeShader);

        mainLight.SetPosition(lightPos);

        // draw default shaded models
        defaultShader.use();
        loadedModelShader.use();
        // set up the light
        mainLight.SetShader(defaultShader, 0);
        mainLight.SetShader(loadedModelShader, 0);

        dirLight.SetShader(defaultShader, 0);
        dirLight.SetShader(loadedModelShader, 0);

        defaultShader.setVec3("viewPos", camera.Position);
        defaultShader.setMat4("projection", projection);
        defaultShader.setMat4("view", view);
        defaultShader.setFloat("near", near);
        defaultShader.setFloat("far", far);
        defaultShader.setBool("enableVisualiseDepthBuffer", false);

        loadedModelShader.setVec3("viewPos", camera.Position);
        loadedModelShader.setMat4("projection", projection);
        loadedModelShader.setMat4("view", view);
        loadedModelShader.setFloat("near", near);
        loadedModelShader.setFloat("far", far);
        loadedModelShader.setBool("enableVisualiseDepthBuffer", false);


        loaded_model.Draw(loadedModelShader);

        // defaultShader users
        platform.Draw(defaultShader);

        // stencil
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        sphere.Draw(defaultShader);

        // draw the upscaled sphere
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        shaderSingleColor.use();
        shaderSingleColor.setMat4("projection", projection);
        shaderSingleColor.setMat4("view", view);
        sphere_outline.Draw(shaderSingleColor);

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);

        sphere.Rotate(100.0f * deltaTime);

        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    const float cameraSpeed = 2.5f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) // initially set to true
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.ProcessMouseMovement(xoffset, yoffset, GL_TRUE);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE) return;

    if (key == GLFW_KEY_X) camRot = !camRot;
}