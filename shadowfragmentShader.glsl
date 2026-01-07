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

    float bias = max(0.005*(1.0 - dot(N, L)), 0.00002) + 0.00001;
    int shadowCounter = 0;
    vec2 texelSize = vec2(1.0/float(shadowMapResolution));
    float sampleDepth = texture(shadow0, shadowUVCoord).r;

	float rec = length(L);
	float blo = shadowDepth;
	float pcff_range = (rec - blo)*0.015/blo;

	int range = int(pcff_range);
	range = range<1 ?2 :range;
	range = range>40?40:range;

    for(int x = -range; x < range; ++x)
    {
        for(int y = -range; y < range; ++y)
        {
            float shadow_nn = texture(shadow0, shadowUVCoord+(vec2(x,y)*texelSize)).x;
			if(shadowDepth - bias > shadow_nn)
            {
                shadow += 1.0f;
            }
        }
    }

    result = vec3(shadow/pow(range+1.0f+range,2));

    if(shadowDepth > 1.0)
    {
        result = vec3(0);
    }

    finalColor = vec4(result, textureMesh.a);
}
