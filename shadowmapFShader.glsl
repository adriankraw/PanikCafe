#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in float fragShadowDepth;
in vec2 fragShadowTexCoord;

uniform sampler2D texture0;

out vec4 finalColor;
uniform vec3 lightDir;
uniform vec4 lightColor;

uniform int shadowMapResolution;

float minclamp = 0.00001f;
float maxclamp = 100.0f;

void main()
{
    finalColor = fragColor * texture(texture0, fragTexCoord);
    float depth = fragShadowDepth;
	depth = normalize(gl_FragCoord).z;
    depth = 1.0/depth - 1.0/minclamp;
    depth /= (1/maxclamp-1/minclamp);
    gl_FragDepth = (depth);
    finalColor = vec4(vec3(depth),1.0f);
}
