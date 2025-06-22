#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float scared;

// Output fragment color
out vec4 finalColor;

void main()
{
    vec2 uv = fragTexCoord;
    float width = 1280;
    float height = 720;
    float colorRes = 4196;
    float colorDiff = 0.2f;
    
    vec4 texelColor = texture(texture0, uv);
    int samplerCount = 1;

    texelColor = vec4(0);
    samplerCount = 0;
    int testRange = 1;
    for(int x = -testRange; x <= testRange; x++)
    {
        for(int y = -testRange; y <= testRange; y++)
        {
            vec2 m_uv = uv;
            uv.x += x/width;
            uv.y += y/height;
            vec4 m_color = texture(texture0, m_uv);
            if (abs(texelColor.x-m_color.x)>colorDiff || abs(texelColor.y-m_color.y)>colorDiff || abs(texelColor.z-m_color.z)>colorDiff)
            {
                texelColor += m_color;
                samplerCount++;
            }
        }
    }
    texelColor /= samplerCount;
    if(length(texelColor.rgb) <= 10.0f)
    {
        texelColor = texture(texture0, uv);
    }else{
        texelColor = vec4(0);
    }
    //texelColor = texture(texture0, uv);
    vec3 result = texelColor.rgb;
    //result = floor(result * colorRes) / colorRes;
    finalColor = vec4(result, 1.0f);
}
