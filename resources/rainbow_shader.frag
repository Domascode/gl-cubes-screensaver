#version 330 core
in vec3 ourColor;
out vec4 FragColor;

uniform float t;
uniform float speed;
uniform float fragSpeed;
uniform float x;
uniform float y;
uniform float z;

void main()
{
    vec3 dynamicColor = ourColor * 0.5 + 0.5 * vec3(sin(t * fragSpeed), sin(t * fragSpeed + 8), sin(t * fragSpeed + 16));
    FragColor = vec4(dynamicColor, 1.0);
}