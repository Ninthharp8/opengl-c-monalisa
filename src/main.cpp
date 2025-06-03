#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb/stb_image.h"
#include "../include/common/shader.hpp"

#define M_PI 3.14159265358979323846264338327950288

// Constantes
const int POINT_COUNT = 25000;
const float CANVAS_WIDTH = 768.0f; // x
const float CANVAS_HEIGHT = 1080.0f; // y
float POINT_SIZE = 2.5f;

// Estructura para almacenar coordenadas y velocidades
struct Point {
    float x, y;       // Coordenadas del punto
    float vx, vy;     // Velocidades
};

// Función para cargar textura
GLuint loadTexture(const char* filePath) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// Generar datos iniciales para puntos
std::vector<Point> generatePoints() {
    std::vector<Point> points(POINT_COUNT);
    for (int i = 0; i < POINT_COUNT; ++i) {
        points[i].x = static_cast<float>(std::rand()) / RAND_MAX * CANVAS_WIDTH;
        points[i].y = static_cast<float>(std::rand()) / RAND_MAX * CANVAS_HEIGHT;

        float velocity = 15 + 20 * (static_cast<float>(std::rand()) / RAND_MAX);
        float angle = 2 * M_PI * (static_cast<float>(std::rand()) / RAND_MAX);
        points[i].vx = velocity * std::cos(angle);
        points[i].vy = velocity * std::sin(angle);
    }
    return points;
}

// Actualizar posiciones de los puntos
void updatePoints(std::vector<Point>& points, float deltaTime) {
    for (auto& point : points) {
        point.x += point.vx * deltaTime;
        point.y += point.vy * deltaTime;

        if (point.x < 0 || point.x > CANVAS_WIDTH) {
            point.vx = -point.vx;
            point.x = std::max(0.0f, std::min(point.x, CANVAS_WIDTH));
        }
        if (point.y < 0 || point.y > CANVAS_HEIGHT) {
            point.vy = -point.vy;
            point.y = std::max(0.0f, std::min(point.y, CANVAS_HEIGHT));
        }
    }
}

int main() {
    // Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(CANVAS_WIDTH, CANVAS_HEIGHT, "Texel Fetch Demo", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    glEnable(GL_PROGRAM_POINT_SIZE);

    GLuint shaderProgram = LoadShaders("../shaders/VertexShader.glsl", "../shaders/FragmentShader.glsl");
    GLuint texture = loadTexture("../shaders/monalisa.png");

    // Generar puntos iniciales
    std::vector<Point> points = generatePoints();

    // Configuración de buffers
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), points.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    float lastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClear(GL_COLOR_BUFFER_BIT);
        

        glUseProgram(shaderProgram);

        glUniform1f(glGetUniformLocation(shaderProgram, "canvasWidth"), CANVAS_WIDTH);
        glUniform1f(glGetUniformLocation(shaderProgram, "canvasHeight"), CANVAS_HEIGHT);
        glUniform1f(glGetUniformLocation(shaderProgram, "pointSize"),POINT_SIZE);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        updatePoints(points, deltaTime);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(Point), points.data());

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, POINT_COUNT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}
