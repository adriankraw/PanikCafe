#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform sampler2D colormap;

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
    //texelColor.a = length(texelColor.xyz);
    //texelColor = texture(colormap, vec2(clamp(texelColor.a,0.0f,1.0f), 0.0f));

    /*
    int samplerCount = 1;

    texelColor = vec4(0);
    samplerCount = 1;
    int testRange = 2;
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
    if(length(texelColor.rgb) <= 1.0f)
    {
        texelColor = texture(texture0, uv);
    }else{
        texelColor = vec4(0);
    }
    */
    //texelColor = texture(texture0, uv);
    /*
    if(clamp((texelColor.r+texelColor.g+texelColor.b)/3.0f, 0.0f, 1.0f) >= 0.99f)
    {
        int blurRange = 4;
        for(int x = -blurRange; x <= blurRange; x++)
        {
            texelColor += texture(texture0,vec2(uv.x+x/width,uv.y));
        }
        for(int y = -blurRange; y <= blurRange; y++)
        {
            texelColor += texture(texture0,vec2(uv.x,uv.y+y/height));
        }
        texelColor /= 2*(1 + 2*blurRange);
    }
    */
    vec3 result = texelColor.rgb;

    //result = floor(result * colorRes) / colorRes;
    finalColor = vec4(result, 1.0f);
}
