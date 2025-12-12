#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
in vec3 fragPosition;

// Input uniform values
uniform sampler2D texture0;
uniform sampler2D shadow0;

uniform float fragTime;
uniform vec3 viewPos;
uniform vec3 viewDir;
uniform vec3 lightPos;
uniform float shadowMapResolution;
uniform mat4 lightViewProj;

// Output fragment color
out vec4 finalColor;

void main()
{
    //use previous pass to generate shadows with every light on each own. generate one texture to export to cpu
    vec3 result = vec3(1);

    //shadow
    vec4 fragPosLightSpace = lightViewProj[i] * vec4(fragPosition, 1);
    fragPosLightSpace.xyz /= fragPosLightSpace.w;
    fragPosLightSpace.xyz = (fragPosLightSpace.xyz+1.0)/2.0f;

    vec2 shadowUVCoord = fragPosLightSpace.xy;
    float shadowDepth = fragPosLightSpace.z;
    float shadow     = texture(shadow0[i], shadowUVCoord).x;

    float bias = max(0.0002*(1.0 - dot(N, L)), 0.00002) + 0.00001;
    vec2 texelSize = vec2(1.0/float(shadowMapResolution));

    if((shadow <= bias || shadow < 1))
    {
        result *= shadow;
    }

    finalColor = vec4(result , textureMesh.a);
}
