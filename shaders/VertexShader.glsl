#version 440 core

layout (location = 0) in vec2 aPos; // Posición del punto
out vec2 texCoords;                // Coordenadas de textura normalizadas

uniform float canvasWidth;         // Ancho del lienzo
uniform float canvasHeight;        // Alto del lienzo
uniform float pointSize;           // Tamaño de los puntos 

void main() {
    // Convertir coordenadas de espacio de ventana a espacio de textura
    texCoords = vec2(aPos.x / canvasWidth, aPos.y / canvasHeight);

    // Convertir coordenadas al sistema de clip para OpenGL
    float xClip = (aPos.x / canvasWidth) * 2.0 - 1.0;
    float yClip = (aPos.y / canvasHeight) * 2.0 - 1.0;
    gl_Position = vec4(xClip, yClip, 0.0, 1.0);

    // Establecer tamaño de los puntos
    gl_PointSize = pointSize;
}
