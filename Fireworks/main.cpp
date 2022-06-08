#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <gl/GLU.h>
#include <gl/GL.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "utils.h"

#include <iostream>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void render(GLuint vertexbuffer, GLuint colorbuffer, Shader cubeShader);
void processInput(GLFWwindow* window);

float SCREEN_W = 800.0;
float SCREEN_H = 600.0;

const float FOV = 45.0;
const float NEAR_CLIP = 1.0;
const float FAR_CLIP = 100.0;

Camera* camera;
glm::highp_mat4 projection;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_W, SCREEN_H, "Fireworks", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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

    glClearColor(0, 0.6f, 0.8f, 1.0f);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto worldToView = camera->getWorldToViewMatrix();
        auto modelView = glm::mat4(1.0);
        auto mvp = projection * worldToView * modelView;

        GLuint mvpLocation = glGetUniformLocation(cubeShader.id, "MVP");
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_REPEAT)
        return;

    switch (key)
    {
    case GLFW_KEY_W:
        std::cout << "Z pressed" << std::endl;
        camera->moveForward(1);
        break;
    case GLFW_KEY_A:
        std::cout << "Q pressed" << std::endl;
        break;
    case GLFW_KEY_S:
        std::cout << "S pressed" << std::endl;
        camera->moveForward(-1);
        break;
    case GLFW_KEY_D:
        std::cout << "D pressed" << std::endl;
    }
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCREEN_W = width;
    SCREEN_H = height;
    projection = glm::perspective(glm::radians(FOV), (GLfloat)(SCREEN_W / SCREEN_H), NEAR_CLIP, FAR_CLIP);

    glViewport(0, 0, width, height);
}
