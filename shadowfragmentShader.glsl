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
    vec4 textureMesh = texture(texture0, fragTexCoord);
    vec3 result = vec3(1);
    vec3 N = normalize(fragNormal);
    vec3 L = normalize(lightPos - fragPosition);

    vec4 fragPosLightSpace = lightViewProj * vec4(fragPosition, 1);
    fragPosLightSpace.xyz /= fragPosLightSpace.w;
    fragPosLightSpace.xyz = (fragPosLightSpace.xyz+1.0)/2.0f;

    vec2 shadowUVCoord = fragPosLightSpace.xy;
    float shadowDepth = fragPosLightSpace.z;
    float shadow     = texture(shadow0, shadowUVCoord).x;

    float bias = max(0.0002*(1.0 - dot(N, L)), 0.00002) + 0.00001;
    int shadowCounter = 0;
    vec2 texelSize = vec2(1.0/float(shadowMapResolution));
    float sampleDepth = texture(shadow0, shadowUVCoord).r;

    float shadowcount = 0;

    for(int x = -1; x < 1 ; ++x)
    {
        for(int y = -1; y < 1; ++y)
        {
            float shadow_nn = texture(shadow0, shadowUVCoord+(vec2(x,y)/texelSize)).x;
            if( shadow_nn <= bias)
            {
                shadowcount++;
                shadow += shadow_nn;
            }
        }
    }

    if(shadowcount > 3)
    {
        result += max(shadow, 0.00001)/shadowcount;
    }else{
        result = vec3(0);
    }
    finalColor = vec4(result, textureMesh.a);
}
