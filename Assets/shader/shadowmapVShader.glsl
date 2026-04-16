#version 330

// Input vertex attributes
in vec3 vertexPosition;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matLightVP;

// Output vertex attributes (to fragment shader)
out vec4 lightSpace;

void main()
{
    // Send vertex attributes to fragment shader
    lightSpace = matLightVP * vec4(vertexPosition, 1.0);
    gl_Position = lightSpace;
}
