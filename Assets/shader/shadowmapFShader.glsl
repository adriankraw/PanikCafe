#version 330

in vec4 lightSpace;

out vec4 finalColor;

uniform float minclamp = 1.0f;
uniform float maxclamp = 60.0f;

float LinearizeDepth(float depth)
{
	float range = maxclamp - minclamp;
	float z = depth * range - minclamp;
    return z;
}

void main()
{
    //finalColor = fragColor * texture(texture0, fragTexCoord);
    float depth = (lightSpace.z/lightSpace.w)+minclamp;

    //depth = LinearizeDepth(depth);
    depth = clamp(depth, 0.01f,0.999f);
    gl_FragDepth = depth;
    finalColor = vec4(1.0f - depth);
}
