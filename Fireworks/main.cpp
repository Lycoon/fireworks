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

float SCREEN_W = 1920.0f;
float SCREEN_H = 1080.0f;
float FOV = 45.0f;
float SENSITIVITY = 0.1f;

const float NEAR_CLIP = 1.0f;
const float FAR_CLIP = 100.0f;

double lastXPos = 0, lastYPos = 0, yaw = 0, pitch = 0, xPos, yPos;

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

    // Instantiating camera
    camera = new Camera(glm::vec3(0, 2.0f, -8.0f));
    projection = glm::perspective(glm::radians(FOV), (GLfloat)(SCREEN_W / SCREEN_H), NEAR_CLIP, FAR_CLIP);

    // Shaders
    //Shader cubeShader("cube.vert", "cube.frag");
    Shader particleShader("particle.vert", "particle.frag");

    GLfloat* particle_position = new GLfloat[maxParticles * 4];
    GLubyte* particle_color = new GLubyte[maxParticles * 4];

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

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

    /*
    // Cube
    GLuint cubeVAO, cube_vert, cube_color_buffer;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glGenBuffers(1, &cube_vert);
    glGenBuffers(1, &cube_color_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, cube_vert);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, cube_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_color), cube_color, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);*/

    Launcher launcher;

    glClearColor(0, 0.6f, 0.8f, 1.0f);
    while (!glfwWindowShouldClose(window))
    {
        Camera::updateDeltaTime();
        launcher.update(*camera, particle_position, particle_color);

        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Projection
        auto modelMatrix = glm::mat4(1.0);
        auto viewMatrix = camera->getWorldToViewMatrix();
        auto vp = projection * viewMatrix;
        auto mvp = vp * modelMatrix;

        //float size = 1.0f;
        //auto camRight = glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
        //auto camUp = glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

        //auto pos = glm::vec3(0.0f, 3.0f, 0.0f);
        //auto v1 = vp * glm::vec4(pos + camRight * 0.5f * size + camUp * -0.5f * size, 1.0f);
        //auto v2 = vp * glm::vec4(pos + camRight * 0.5f * size + camUp * 0.5f * size, 1.0f);
        //auto v3 = vp * glm::vec4(pos + camRight * -0.5f * size + camUp * -0.5f * size, 1.0f);
        //auto v4 = vp * glm::vec4(pos + camRight * -0.5f * size + camUp * 0.5f * size, 1.0f);

        //GLuint mvpLocation = glGetUniformLocation(cubeShader.id, "MVP");
        //glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

        GLuint mvpLocation = glGetUniformLocation(particleShader.id, "MVP");
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

        // Drawing

        //glBindVertexArray(cubeVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * sizeof(GLfloat) * 4, particle_position);

        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * sizeof(GLubyte) * 4, particle_color);
        
        particleShader.use();

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

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particlesCount);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

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
    updateCameraRotation(window);

    if (heldKeys[GLFW_KEY_SPACE])
        camera->moveUp(0.4f);
    if (heldKeys[GLFW_KEY_LEFT_SHIFT])
        camera->moveUp(-0.4f);
    if (heldKeys[GLFW_KEY_W])
        camera->moveForward(0.7f);
    if (heldKeys[GLFW_KEY_S])
        camera->moveForward(-0.7f);
    if (heldKeys[GLFW_KEY_D])
        camera->moveLeft(0.5f);
    if (heldKeys[GLFW_KEY_A])
        camera->moveLeft(-0.5f);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCREEN_W = (float)width;
    SCREEN_H = (float)height;
    projection = glm::perspective(glm::radians(FOV), (GLfloat)(SCREEN_W / SCREEN_H), NEAR_CLIP, FAR_CLIP);

    glViewport(0, 0, width, height);
}
