#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
in vec3 fragPosition;

// Input uniform values
uniform sampler2D texture0;
uniform sampler2D shadow0;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float shadowMapResolution;
uniform mat4 lightViewProj;
uniform float amplitude;

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
    float shadowDepth = 1.0f - (fragPosLightSpace.z);
    float shadow     = 0;

    float bias = max(0.005*(1.0 - dot(N, L)), 0.00002) + 0.00001;
    int shadowCounter = 0;
    vec2 texelSize = vec2(1.0/shadowMapResolution);

    float rec = shadowDepth;
    float blo = 0;
    int blockerCount = 0;
    float blockRange = 2.0f;
    for(float x = -blockRange; x <= blockRange; x+= 1.0f)
    {
	for(float y = -blockRange; y <= blockRange; y+= 1.0f)
	{
	    float shadow_nn = texture(shadow0, shadowUVCoord+(vec2(x,y)*texelSize)).x;
	    if(shadowDepth > shadow_nn)
	    {
		    blo += shadow_nn;
		    blockerCount++;
	    }
	}
    }

    int range = 2;
    if(blockerCount > 0)
    {
	blo = blo/blockerCount;
	float pcff_range = (rec - blo)*(amplitude/blo);
	pcff_range *= 1.0f;
	//range is still not right 

	range = int(pcff_range);
	range = clamp(range, 1, 15);
    }else
    {
	finalColor = vec4(0.0);
	return;
    }

    for(int x = -range; x <= range; ++x)
    {
        for(int y = -range; y <= range; ++y)
        {
            float shadow_nn = texture(shadow0, shadowUVCoord+(vec2(x,y)*texelSize)).x + 0.001f;
	    if(shadowDepth > shadow_nn){
                shadow += shadow_nn;
            }
        }
    }
    if(shadow > 0.01f){
	    result = vec3(1.0f - (shadow/((1+range+range)*(1+range+range))));
    }else{
	result = vec3(0.0f);
    }
    result = clamp(result, vec3(0), vec3(1));
    //result is too dark and does not behave as intended
    result = clamp(1.0f - result*result, vec3(0.01f),vec3(0.99));
    result = mix(lightColor, vec3(1.0f), result.x);

    finalColor = vec4(result, 1.0f);
}
