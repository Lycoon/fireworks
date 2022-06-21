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
void updateCameraRotation(GLFWwindow* window);
void processInput(GLFWwindow* window);

float SCREEN_W = 1920.0f;
float SCREEN_H = 1080.0f;
float FOV = 45.0f;
float SENSITIVITY = 0.1f;

const float NEAR_CLIP = 1.0f;
const float FAR_CLIP = 500.0f;

double lastXPos = SCREEN_W / 2, lastYPos = SCREEN_H / 2;
double yaw = 0, pitch = 0, xPos, yPos;
unsigned int Launcher::particlesCount = 0;

Camera* camera;
glm::highp_mat4 projection;
std::map<int, bool> heldKeys;

int main()
{
    srand(time(NULL));

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_MAXIMIZED, true);
    glfwWindowHint(GLFW_SAMPLES, 4);

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

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Camera
    camera = new Camera(glm::vec3(0.0f, 120.0f, -430.0f));
    projection = glm::perspective(glm::radians(FOV), (GLfloat)(SCREEN_W / SCREEN_H), NEAR_CLIP, FAR_CLIP);

    // Shaders
    Shader particleShader("particle.vert", "particle.frag");

    GLuint cameraRightId = glGetUniformLocation(particleShader.id, "cameraRight");
    GLuint cameraUpId = glGetUniformLocation(particleShader.id, "cameraUp");
    GLuint viewProjId = glGetUniformLocation(particleShader.id, "VP");

    GLfloat* particle_position = new GLfloat[maxParticles * 4];
    GLubyte* particle_color = new GLubyte[maxParticles * 4];

    GLuint billboard_vertex_buffer;
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    GLuint particles_position_buffer;
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    GLuint particles_color_buffer;
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

    Launcher launcher;

    glClearColor(0, 0.1f, 0.2f, 0.8f);
    while (!glfwWindowShouldClose(window))
    {
        Camera::updateDeltaTime();
        launcher.update(*camera, particle_position, particle_color);

        std::cout << "pos: " << glm::to_string(camera->getPosition()) << std::endl;
        std::cout << "dir: " << glm::to_string(camera->getDirection()) << std::endl;

        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Projection
        auto modelMatrix = glm::mat4(1.0);
        auto viewMatrix = camera->getWorldToViewMatrix();
        auto vp = projection * viewMatrix;
        // auto mvp = vp * modelMatrix;

        // Updating particle position buffer
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, Launcher::particlesCount * sizeof(GLfloat) * 4, particle_position);

        // Updating particle color buffer
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, Launcher::particlesCount * sizeof(GLubyte) * 4, particle_color);
        
        glEnable(GL_BLEND);
        particleShader.use();

        // Shader uniforms
        glUniform3f(cameraRightId, viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
        glUniform3f(cameraUpId, viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
        glUniformMatrix4fv(viewProjId, 1, GL_FALSE, &vp[0][0]);

        // Quad vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Particle position
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Particle color
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0);

        glVertexAttribDivisor(0, 0); // Quad vertices (4 for each particle)
        glVertexAttribDivisor(1, 1); // 1 position for each quad
        glVertexAttribDivisor(2, 1); // 1 color for each quad

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, Launcher::particlesCount);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete[] particle_position;
    delete[] particle_color;

    glDeleteBuffers(1, &particles_color_buffer);
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &billboard_vertex_buffer);
    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();
    return 0;
}

void updateCameraRotation(GLFWwindow* window)
{
    glfwGetCursorPos(window, &xPos, &yPos);

    yaw += (xPos - lastXPos) * SENSITIVITY;
    pitch += (lastYPos - yPos) * SENSITIVITY;
    pitch = clamp(pitch, 89.0f, -89.0f);

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

    // Camera controls
    // updateCameraRotation(window);

    if (heldKeys[GLFW_KEY_SPACE])
        camera->moveUp(2.0f);
    if (heldKeys[GLFW_KEY_LEFT_SHIFT])
        camera->moveUp(-2.0f);
    if (heldKeys[GLFW_KEY_W])
        camera->moveForward(2.0f);
    if (heldKeys[GLFW_KEY_S])
        camera->moveForward(-2.0f);
    if (heldKeys[GLFW_KEY_D])
        camera->moveLeft(1.5f);
    if (heldKeys[GLFW_KEY_A])
        camera->moveLeft(-1.5f);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCREEN_W = (float)width;
    SCREEN_H = (float)height;
    projection = glm::perspective(glm::radians(FOV), (GLfloat)(SCREEN_W / SCREEN_H), NEAR_CLIP, FAR_CLIP);

    glViewport(0, 0, width, height);
}
