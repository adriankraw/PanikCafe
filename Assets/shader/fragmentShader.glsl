#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
in vec3 fragPosition;

// Input uniform values
uniform sampler2D texture0;
uniform float noTexture0;
uniform sampler2D roughness;
uniform float roughness1;
uniform sampler2D metalness;
uniform float metalic1;
uniform sampler2D normal;

uniform float fragTime;
uniform vec3 viewPos;
uniform vec3 viewDir;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightstrength;
uniform float selected;

// Output fragment color
out vec4 finalColor;

vec3 ambientColor = vec3(1.0f,1.0f,1.0f);
const float PI = 3.14159265359;
const float alphaCutout = 0.2f;
const float minclamp = 0.0001f;
const float maxclamp = 100.0f;


float D(float alpha, vec3 N, vec3 H)
{
    float numerator = pow(alpha, 2.0);
    float NdotH = max(dot(N, H), 0.0);
    float denominator = PI * pow(pow(NdotH,2.0) * (pow(alpha, 2.0) - 1.0)+ 1.0, 2.0);
    denominator = max(denominator, 0.000001);
    return numerator / denominator;
}
float G1(float alpha, vec3 N, vec3 X)
{
    float numerator = max(dot(N, X), 0.00001f);
    //float k = alpha / 2.0f;
    float k = pow(alpha+0.00001, 2) / 2.0;
    float denominator = numerator * (1.0 - k) + k;

    return numerator / max(denominator, 0.0001);
}
float G(float alpha, vec3 N, vec3 V, vec3 L)
{
    return G1(alpha, N, V) * G1(alpha, N, L);
}
vec3 F(vec3 F0, vec3 V, vec3 L, vec3 H)
{
    //return F0 + (vec3(1.0) - F0) * pow(1.0 - max(dot(V, H), 0.0), 5.0);
    vec3 basecolor = max(texture(texture0,  fragTexCoord), 0.01).xyz / PI;
    basecolor *= (1 + (F0.x - 1)* pow(max(dot(V, H), 0.0), 5.0));
    basecolor *= (1 + (F0.x - 1)* pow(max(dot(L, H), 0.0), 5.0));
    return basecolor;
}

void main()
{
    if (selected == 1)
    {
        finalColor = vec4(vec3(1, 0, 0) ,1.0f );
        return;
    }
    vec3 ambient = ambientColor * 0.03f;

    vec4 textureMesh = texture(texture0, fragTexCoord);
    vec3 normalMap = max(texture(normal, fragTexCoord).rgb,0.001);

    if(noTexture0 == 1){
        textureMesh = fragColor;
    }
    if(textureMesh.a <= 0.001){
        discard;
    }
    float roughness  = max(texture(roughness,fragTexCoord).x,0.01);
    if (roughness1 > 0) roughness = roughness1;
    float metalic    = max(texture(metalness,  fragTexCoord).x,0.01);
    if (metalic1 > 0) metalic = metalic1;
    vec3 result = vec3(0, 0, 0);

    int lightIndex = 0;

    vec3 albedoMesh = textureMesh.xyz * fragColor.xyz;
    float alpha = clamp(max(roughness * roughness,0.0),0.0,1.0f);

    float lightdistance = length(lightPos - fragPosition);
    float attenuation = 1.0/(lightdistance * lightdistance);
    vec3 radiance = (lightColor * 20.0f)/3.0f * lightstrength * attenuation;
    vec3 emissivityMesh = vec3(0.0);

    vec3 N = normalize(fragNormal + normalMap);
    vec3 L = normalize(lightPos - fragPosition);
    vec3 V = normalize(viewPos - fragPosition);
    vec3 H = normalize(L + V);
    vec3 F0= mix(vec3(0.04), albedoMesh, metalic);
    //vec3 F0= vec3(0.5 + 2*roughness*pow(0.04,2));

    vec3 Ks = F(F0, V, L, H); // specular
    vec3 Kd = vec3(1.0) - Ks; //diffuse
    Kd *= (1.0 - metalic);

    vec3 lambert = albedoMesh.xyz / PI;
    vec3 cookTorranceNumerator = D(alpha, N, H) * G(alpha, N, V, L) * F(F0, V, L, N);
    float cookTorranceDenominator = 4.0 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0) + 0.00001;
    vec3 cookTorrance = cookTorranceNumerator / cookTorranceDenominator; 

    vec3 BRDF = Kd * lambert + cookTorrance;
    vec3 outgoingLight = emissivityMesh + BRDF * radiance * 20 * max(dot(L, N), 0.0);

    result += outgoingLight;

    //result = textureMesh.xyz*ambient;
    result = clamp(result, vec3(0), vec3(1.0f));
    finalColor = vec4(result, textureMesh.a);
}
