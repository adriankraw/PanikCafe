#version 330

in vec4 lightSpace;

out vec4 finalColor;

float minclamp = 1.0f;
float maxclamp = 60.0f;

float LinearizeDepth(float depth)
{
	float range = maxclamp - minclamp;
	float z = depth - minclamp;
    return z/100.0f;
}

void main()
{
    //finalColor = fragColor * texture(texture0, fragTexCoord);
    float depth = (lightSpace.z/lightSpace.w) + minclamp;

	//depth = LinearizeDepth(depth);
    gl_FragDepth = depth;
	finalColor = vec4(depth);
}
