#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;
uniform mat4 matLightVP;
uniform vec3 lightPos;

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out float fragShadowDepth;
out vec2 fragShadowTexCoord;

// NOTE: Add your custom variables here

void main()
{
    // Calculate final vertex position
    gl_Position = mvp*vec4(vertexPosition, 1.0);

    // Send vertex attributes to fragment shader
    fragPosition = vec3(matModel*vec4(vertexPosition, 1.0));
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    vec4 worldSpace = matModel * vec4(vertexPosition, 1.0f);
    vec4 screenSpace = matLightVP*worldSpace;
    fragShadowDepth = screenSpace.z/screenSpace.w;
    vec3 lightDir = -lightPos;
    fragShadowDepth *= ((lightDir.x*fragPosition.x)+(lightDir.y*fragPosition.y)+(lightDir.z*fragPosition.z));
    fragShadowTexCoord = (screenSpace.xy/screenSpace.w)*0.5f+0.5f;
}
