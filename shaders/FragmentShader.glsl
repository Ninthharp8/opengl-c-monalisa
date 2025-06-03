#version 440 core

in vec2 texCoords;           // Coordenadas de textura
out vec4 FragColor;          // Color del fragmento

uniform sampler2D texture1;  // Textura de la Mona Lisa

void main() {
    // Obtener el color de la textura en las coordenadas actuales
    FragColor = texture(texture1, texCoords);
}
