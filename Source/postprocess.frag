#version 410 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec2 resolution;
uniform float time;

vec2 offset = vec2(1.0/resolution.x, 1.0/resolution.y);

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    
    vec2 offsets[9] = vec2[](
            vec2(-offset.x,  offset.y), // top-left
            vec2( 0.0f,    offset.y), // top-center
            vec2( offset.x,  offset.y), // top-right
            vec2(-offset.x,  0.0f),   // center-left
            vec2( 0.0f,    0.0f),   // center-center
            vec2( offset.x,  0.0f),   // center-right
            vec2(-offset.x, -offset.y), // bottom-left
            vec2( 0.0f,   -offset.y), // bottom-center
            vec2( offset.x, -offset.y)  // bottom-right
        );
    
    float kernel[9] = float[](
        -1.0, -1.0, -1.0,
        -1.0, 8.0, -1.0,
        -1.0, -1.0, -1.0
    );
        
        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
        }
        col = vec3(0.0);
        for(int i = 0; i < 9; i++)
            col += sampleTex[i] * kernel[i];
        
    FragColor = vec4(/*col*/texture(screenTexture, TexCoords.st).rgb, 1.0);
} 
