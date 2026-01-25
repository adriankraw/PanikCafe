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
    float shadowDepth = (fragPosLightSpace.z)+0.5f;
    float shadow     = 0;

    float bias = max(0.005*(1.0 - dot(N, L)), 0.00002) + 0.00001;
    int shadowCounter = 0;
    vec2 texelSize = vec2(1.0/float(2048));
    float sampleDepth = texture(shadow0, shadowUVCoord).r;

	float rec = shadowDepth;
	float blo = 0;
	int blockerCount = 0;
    float blockRange = 3.0f;
	for(float x = -blockRange; x <= blockRange; x+= 1.0f)
	{
		for(float y = -blockRange; y <= blockRange; y+= 1.0f)
		{
			float shadow_nn = texture(shadow0, shadowUVCoord+(vec2(x,y)*texelSize)).x;
			if(shadowDepth > shadow_nn)
			{
				blo += 1.0f;
				blockerCount++;
			}
		}
	}

	blo= blo/blockerCount;
	if(blockerCount == 0)
	{	
		finalColor = vec4(vec3(1),1); 
		return;
	}
	float pcff_range = (rec - blo)*2.0f/blo;

	int range = int(pcff_range);
	range = clamp(range, 2, 16);

    for(int x = -range; x <= range; ++x)
    {
        for(int y = -range; y <= range; ++y)
        {
            float shadow_nn = texture(shadow0, shadowUVCoord+(vec2(x,y)*texelSize)).x;
			if(shadowDepth > shadow_nn)			if(shadowDepth > shadow_nn)
            {
                shadow += shadow_nn;
            }
        }
    }
	if(shadow > 0){
		result = vec3(shadow/((1+range+range)*(1+range+range)));
	}


    finalColor = vec4(result, 1);
}
