#include <resources.h>

#include <iostream>
#include <string>
#include <cmath>
#include <unordered_map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/mesh.h>
#include <rendersystem/Shader.h>
#include <rendersystem/Camera.h>
#include <rendersystem/Model.h>
#include <rendersystem/Lights.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile our shader zprogram
    // ------------------------------------

    // build and compile our shader zprogram
    // ------------------------------------
    Shader defaultShader(SHADERS_DIR "colors.vert",  SHADERS_DIR "colors.frag");
    Shader grassShader(SHADERS_DIR "drop.vert", SHADERS_DIR "drop.frag");
    Shader lightCubeShader(SHADERS_DIR "light_cube.vert", SHADERS_DIR "light_cube.frag");
    Shader blendedShader(SHADERS_DIR "blend.vert", SHADERS_DIR "blend.frag");
    auto mainLight = PointLight::DefaultPointLight();
    auto dirLight = DirectionalLight::DefaultDirectionalLight();

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    auto lightCube = std::make_shared<ControlledMesh>(ControlledMesh::CreateCube(0.2f));
    lightCube->SetAxis(glm::vec3{ 0, 1, 0 });
    lightCube->SetColor(glm::vec3{ 1, 1, 1 });

    auto platform = std::make_shared<ControlledMesh>(ControlledMesh::CreateCuboid(10.0f, 1.0f, 10.0f));
    platform->SetAxis(glm::vec3{ 0, 1, 0 });
    platform->AddTexture(ASSETS_DIR "cobble.jpg", "texture_diffuse");
    platform->SetPosition(glm::vec3{ 0, -3, 0 });

    auto sphere = std::make_shared<ControlledMesh>(ControlledMesh::CreateSphere(0.5f));
    sphere->SetAxis(glm::vec3{ 0, 1, 0 });
    sphere->SetPosition(glm::vec3{ 0, 0, -3 });
    sphere->AddTexture(ASSETS_DIR "eye.png", "texture_diffuse", []() {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    });

    auto grass = std::make_shared<ControlledMesh>(ControlledMesh::CreateQuad(1.5f, 1.5f));
    grass->SetAxis(glm::vec3{ 0, 1, 0 });
    grass->SetPosition(glm::vec3{ 3.0f, -1.75f, 0 });
    grass->AddTexture(ASSETS_DIR "grass.png", "texture_diffuse", []() {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    });
    grass->Rotate(90.0f);

    auto blended_window = std::make_shared<ControlledMesh>(ControlledMesh::CreateQuad(1.5f, 1.5f, false));
    blended_window->SetPosition(glm::vec3{ -3.0f, -1.75f, 0 });
    blended_window->AddTexture(ASSETS_DIR "blending_transparent_window.png", "texture_diffuse");

    auto blended_window_2 = std::make_shared<ControlledMesh>(ControlledMesh::CreateQuad(1.5f, 1.5f, false));
    blended_window_2->SetPosition(glm::vec3{ -3.0f, -1.75f, 3.0f });
    blended_window_2->AddTexture(ASSETS_DIR "blending_transparent_window.png", "texture_diffuse");

    auto loaded_model = std::make_shared<Model>(MODELS_DIR "backpack/backpack.obj");
    loaded_model->Rotate(180.0f);


    auto lightedShaders = std::vector<std::pair<const Shader, std::vector<std::shared_ptr<Drawable>>>>{
        std::make_pair(defaultShader,
            std::vector<std::shared_ptr<Drawable>>{
                loaded_model,
                platform,
                sphere
            }),
        std::make_pair(grassShader,
            std::vector<std::shared_ptr<Drawable>>{
                grass
            }),
        std::make_pair(lightCubeShader,
            std::vector<std::shared_ptr<Drawable>>{
                lightCube
            }),
        std::make_pair(blendedShader,
            std::vector<std::shared_ptr<Drawable>>{
                blended_window,
                blended_window_2
            })
    };

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (camRot) {
            camera.Rotate((float)glm::radians(0.01f), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, near, far);
        glm::mat4 view = camera.GetViewMatrix();

        // draw default shaded models
        // set up shaders (camera, lights, etc.)
        for (auto& [shader, objs] : lightedShaders) {
            shader.use();

            mainLight.SetShader(shader, 0);
            dirLight.SetShader(shader, 0);

            shader.setVec3("viewPos", camera.Position);
            shader.setMat4("view", view);
            shader.setMat4("projection", projection);
            shader.setFloat("near", near);
            shader.setFloat("far", far);
            shader.setBool("enableVisualiseDepthBuffer", false);

            std::map<float, std::shared_ptr<Drawable>> sorted_nonopaques;
            for (auto& obj : objs) {
                if (obj->IsOpaque()) {
                    obj->Draw(shader);
                }
                else {
                    auto dist = glm::length(camera.Position - obj->Position());
                    sorted_nonopaques[dist] = obj;
                }
            }

            for (auto it = sorted_nonopaques.rbegin(); it != sorted_nonopaques.rend(); ++it) {
                it->second->Draw(shader);
            }
        }

        // action logic
        // -----
        glm::quat rot = glm::angleAxis((float)glm::radians(50.0f * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
        lightPos = rot * glm::vec4(lightPos, 1.0f);

        sphere->Rotate(100.0f * deltaTime);
        lightCube->SetPosition(lightPos);
        mainLight.SetPosition(lightPos);

        
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