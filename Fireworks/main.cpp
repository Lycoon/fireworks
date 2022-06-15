#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <gl/GLU.h>
#include <gl/GL.h>
#include <gtx/transform.hpp>
#include <gtx/string_cast.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <map>

#include "camera.h"
#include "shader.h"
#include "utils.h"
#include "launcher.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void render(GLuint vertexbuffer, GLuint colorbuffer, Shader cubeShader);
void updateCameraRotation(GLFWwindow* window);
void processInput(GLFWwindow* window);

float SCREEN_W = 1920.0;
float SCREEN_H = 1080.0;
float FOV = 45.0;
float SENSITIVITY = 0.1f;

const float NEAR_CLIP = 1.0;
const float FAR_CLIP = 100.0;

double lastXPos = 0, lastYPos = 0, yaw = 0, pitch = 0, xPos, yPos;

Camera* camera;
glm::highp_mat4 projection;
std::map<int, bool> heldKeys;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_MAXIMIZED, true);

    GLFWwindow* window = glfwCreateWindow(SCREEN_W, SCREEN_H, "Fireworks", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Instantiating camera
    camera = new Camera(glm::vec3(0, 2, -8));
    projection = glm::perspective(glm::radians(FOV), (GLfloat)(SCREEN_W / SCREEN_H), NEAR_CLIP, FAR_CLIP);

    // Shaders
    Shader cubeShader("cube.vert", "cube.frag");

    // Buffers
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_cube), g_vertex_cube, GL_STATIC_DRAW);

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_cube), g_color_cube, GL_STATIC_DRAW);

    Launcher launcher;

    glClearColor(0, 0.6f, 0.8f, 1.0f);
    while (!glfwWindowShouldClose(window))
    {
        Camera::updateDeltaTime();
        launcher.update();

        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Projection
        updateCameraRotation(window);
        auto worldToView = camera->getWorldToViewMatrix();
        auto modelView = glm::mat4(1.0);
        auto mvp = projection * worldToView * modelView;

        GLuint mvpLocation = glGetUniformLocation(cubeShader.id, "MVP");
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

        // Drawing
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
        glDisableVertexAttribArray(0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        cubeShader.use();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void updateCameraRotation(GLFWwindow* window)
{
    glfwGetCursorPos(window, &xPos, &yPos);

    yaw += (xPos - lastXPos) * SENSITIVITY;
    pitch += (lastYPos - yPos) * SENSITIVITY;
    pitch = clamp(pitch, 89.0, -89.0);

    lastXPos = xPos;
    lastYPos = yPos;

    camera->rotate((float)yaw, (float)pitch);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_REPEAT)
        return;
    heldKeys[key] = action == GLFW_PRESS;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (heldKeys[GLFW_KEY_W])
        camera->moveForward(0.55);
    if (heldKeys[GLFW_KEY_S])
        camera->moveForward(-0.55);
    if (heldKeys[GLFW_KEY_D])
        camera->moveLeft(0.3);
    if (heldKeys[GLFW_KEY_A])
        camera->moveLeft(-0.3);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCREEN_W = (float)width;
    SCREEN_H = (float)height;
    projection = glm::perspective(glm::radians(FOV), (GLfloat)(SCREEN_W / SCREEN_H), NEAR_CLIP, FAR_CLIP);

    glViewport(0, 0, width, height);
}
